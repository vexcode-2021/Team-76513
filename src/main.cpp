#include "main.h"
#include "subsystems/claw.hpp"
#include "subsystems/drivetrain.hpp"

Claw claw;
Drivetrain drive;
void claw_monitor()
{
	if (HARDWARE::LIMIT_SWITCHES.size() > 0)
		while (true)
		{
			claw.Protect();
			// 5 millis is a fraction of a degree of arm movement, it's fine to wait that long between checks
			pros::delay(5);
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
void competition_initialize() {}

void autonomous() {}

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

		pros::delay(ButtonMapping::delay.convert(1_ms));
	}
}