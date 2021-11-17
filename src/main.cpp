
#include "main.h"
#include "shared.hpp"
#include "autonomous.hpp"

#include "auton_screen.hpp"

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
			claw.Toggle() ;
		}
	}
}

pros::Task *drive_task = nullptr;
pros::Task *claw_task = nullptr;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void print()
{

	while (true)
	{
		printf("pot %f %f\n", HARDWARE::POTL->get(), HARDWARE::POTR->get());

		pros::delay(500);
	}
}

void calibratearm()
{
	pros::delay(500);
	double lproportion = HARDWARE::POTL->get() / (HARDWARE::LMIN - HARDWARE::LMAX);
	double rproportion_shouldbe = lproportion * (HARDWARE::RMIN - HARDWARE::RMAX);
	double r_is = HARDWARE::POTR->controllerGet();

	HARDWARE::RMIN -= r_is - rproportion_shouldbe;
	HARDWARE::RMAX -= r_is - rproportion_shouldbe;

	printf("R-ARM adjust %f %f %f\n", r_is - rproportion_shouldbe, lproportion, r_is);
	printf("%f\n", HARDWARE::POTR->controllerGet());
}

void initialize()
{
	printf("init\n");
	pros::lcd::initialize();

	claw.init();

	drive.init();
	drive_task = new pros::Task(opctrl_drivetrain);
	claw_task = new pros::Task(opctrl_claw);

	vision_init();

	printf("inited\n");
	pros::Task _ = pros::Task(print);
	//pros::Task _1 = pros::Task(calibratearm);
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

void autonomous()
{

	claw.Leave();
	pros::delay(100);
	claw.Motor2Hold(false);

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

		static okapi::ControllerButton back_claw_up_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::back_claw_up);
		static okapi::ControllerButton back_claw_down_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::back_claw_down);

		static auto m = HARDWARE::BACK_CLAW_MOTOR;
		m.setEncoderUnits(okapi::AbstractMotor::encoderUnits::degrees);
		if (back_claw_up_button.changedToPressed())
		{
			m.moveRelative(30 / HARDWARE::BACK_CLAW_RATIO, 30);
		}
		else if (back_claw_down_button.changedToPressed())
		{
			m.moveRelative(-30 / HARDWARE::BACK_CLAW_RATIO, 30);
		}

		pros::delay(ButtonMapping::delay.convert(1_ms));
	}
}
