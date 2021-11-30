void vision_init()
{
	Vision.init();
}

double vision(int sig)
{
	auto c = sig == 1 ? Vision.YELLOW : Vision.ALLIANCE;

	return std::get<0>(Vision.get_mogo_target_diff(c, Vision.FRONT, VISION_FOV_WIDTH / 2, 0));
}
void auton_awp()
{
	drive.chassis->setMaxVelocity(40);
	claw.ArmSet(40);
	claw.WaitUntilSettled();
	drive.chassis->moveDistance(9_in);
	drive.chassis->setMaxVelocity(20);

	auto exampleController = okapi::IterativeControllerFactory::posPID(0.005, 0.001, 0);
	exampleController.setTarget(130);
	while (!exampleController.isSettled())
	{
		double in = ultrasonic.controllerGet();
		double newOutput = exampleController.step(in);

		static int count = 0;
		count++;
		if (!(count % 30))
			printf("%f:%f\n", in, newOutput);
		drive.chassis->getModel()->forward(-newOutput);
		pros::delay(10);
	}
	//drive.chassis->moveDistance(1_in);
	printf("REACHED\n");

	claw.Clasp();

	pros::delay(300);
	//drive.chassis->moveDistance(-8_in);

	claw.Leave();
	//claw.ArmSet(2);
	//pros::delay(300);
	//claw.WaitUntilSettled();

	//drive.chassis->turnAngle((vision(2) / 6) * 1.0_deg);
	drive.chassis->turnAngle(15_deg);
	drive.chassis->moveDistance(16_in);
	printf("MOVEDforw7\n");
	//claw.Clasp();
	//printf("GRABBED\n");

	//drive.chassis->setMaxVelocity(50);
	//drive.chassis->moveDistance(-1.3_ft);
	//claw.Leave();
}

void auton_awp2()
{
	drive.chassis->setMaxVelocity(20);
	claw.ArmSet(40);
	claw.WaitUntilSettled();
	drive.chassis->moveDistance(10_in);

	auto exampleController = okapi::IterativeControllerFactory::posPID(0.005, 0.001, 0);
	exampleController.setTarget(145);
	while (!exampleController.isSettled())
	{
		double in = ultrasonic.controllerGet();
		double newOutput = exampleController.step(in);

		static int count = 0;
		count++;
		if (!(count % 30))
			printf("%f:%f\n", in, newOutput);
		drive.chassis->getModel()->forward(-newOutput);
		pros::delay(10);
	}
	printf("REACHED\n");

	claw.Clasp();
	pros::delay(600);
	claw.Leave();
}

void auton_yellow()
{
	printf("AUTON_YELLOW\n");
	drive.chassis->setMaxVelocity(120);
	claw.ArmSet(2);
	printf("1\n");
	drive.chassis->moveDistance(36_in);
	pros::delay(100);
	drive.chassis->setMaxVelocity(30);
	drive.chassis->turnAngle((vision(1) / 3.8) * 1.0_deg);
	printf("4\n");
	drive.chassis->moveDistance(14_in);

	printf("REACHED\n");

	claw.Clasp();
	drive.chassis->setMaxVelocity(120);
	pros::delay(300);
	drive.chassis->moveDistance(-45_in);

	claw.Leave();
}

void auton_yellow_mid() //TODO
{
	printf("AUTON_YELLOW_MID\n");
	drive.chassis->setMaxVelocity(120);
	claw.ArmSet(2);
	printf("1\n");
	drive.chassis->moveDistance(21_in);
	drive.chassis->setMaxVelocity(30);
	drive.chassis->turnAngle(-47.5_deg);
	drive.chassis->setMaxVelocity(120);
	drive.chassis->moveDistance(32.49_in);
	pros::delay(200);
	printf("2\n");
	drive.chassis->waitUntilSettled();
	drive.chassis->setMaxVelocity(30);
	printf("4\n");

	drive.chassis->turnAngle((vision(1) / 3.8) * 1.0_deg);

	pros::delay(300);
	drive.chassis->moveDistance(13_in);

	printf("REACHED\n");

	pros::delay(200);
	claw.Clasp();
	pros::delay(200);
	drive.chassis->setMaxVelocity(90);
	drive.chassis->moveDistance(-53_in);

	claw.Leave();
}
