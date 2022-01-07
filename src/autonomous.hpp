// double vision(int sig)
//{
//	auto c = sig == 1 ? Vision.YELLOW : Vision.ALLIANCE;
//
//	return std::get<0>(Vision.get_mogo_target_diff(c, Vision.FRONT, VISION_FOV_WIDTH / 2, 0));
// }

void front_line_up(double timeout_seconds = 4, double multiplier = 1, double dmult = 1, bool loose = false, Vision::MOGO type = Vision::ANY)
{
	auto distancecontroller = okapi::IterativePosPIDController({multiplier * 0.0010, 0.00003, dmult * 0.000001}, okapi::TimeUtilFactory().withSettledUtilParams(loose ? 15 : 7, 5, 150_ms));
	auto anglecontroller = okapi::IterativePosPIDController({multiplier * 0.00020, 0, 0}, okapi::TimeUtilFactory().withSettledUtilParams(loose ? 15 : 6, 5, 150_ms));

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
		anglecontroller.setTarget(160);
		distancecontroller.setTarget(185);
	}
	int count = 0;
	do
	{
		count++;
		auto [_, top, x_mid, y_mid, _1] = Visions[Vision::FRONT]->get_mogo(Vision::ANY);
		auto distvel = distancecontroller.step(y_mid);
		auto yawvel = -anglecontroller.step(x_mid);
		//distvel = 0;

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

// void pathfinder()
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
// }

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
void drop_left_awp()
{
	//	drive.chassis->setMaxVelocity(200);
	//	const double v = 0.3;
	//	const int time = 250;
	//	drive.chassis->getModel()->driveVector(-v, 0);
	//	pros::delay(time);
	//	drive.chassis->getModel()->driveVector(v, 0);
	//	pros::delay(time);
	//	drive.chassis->getModel()->driveVector(0, 0);
	//	pros::delay(time);
	back_claw.piston.extend();
	pros::delay(400);
	back_claw.piston.retract();
};

void solo_awp_f()
{

	drive.chassis->setState(okapi::OdomState{x : 0_in, y : 0_in, theta : 0_deg});

	drop_left_awp();

	drive.chassis->setMaxVelocity(95);
	drive.chassis->turnAngle(90_deg, okapi::ChassisController::swing::left);
			printf("%s\n", drive.chassis->getState().str().c_str());
	drive.chassis->setMaxVelocity(100);
	drive.chassis->moveDistance(.5_tile);
			printf("%s\n", drive.chassis->getState().str().c_str());
	claw.ArmSetNum(2);

	drive.chassis->setMaxVelocity(135);
	auto x = -(4.5_tile - 4_in);
	auto y = 1.50_tile;
	drive.chassis->driveToPoint(okapi::Point{x : x, y : y});
			printf("%s\n", drive.chassis->getState().str().c_str());

	drive.chassis->turnToAngle(270_deg);
			printf("%s\n", drive.chassis->getState().str().c_str());
	return;

	drive.chassis->setMaxVelocity(80);
	// drive.chassis->turnToPoint(okapi::Point{x : x, y : y - 1_tile});

	drive.chassis->setMaxVelocity(120);
	drive.chassis->driveToPoint(okapi::Point{x : x, y : y - 1_tile}, false, 4_in);

	claw.Clasp();
	drive.chassis->setMaxVelocity(200);
	drive.chassis->moveDistance(.6_tile);
}

void right_awp()
{
}

void testing_routine()
{

	solo_awp_f();
	return;

	// profileController->moveTo({{1_tile, 0_tile + 18_in, 90_deg}, {1_tile, 1_tile + 18_in, 90_deg}, {3_tile, 3_tile, 45_deg}}, false, false);

	drive.chassis->setState(okapi::OdomState{x : 5_tile, y : 18_in, theta : 90_deg});
	drive.moveDistance(12_in);
	drive.chassis->setMaxVelocity(100);
	drive.chassis->driveToPoint(okapi::Point{x : 3_tile, y : 3_tile}, false, 4_in);

	front_line_up(2, 2, 2, false, Vision::YELLOW);
	drive.moveDistance(2_in);

	claw.Clasp();
	drive.chassis->driveToPoint(okapi::Point{x : 3_tile, y : 2_tile});
	claw.Leave();
}

void neumogo()
{
	claw.Leave();
	drive.chassis->moveDistance(1.5_tile);
	front_intake(6, 2, 2, false, Vision::YELLOW);
	drive.chassis->driveToPoint(okapi::Point{x : 1_tile, y : 1_tile}, true);
}

void auton_awp_left()
{
	drive.chassis->setState(okapi::OdomState{x : 12_in, y : 24_in - 5.5_in, theta : -90_deg});
	drive.chassis->setMaxVelocity(75);
	back_claw.ArmSetNum(2);
	back_claw.WaitUntilSettled();
	drive.chassis->moveDistance(6_in);
	pros::delay(200);
	back_claw.ArmSetNum(0);
	back_claw.WaitUntilSettled();

	drive.chassis->setMaxVelocity(90);
	drive.chassis->turnAngle(101_deg);
	drive.chassis->setMaxVelocity(100);

	// neumogo();
}

void auto_skills()
{
	claw.Leave();

	// front_intake(6, 2, 2, false, Vision::BLUE);

	drive.chassis->setMaxVelocity(70);
	drive.chassis->moveDistance(1_tile);
	pros::delay(100);

	drive.chassis->setMaxVelocity(120);

	claw.Leave();
	drive.chassis->moveDistance(3.5_tile);
	claw.Leave();
	pros::delay(300);
	drive.chassis->turnAngle(4_deg);
	drive.chassis->moveDistance(-5.5_tile);

	drive.chassis->turnAngle(-90_deg);
	drive.chassis->moveDistance(1_tile);
	drive.chassis->turnAngle(90_deg);

	front_intake(6, 2, 2, false, Vision::YELLOW);
	drive.chassis->moveDistance(2_tile);
	claw.Leave();
	pros::delay(300);
	drive.chassis->moveDistance(-5_tile);

	drive.chassis->turnAngle(-90_deg);
	drive.chassis->moveDistance(1.5_tile);
	drive.chassis->turnAngle(90_deg);

	front_intake(6, 2, 2, false, Vision::YELLOW);
	drive.chassis->moveDistance(1.5_tile);
	claw.Leave();
	pros::delay(300);
	drive.chassis->moveDistance(-5.5_tile);

	drive.chassis->turnAngle(-90_deg);
	drive.chassis->moveDistance(1.5_tile);
	drive.chassis->turnAngle(90_deg);

	front_intake(6, 2, 2, false, Vision::YELLOW);
	drive.chassis->moveDistance(2_tile);
	claw.Leave();
	pros::delay(300);
	drive.chassis->moveDistance(-5_tile);
	//	drive.chassis->driveToPoint(okapi::Point{x : 3_tile, y : 0.5_tile});
	//	front_intake(6, 2, 2, false, Vision::YELLOW);
	//	drive.chassis->driveToPoint(okapi::Point{x : 0_tile, y : 0_tile});
	//	claw.Leave();
}
void back_up()
{
	printf("RUNNING\n");
	// back_intake();
	// pathfinder();
	// front_intake();
	solo_awp_f();
	// neumogo();
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

void auton_yellow_mid() // TODO
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
