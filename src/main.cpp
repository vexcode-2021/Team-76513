#include "main.h"
#include "subsystems/claw.hpp"
#include "subsystems/drivetrain.hpp"

Claw claw;
Drivetrain drive;

okapi::ADIUltrasonic ultrasonic = okapi::ADIUltrasonic(HARDWARE::ULTRASONIC_FRONT_OUTPUT, HARDWARE::ULTRASONIC_FRONT_INPUT, std::make_unique<okapi::MedianFilter<5>>());

void claw_monitor()
{
	if (HARDWARE::LIMIT_SWITCHES.size() > 0)
		while (true)
		{
			claw.Protect();
			// 5 millis is a fraction of a degree of arm movement, it's fine to wait that long between checks
			// its not says broken limit switch arms
			pros::delay(1);
		}
}

enum AUTON_ROUTINES
{
	awp_right,
	awp_left,
	neumogo_front,
	neumogo_mid,
	auton_routine_none
};
static AUTON_ROUTINES SELECTED_AUTON_ROUTINE = awp_right;

void on_screen_button()
{
	switch (SELECTED_AUTON_ROUTINE)
	{
	case awp_right:
		SELECTED_AUTON_ROUTINE = awp_left;
		break;
	case awp_left:
		SELECTED_AUTON_ROUTINE = neumogo_front;
		break;
	case neumogo_front:
		SELECTED_AUTON_ROUTINE = neumogo_mid;
		break;
	case neumogo_mid:
		SELECTED_AUTON_ROUTINE = auton_routine_none;
		break;
	case auton_routine_none:
		SELECTED_AUTON_ROUTINE = awp_right;
		break;
	}
}

void opctrl_drivetrain()
{

	okapi::Controller c = okapi::Controller(ButtonMapping::drive_controller);
	while (true)
	{

		// if button is pressed
		if (pros::Task::notify_take(true, 0))
			drive.toggleMode();
		static int count = 0;
		count++;

		if (pros::competition::is_autonomous())
		{
			pros::delay(CONFIG_DRIVE::delay.convert(1_ms));
			continue;
		}
		drive.drive(c);

		pros::delay(CONFIG_DRIVE::delay.convert(1_ms));
	}
}

void claw_bail()
{
	claw.Motor2Hold(false);
	pros::delay(300);
	claw.Motor2Hold(true);
}

const uint32_t claw_task_up = 1 << 0;
const uint32_t claw_task_down = 1 << 1;
const uint32_t claw_task_toggle = 1 << 2;
void opctrl_claw()
{
	while (true)
	{
		uint32_t val = pros::Task::notify_take(true, TIMEOUT_MAX);
		//if (val != 0)
		//{
		//	printf("%x %x %x %x\n", val, claw_task_up, claw_task_down, claw_task_toggle);
		//	printf("%x\n", val & claw_task_up);
		//	printf("%x\n", val & claw_task_down);
		//	printf("%x\n", val & claw_task_toggle);
		//	//printf("%x\n", false * 0b010);
		//	//printf("%x\n", true * 0b010);
		//}

		if (pros::competition::is_autonomous())
			continue;
		if (!((val & claw_task_up) ^ (val & claw_task_down)))
			claw.ArmSoftStop();
		else if (val & claw_task_up)
			claw.ArmUp();
		else if (val & claw_task_down)
			claw.ArmDown();

		if (val & claw_task_toggle)
		{
			claw.Toggle() ? claw.ArmUp() : claw.ArmDown();
		}
	}
}

pros::Task *drive_task = nullptr;
pros::Task *claw_task = nullptr;
pros::Vision *visionsensor = nullptr;

static   pros::ADIAnalogIn sensor ('A');
		static   pros::ADIAnalogIn sensor2 ('B');



/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
	printf("init\n");
	pros::lcd::initialize();
	visionsensor = new pros::Vision(6);

	claw.init();
	auto t = pros::Task(claw_monitor);

	drive.init();
	drive_task = new pros::Task(opctrl_drivetrain);
	claw_task = new pros::Task(opctrl_claw);

	pros::vision_signature_s_t sig1 = pros::Vision::signature_from_utility(1, 45, 2667, 1356, -4103, -1023, -2562, 1.000, 0);
	pros::vision_signature_s_t sig2 = pros::Vision::signature_from_utility(2, -3229, -1431, -2330, 4031, 13055, 8542, 1.100, 0);
	pros::vision_signature_s_t sig3 = pros::Vision::signature_from_utility(3, 2239, 9421, 5830, -1119, 221, -448, 1.000, 0);
	visionsensor->set_signature(1, &sig1);
	visionsensor->set_signature(2, &sig2);
	visionsensor->set_signature(3, &sig3);

	sensor.calibrate();
	sensor2.calibrate();

	printf("inited\n");
}

void printAutonRoutines()
{
	auto r = SELECTED_AUTON_ROUTINE;
	bool ans =
		pros::lcd::print(1, "AWP Right          %s", r == awp_right ? "yes" : "no ");
	//printf("printing %d\n", ans);
	pros::lcd::print(2, "AWP Left           %s", r == awp_left ? "yes" : "no ");
	pros::lcd::print(3, "Front Neutral MOGO %s", r == neumogo_front ? "yes" : "no ");
	pros::lcd::print(4, "Mid Neutral MOGO %s", r == neumogo_mid ? "yes" : "no ");
	pros::lcd::print(5, "None               %s", r == auton_routine_none ? "yes" : "no ");
	pros::lcd::print(6, "make sure arm down btw", r == auton_routine_none ? "yes" : "no ");
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled()
{

	pros::lcd::register_btn0_cb(on_screen_button);
	
	while (true)
	{
		//pros::lcd::clear();
		printAutonRoutines();
		pros::delay(100);
	}
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize()
{
	printf("COMPINITSTART\n");
	pros::lcd::register_btn0_cb(on_screen_button);

	while (true)
	{
		//pros::lcd::clear();
		printAutonRoutines();
		pros::delay(100);
	}

	pros::lcd::clear();

	printf("COMPINIT\n");
}

void print()
{

	while (true)
	{
		//printf("c %f\n", ultrasonic.controllerGet());

		pros::delay(300);
	}
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

void autonomous()
{

	claw.Leave();
	pros::delay(100);
	claw.Motor2Hold(false);
	pros::Task us = pros::Task(print);

	claw.ArmSetNum(0);
	pros::delay(100);
	claw.WaitUntilSettled();
	printf("arm SETTLED\n");

	if (SELECTED_AUTON_ROUTINE == awp_right)
	{
		claw.ArmSetNum(1);
		claw.ArmSetNum(0);
		auton_awp();
	}
	else if (SELECTED_AUTON_ROUTINE == awp_left)
	{
		claw.ArmSetNum(1);
		claw.ArmSetNum(0);
		auton_awp2();
	}
	else if (SELECTED_AUTON_ROUTINE == neumogo_front)
		auton_yellow();

	else if (SELECTED_AUTON_ROUTINE == neumogo_mid)
		auton_yellow_mid();
	else
		;

	claw.Motor2Hold(true);
}

void opcontrol()
{
	claw.Motor2Hold(true);

	printf("opinited\n");
	while (true)
	{
		
		static int ct = 0;
		ct ++;
		if (!(ct % 50))
		printf("pot %d %d\n", sensor.get_value_calibrated(), sensor2.get_value_calibrated());

		static okapi::ControllerButton drive_mode_button = okapi::ControllerButton(ButtonMapping::drive_controller, ButtonMapping::drive_mode_switch);

		if (drive_mode_button.changedToPressed())
			drive_task->notify();

		static okapi::ControllerButton claw_hook_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::claw_hook_button);

		static okapi::ControllerButton claw_up_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::claw_arm_up);

		static okapi::ControllerButton claw_down_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::claw_arm_down);

		if (claw_hook_button.changed() || claw_up_button.changed() || claw_down_button.changed())

			claw_task->notify_ext(
				claw_hook_button.isPressed() * claw_task_toggle |
					claw_up_button.isPressed() * claw_task_up |
					claw_down_button.isPressed() * claw_task_down,
				pros::E_NOTIFY_ACTION_BITS, NULL);

		//static int count = 0;
		//	if (count % 40 == 0)
		//	{
		//		printf("%x\n", claw_hook_button.changedToPressed() * claw_task_toggle |
		//						 claw_up_button.changedToPressed() * claw_task_up |
		//						 claw_down_button.changedToPressed() * claw_task_down);
		//	}
		//count++;

		if (CONF_AUTON::autonButton)
		{
			static okapi::ControllerButton awp_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::auton_run);
			if (awp_button.changedToPressed())
			{
				drive_task->suspend();
				claw_task->suspend();
				competition_initialize();
				autonomous();
				drive_task->resume();
				claw_task->resume();
			}
		}

		//TODO add claw bail button
		static okapi::ControllerButton bail_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::claw_bail);
		if (bail_button.changedToPressed())
		{

			auto t = pros::Task(claw_bail);
		}

		pros::delay(ButtonMapping::delay.convert(1_ms));
	}
}
