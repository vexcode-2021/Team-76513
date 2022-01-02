
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

		if (pros::competition::is_autonomous())
			continue;
		if (val & claw_task_up)
			claw.ArmUp();
		else if (val & claw_task_down)
			claw.ArmDown();

		if (val & claw_task_toggle)
		{
			claw.Toggle() ? claw.ArmSetRelative(-3) : claw.ArmSetRelative(3);
		}
	}
}

void opctrl_back_claw()
{
	while (true)
	{
		uint32_t val = pros::Task::notify_take(true, TIMEOUT_MAX);

		if (pros::competition::is_autonomous())
			continue;

		//if (val == 2)
		//{
		//	back_claw.ArmSetRelative(15);
		//}
		//else if (val == 1)
		//{
		//	back_claw.ArmSetRelative(-15);
		//} else {
		//	printf("AAA\n");
		//}

		if (val == 2)
			back_claw.ArmUp();
		else if (val == 1)
			back_claw.ArmDown();
		else
			printf("AAA back claw wrong number\n");
	}
}

pros::Task *drive_task = nullptr;
pros::Task *claw_task = nullptr;
pros::Task *back_claw_task = nullptr;

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
		printf("BACK_CLAW %f\n", back_claw.ArmGet());
		printf("FRONT:\n%s", Visions[Vision::FRONT]->status().c_str());
		//printf("BACK:\n%s", Visions[Vision::BACK]->status().c_str());
		printf("back ultrasonic: %f\n", ultrasonic.get());

		pros::delay(1000);
	}
}

void initialize()
{
	printf("init\n");
	pros::lcd::initialize();

	claw.init();
	back_claw.init();

	drive.init();
	drive_task = new pros::Task(opctrl_drivetrain);
	claw_task = new pros::Task(opctrl_claw);
	back_claw_task = new pros::Task(opctrl_back_claw);

	vision_init();

	printf("inited\n");
	pros::Task _ = pros::Task(print);
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

	drive.current_drive_mode = DRIVER_NONE;

	claw.ArmSetNum(0);
	pros::delay(100);
	claw.WaitUntilSettled();
	printf("arm SETTLED\n");

	if (SELECTED_AUTON_ROUTINE == solo_awp)
		solo_awp_f();
	else if (SELECTED_AUTON_ROUTINE == awp_left)
		auton_awp_left();
	else if (SELECTED_AUTON_ROUTINE == neumogo_front)
		neumogo();
	else if (SELECTED_AUTON_ROUTINE == skills)
		auto_skills();
	else
	{
	}

	drive.current_drive_mode = DRIVER_CONTROLLER;
}

void awp_t()

{
	testing_routine();
	pros::Task::current().suspend();
}
void fi_t()
{
	front_intake(7, 2.5, 50);
	pros::Task::current().suspend();
}
void fi2_t()
{
	//drive.chassis->setMaxVelocity(150);
	drive.chassis->turnAngle(360_deg);
	pros::delay(20);
	pros::Task::current().suspend();
}

void opcontrol()
{

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

		if constexpr (CONF_AUTON::autonButton)
		{
			static okapi::ControllerButton awp_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::auton_run);
			if (awp_button.changedToPressed())
				monitored_task(pros::Task(awp_t));
		}

		static okapi::ControllerButton fi_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::auto_front_intake);
		if (fi_button.changedToPressed())
			monitored_task(pros::Task(fi_t));

		static okapi::ControllerButton fi2_button = okapi::ControllerButton(ButtonMapping::claw_controller, okapi::ControllerDigital::right);
		if (fi2_button.changedToPressed())
			monitored_task(pros::Task(fi2_t));

		static okapi::ControllerButton back_claw_up_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::back_claw_up);
		static okapi::ControllerButton back_claw_down_button = okapi::ControllerButton(ButtonMapping::claw_controller, ButtonMapping::back_claw_down);

		static auto m = HARDWARE::BACK_CLAW_MOTOR;
		m.setEncoderUnits(okapi::AbstractMotor::encoderUnits::degrees);
		if (back_claw_up_button.changedToPressed())
		{
			back_claw_task->notify_ext(2, pros::E_NOTIFY_ACTION_OWRITE, NULL);
		}
		else if (back_claw_down_button.changedToPressed())
		{
			back_claw_task->notify_ext(1, pros::E_NOTIFY_ACTION_OWRITE, NULL);
		}

		pros::delay(ButtonMapping::delay.convert(1_ms));
	}
}
