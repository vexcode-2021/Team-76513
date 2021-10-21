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
	pros::lcd::set_text(1, "Hello PROS User!");

	claw.init();
	auto t = pros::Task(claw_monitor);

	drive.init();

	printf("inited\n");
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

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
	//claw.ArmSetNum(0);
	//pros::delay(90);
	//claw.WaitUntilSettled();
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
void auton_awp()
{
	drive.chassis->setMaxVelocity(20);
	claw.ArmSet(40);
	claw.WaitUntilSettled();
	drive.chassis->moveDistance(6_in);

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
	//drive.chassis->moveDistance(1_in);
	printf("REACHED\n");

	claw.Clasp();

	pros::delay(300);
	drive.chassis->moveDistance(1_in);
	printf("REACHED\n");
	drive.chassis->moveDistanceAsync(-8_in);
	claw.Leave();
	claw.ArmSetNum(1);
	claw.ArmSet(2);
	drive.chassis->waitUntilSettled();
	claw.WaitUntilSettled();
	printf("MOVED7andCLAW_DOWN\n");

	drive.chassis->moveDistance(9_in);
	printf("MOVEDforw7\n");
	claw.Clasp();
	printf("GRABBED\n");
	drive.chassis->moveDistance(-1.3_ft);
	claw.Leave();
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
	//drive.chassis->moveDistance(1_in);
	printf("REACHED\n");

	claw.Clasp();
	pros::delay(600);
	claw.Leave();
}

void auton_yellow()
{
	okapi::QLength d = 47_in;
	drive.chassis->setMaxVelocity(120);
	claw.ArmSet(2);
	printf("1\n");
	drive.chassis->moveDistanceAsync(d - 5_in);
	drive.chassis->turnAngleAsync(-5_deg);
	printf("2\n");
	//claw.WaitUntilSettled();
	//printf("3\n");
	drive.chassis->waitUntilSettled();
	drive.chassis->setMaxVelocity(60);
	printf("4\n");
	drive.chassis->moveDistance(6_in);

	//auto exampleController = okapi::IterativeControllerFactory::posPID(0.005, 0.001, 0);
	//exampleController.setTarget(75);
	//while (!exampleController.isSettled())
	//{
	//	double in = ultrasonic.controllerGet();
	//	double newOutput = exampleController.step(in);

	//	static int count = 0;
	//	count++;
	//	if (!(count % 30))
	//		printf("%f:%f\n", in, newOutput);
	//	drive.chassis->getModel()->forward(-newOutput);
	//	pros::delay(10);
	//}
	//drive.chassis->moveDistance(1_in);
	printf("REACHED\n");

	claw.Clasp();
	pros::delay(600);
	drive.chassis->moveDistance(-(d - 5_in) / 2);
	drive.chassis->turnAngle(-9_deg);
	drive.chassis->moveDistance(-(d - 5_in) / 2);

	claw.Leave();
}

void autonomous()
{

	claw.Motor2Hold(false);
	pros::Task us = pros::Task(print);
	claw.ArmSet(0);
	//auton_awp();
	//auton_awp2();
	auton_yellow();
	claw.Motor2Hold(true);
}

void opctrl_drivetrain()
{
	okapi::Controller c = okapi::Controller(ButtonMapping::drive_controller);
	while (true)
	{

		// if button is pressed
		if (pros::Task::notify_take(true, 0))
			drive.toggleMode();

		drive.drive(c);
		pros::delay(CONFIG_DRIVE::delay.convert(1_ms));
	}
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

		if (!((val & claw_task_up) ^ (val & claw_task_down)))
			claw.ArmSoftStop();
		else if (val & claw_task_up)
			claw.ArmUp();
		else if (val & claw_task_down)
			claw.ArmDown();

		if (val & claw_task_toggle)
			claw.Toggle();
	}
}

void opcontrol()
{
	claw.Motor2Hold(true);
	pros::Task drive_task = pros::Task(opctrl_drivetrain);
	pros::Task claw_task = pros::Task(opctrl_claw);

	printf("opinited\n");
	while (true)
	{

		static okapi::ControllerButton drive_mode_button = okapi::ControllerButton(ButtonMapping::drive_controller, ButtonMapping::drive_mode_switch);

		if (drive_mode_button.changedToPressed())
			drive_task.notify();

		static okapi::ControllerButton claw_hook_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::claw_hook_button);

		static okapi::ControllerButton claw_up_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::claw_arm_up);

		static okapi::ControllerButton claw_down_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::claw_arm_down);

		if (claw_hook_button.changed() || claw_up_button.changed() || claw_down_button.changed())

			claw_task.notify_ext(
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
				drive_task.suspend();
				claw_task.suspend();
				competition_initialize();
				autonomous();
				drive_task.resume();
				claw_task.resume();
			}
		}

		//TODO add claw bail button

		pros::delay(ButtonMapping::delay.convert(1_ms));
	}
}
