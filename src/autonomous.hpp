void vision_init()
{

	visionsensor = new pros::Vision(6);

	pros::vision_signature_s_t sig1 = pros::Vision::signature_from_utility(1, 45, 2667, 1356, -4103, -1023, -2562, 1.000, 0);
	pros::vision_signature_s_t sig2 = pros::Vision::signature_from_utility(2, -3229, -1431, -2330, 4031, 13055, 8542, 1.100, 0);
	pros::vision_signature_s_t sig3 = pros::Vision::signature_from_utility(3, 2239, 9421, 5830, -1119, 221, -448, 1.000, 0);
	visionsensor->set_signature(1, &sig1);
	visionsensor->set_signature(2, &sig2);
	visionsensor->set_signature(3, &sig3);
}

double vision(int sig)
{

	auto alliancemogo = visionsensor->create_color_code(2, 3);
	auto f = okapi::MedianFilter<5>();
	for (int i = 0; i < 5; i++)
	{
		int16_t n;
		if (sig == 1)
			n = visionsensor->get_by_sig(0, sig).x_middle_coord;
		else
		{
			auto a1 = visionsensor->get_by_code(0, 3).x_middle_coord;
			auto a2 = visionsensor->get_by_sig(0, 2).x_middle_coord;
			if (abs(a1 - 158) < abs(a2 - 158))
			{
				n = a1;
			}
			else
				n = a2;
		}
		printf("vision %d\n", n);
		f.filter(n);
	}
	printf("vision final %f\n", f.getOutput());
	return f.getOutput() - VISION_FOV_WIDTH / 2.0;
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