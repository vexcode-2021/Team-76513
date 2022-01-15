#include "../shared.hpp"

#pragma once

void front_line_up(double timeout_seconds = 4, double multiplier = 1, double dmult = 1, bool loose = false, Vision::MOGO type = Vision::ANY)
{
	auto distancecontroller = okapi::IterativePosPIDController({multiplier * 0.0010, 0.00003, dmult * 0.000001}, okapi::TimeUtilFactory().withSettledUtilParams(loose ? 15 : 7, 5, 150_ms));
	auto anglecontroller = okapi::IterativePosPIDController({multiplier * 0.00020 * 2, 0, 0}, okapi::TimeUtilFactory().withSettledUtilParams(loose ? 15 : 6, 5, 150_ms));

	auto [_, _1, _2, _3, w] = Visions[Vision::FRONT]->get_mogo(type);
	if (w == Vision::RED)
	{
		anglecontroller.setTarget(160);
		distancecontroller.setTarget(192);
	}
	else if (w == Vision::BLUE)
	{
		anglecontroller.setTarget(175);
		distancecontroller.setTarget(175);
	}
	else
	{
		anglecontroller.setTarget(170);
		distancecontroller.setTarget(185);
	}
	int count = 0;
	do
	{
		count++;
		errno = 0;
		auto [_, top, x_mid, y_mid, _1] = Visions[Vision::FRONT]->get_mogo(type);
		auto geterr = errno;
		auto distvel = distancecontroller.step(y_mid);
		auto yawvel = -1 * -1 * anglecontroller.step(x_mid);
		distvel = 0;
		if (geterr != 0)
		{
			yawvel = 0;
			distvel = 0;
		}

		drive.chassis->getModel()->driveVector(distvel, yawvel);

		pros::delay(20);
		// printf("LOP: %f %d %f\n", anglecontroller.getProcessValue(), x_mid, yawvel);
		// printf("LOP: %f %d %f\n", distancecontroller.getProcessValue(), y_mid, distvel);
	} while (!((anglecontroller.isSettled() && distancecontroller.isSettled()) || count > timeout_seconds * 1000.0 / 20.0));

	anglecontroller.flipDisable();
	distancecontroller.flipDisable();
}
void front_intake(double timeout_seconds = 4, double multiplier = 1, double dmult = 1, bool loose = false, Vision::MOGO type = Vision::ANY)
{

	claw.ArmSetNum(0);
	claw.ArmSetRelative(-1);
	claw.Leave();
	claw.WaitUntilSettled();

	front_line_up(timeout_seconds, multiplier, dmult, loose, type);

	claw.Clasp();
	drive.chassis->getModel()->driveVector(0, 0);

	claw.ArmSetRelative(3);
	printf("FINISHED\n");
}

void back_intake()
{
	back_claw.ArmSetNum(BACK_CLAW_CONF::ARM_POS_LEN - 1);
	back_claw.WaitUntilSettled();

	auto distancecontroller = okapi::IterativeControllerFactory::posPID(0.0017, 0.029, 0.001);
	auto anglecontroller = okapi::IterativeControllerFactory::posPID(0.0009, 0.001, 0.000000);

	distancecontroller.setTarget(40);
	anglecontroller.setTarget(180);
	do
	{
		auto [_, _1, x_mid, _2, _3] = Visions[Vision::BACK]->get_mogo(Vision::ANY);
		auto distvel = distancecontroller.step(ultrasonic.get());
		auto yawvel = -anglecontroller.step(x_mid);
		distvel = 0;

		drive.chassis->getModel()->driveVector(distvel, yawvel);

		pros::delay(100);
		printf("LOP %f\n", yawvel);
	} while (!((anglecontroller.isSettled() && distancecontroller.isSettled())));
}