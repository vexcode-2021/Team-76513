#include "main.h"
#include "subsystems/claw.hpp"
#include "subsystems/drivetrain.hpp"

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button()
{
	static bool pressed = false;
	pressed = !pressed;
	if (pressed)
	{
		pros::lcd::set_text(2, "I was pressed!");
	}
	else
	{
		pros::lcd::clear_line(2);
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
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
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

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

void opctrl_drivetrain()
{
	Drivetrain drive = Drivetrain();
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

const uint32_t claw_task_up = 1 << 20;
const uint32_t claw_task_down = 1 << 21;
const uint32_t claw_task_toggle = 1 << 22;
void opctrl_claw()
{

	Claw claw = Claw();
	while (true)
	{
		uint32_t val = pros::Task::notify_take(true, TIMEOUT_MAX);

		if (!((val && claw_task_up) ^ (val && claw_task_down)))
			claw.ArmMove(0);
		else if (val && claw_task_up)
			claw.ArmMove(CLAW_CONF::velocity_voltage);
		else if (val && claw_task_down)
			claw.ArmMove(-CLAW_CONF::velocity_voltage);

		if (val && claw_task_toggle)
			claw.Toggle();
	}
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol()
{
	pros::Task drive_task = pros::Task(opctrl_drivetrain);
	pros::Task claw_task = pros::Task(opctrl_claw);

	while (true)
	{

		static okapi::ControllerButton drive_mode_button = okapi::ControllerButton(ButtonMapping::drive_controller, ButtonMapping::drive_mode_switch);

		if (drive_mode_button.changedToPressed())
			drive_task.notify();

		static okapi::ControllerButton claw_hook_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::claw_hook_button);

		static okapi::ControllerButton claw_up_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::claw_arm_up);

		static okapi::ControllerButton claw_down_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::claw_arm_down);

		claw_task.notify_ext(
			claw_hook_button.changedToPressed() * claw_task_toggle |
				claw_up_button.changedToPressed() * claw_task_up |
				claw_down_button.changedToPressed() * claw_task_down,
			pros::E_NOTIFY_ACTION_BITS, NULL);

		pros::delay(ButtonMapping::delay.convert(1_ms));
	}
}