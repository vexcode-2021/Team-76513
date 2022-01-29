#include "../shared.hpp"

#pragma once

void front_line_up(okapi::QTime timeout = 4_s, Vision::MOGO type = Vision::ANY, bool otherline = false)
{

	auto distancecontroller = okapi::IterativePosPIDController({0.0003, 0.00019, 0.000005}, okapi::TimeUtilFactory().withSettledUtilParams(15, 9999, 150_ms));
	auto anglecontroller = okapi::IterativePosPIDController({0.00020 * 2, 0, 0.000001}, okapi::TimeUtilFactory().withSettledUtilParams(15, 5, 150_ms));

	auto [_, _1, _2, _3, w] = Visions[Vision::FRONT]->get_mogo(type);

	anglecontroller.setTarget(158);
	distancecontroller.setTarget(30);
	int count = 0;
	do
	{
		count++;
		errno = 0;
		auto [_, top, x_mid, y_mid, _1] = Visions[Vision::FRONT]->get_mogo(w);
		auto geterr = errno;
		auto distvel = -distancecontroller.step(ultrasonic.get());
		auto yawvel = -1 * anglecontroller.step(x_mid);
		// yawvel = 0;
		if (geterr != 0)
		{
			yawvel = 0;
			// distvel = 0;
		}

		if (otherline)
		{
			if (drive.chassis->getState().y > 4_tile - 6_in)
			{
				drive.chassis->getModel()->driveVectorVoltage(-12, 0);
				return;
			}
		}

		drive.chassis->getModel()->driveVectorVoltage(12.0 * distvel, 12.0 * yawvel);

		pros::delay(20);
		//		printf("LOP: %f %d %f\n", anglecontroller.getProcessValue(), x_mid, yawvel);
		//		printf("LOP: %f %d %f\n", distancecontroller.getProcessValue(), ultrasonic.get(), distvel);
		//		printf("%d %d\n", anglecontroller.isSettled(), distancecontroller.isSettled());
	} while (!((anglecontroller.isSettled() && distancecontroller.isSettled()) || count > timeout.convert(1_ms) / 20.0));

	anglecontroller.flipDisable();
	distancecontroller.flipDisable();
}

void back_line_up(okapi::QTime timeout = 2_s, Vision::MOGO type = Vision::ANY)
{

	auto anglecontroller = okapi::IterativePosPIDController({0.00020 * 1.8, 0, 0.000001}, okapi::TimeUtilFactory().withSettledUtilParams(15, 5, 150_ms));


	auto [_, _1, _2, _3, w] = Visions[Vision::BACK]->get_mogo(type);

	anglecontroller.setTarget(158);
	int count = 0;
	do
	{
		count++;
		errno = 0;
		auto [_, top, x_mid, y_mid, _1] = Visions[Vision::BACK]->get_mogo(w);
		auto geterr = errno;
		auto yawvel = +1 * anglecontroller.step(x_mid);
		if (geterr != 0)
			yawvel = 0;
		drive.chassis->getModel()->driveVectorVoltage(0, 12.0 * yawvel);

		pros::delay(20);
		printf("LOP: %f %d %f %d\n", anglecontroller.getProcessValue(), x_mid, yawvel, geterr);
	} while (!(anglecontroller.isSettled() || count > timeout.convert(1_ms) / 20.0));

	anglecontroller.flipDisable();
}

void front_intake(okapi::QTime timeout = 4_s, Vision::MOGO type = Vision::ANY, bool otherline = false)
{
	claw.ArmSetNum(0);
	claw.Leave();
	claw.Leave();
	claw.Leave();
	claw.WaitUntilSettled();

	front_line_up(timeout, type, otherline);
	claw.Clasp();
	drive.chassis->getModel()->driveVector(0, 0);

	claw.ArmSetRelative(3);
}