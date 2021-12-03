//double vision(int sig)
//{
//	auto c = sig == 1 ? Vision.YELLOW : Vision.ALLIANCE;
//
//	return std::get<0>(Vision.get_mogo_target_diff(c, Vision.FRONT, VISION_FOV_WIDTH / 2, 0));
//}

void front_intake()
{

	claw.ArmSetNum(0);
	claw.ArmSetRelative(-1);
	claw.Leave();
	claw.WaitUntilSettled();

	auto distancecontroller = okapi::IterativePosPIDController({0.0007, 0.003, 0.000001}, okapi::TimeUtilFactory().withSettledUtilParams(7));
	auto anglecontroller = okapi::IterativePosPIDController({0.0005, 0, 0}, okapi::TimeUtilFactory().withSettledUtilParams(6));

	distancecontroller.setTarget(173);
	anglecontroller.setTarget(149);
	int count = 0;
	do
	{
		count++;
		auto [_, top, x_mid, y_mid] = Visions[Vision::FRONT]->get_mogo(Vision::ANY);
		auto distvel = distancecontroller.step(y_mid);
		auto yawvel = anglecontroller.step(x_mid);
		//distvel = 0;

		drive.chassis->getModel()->driveVector(distvel, -yawvel);

		pros::delay(50);
		printf("LOP: %f %d %f\n", anglecontroller.getProcessValue(), x_mid, -yawvel);
	} while (!((anglecontroller.isSettled() && distancecontroller.isSettled()) || drive.current_drive_mode == DRIVER_CONTROLLER || count > 4 * 1000.0 / 50.0));

	claw.Clasp();
	drive.chassis->getModel()->driveVector(0, 0);

	claw.ArmSetRelative(3);
	printf("FINISHED\n");
}

//void pathfinder()
//{
//	std::shared_ptr<okapi::AsyncMotionProfileController> profileController =
//		okapi::AsyncMotionProfileControllerBuilder()
//			.withLimits({
//				1.0, // Maximum linear velocity of the Chassis in m/s
//				2.0, // Maximum linear acceleration of the Chassis in m/s/s
//				10.0 // Maximum linear jerk of the Chassis in m/s/s/s
//			})
//			.withOutput(drive.chassis)
//			.buildMotionProfileController();
//	profileController->generatePath(
//		{{0_ft, 0_ft, 0_deg}, {4_ft, 1_ft, 0_deg}}, "A");
//	profileController->setTarget("A");
//	profileController->waitUntilSettled();
//}

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
		auto [_, _1, x_mid, _2] = Visions[Vision::BACK]->get_mogo(Vision::ANY);
		auto distvel = distancecontroller.step(ultrasonic.get());
		auto yawvel = -anglecontroller.step(x_mid);
		distvel = 0;

		drive.chassis->getModel()->driveVector(distvel, yawvel);

		pros::delay(100);
		printf("LOP %f\n", yawvel);
	} while (!((anglecontroller.isSettled() && distancecontroller.isSettled()) || drive.current_drive_mode == DRIVER_CONTROLLER));
}

void back_up()
{
	printf("RUNNING\n");
	//back_intake();
	//pathfinder();
	front_intake();
	printf("RUNNIGENEDED\n");
}

void auton_awp()
{
	//	drive.chassis->setMaxVelocity(40);
	//	claw.ArmSet(40);
	//	claw.WaitUntilSettled();
	//	drive.chassis->moveDistance(9_in);
	//	drive.chassis->setMaxVelocity(20);
	//
	//	auto exampleController = okapi::IterativeControllerFactory::posPID(0.005, 0.001, 0);
	//	exampleController.setTarget(130);
	//	while (!exampleController.isSettled())
	//	{
	//		double in = ultrasonic.controllerGet();
	//		double newOutput = exampleController.step(in);
	//
	//		static int count = 0;
	//		count++;
	//		if (!(count % 30))
	//			printf("%f:%f\n", in, newOutput);
	//		drive.chassis->getModel()->forward(-newOutput);
	//		pros::delay(10);
	//	}
	//	//drive.chassis->moveDistance(1_in);
	//	printf("REACHED\n");
	//
	//	claw.Clasp();
	//
	//	pros::delay(300);
	//	//drive.chassis->moveDistance(-8_in);
	//
	//	claw.Leave();
	//	//claw.ArmSet(2);
	//	//pros::delay(300);
	//	//claw.WaitUntilSettled();
	//
	//	//drive.chassis->turnAngle((vision(2) / 6) * 1.0_deg);
	//	drive.chassis->turnAngle(15_deg);
	//	drive.chassis->moveDistance(16_in);
	//	printf("MOVEDforw7\n");
	//	//claw.Clasp();
	//	//printf("GRABBED\n");
	//
	//	//drive.chassis->setMaxVelocity(50);
	//	//drive.chassis->moveDistance(-1.3_ft);
	//	//claw.Leave();
}

void auton_awp2()
{
	//	drive.chassis->setMaxVelocity(20);
	//	claw.ArmSet(40);
	//	claw.WaitUntilSettled();
	//	drive.chassis->moveDistance(10_in);
	//
	//	auto exampleController = okapi::IterativeControllerFactory::posPID(0.005, 0.001, 0);
	//	exampleController.setTarget(145);
	//	while (!exampleController.isSettled())
	//	{
	//		double in = ultrasonic.controllerGet();
	//		double newOutput = exampleController.step(in);
	//
	//		static int count = 0;
	//		count++;
	//		if (!(count % 30))
	//			printf("%f:%f\n", in, newOutput);
	//		drive.chassis->getModel()->forward(-newOutput);
	//		pros::delay(10);
	//	}
	//	printf("REACHED\n");
	//
	//	claw.Clasp();
	//	pros::delay(600);
	//	claw.Leave();
}

void auton_yellow()
{
	//	printf("AUTON_YELLOW\n");
	//	drive.chassis->setMaxVelocity(120);
	//	claw.ArmSet(2);
	//	printf("1\n");
	//	drive.chassis->moveDistance(36_in);
	//	pros::delay(100);
	//	drive.chassis->setMaxVelocity(30);
	//	drive.chassis->turnAngle((vision(1) / 3.8) * 1.0_deg);
	//	printf("4\n");
	//	drive.chassis->moveDistance(14_in);
	//
	//	printf("REACHED\n");
	//
	//	claw.Clasp();
	//	drive.chassis->setMaxVelocity(120);
	//	pros::delay(300);
	//	drive.chassis->moveDistance(-45_in);
	//
	//	claw.Leave();
}

void auton_yellow_mid() //TODO
{
	//	printf("AUTON_YELLOW_MID\n");
	//	drive.chassis->setMaxVelocity(120);
	//	claw.ArmSet(2);
	//	printf("1\n");
	//	drive.chassis->moveDistance(21_in);
	//	drive.chassis->setMaxVelocity(30);
	//	drive.chassis->turnAngle(-47.5_deg);
	//	drive.chassis->setMaxVelocity(120);
	//	drive.chassis->moveDistance(32.49_in);
	//	pros::delay(200);
	//	printf("2\n");
	//	drive.chassis->waitUntilSettled();
	//	drive.chassis->setMaxVelocity(30);
	//	printf("4\n");
	//
	//	drive.chassis->turnAngle((vision(1) / 3.8) * 1.0_deg);
	//
	//	pros::delay(300);
	//	drive.chassis->moveDistance(13_in);
	//
	//	printf("REACHED\n");
	//
	//	pros::delay(200);
	//	claw.Clasp();
	//	pros::delay(200);
	//	drive.chassis->setMaxVelocity(90);
	//	drive.chassis->moveDistance(-53_in);
	//
	//	claw.Leave();
}
