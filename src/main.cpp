
#include "main.h"
#include "shared.hpp"
#include "autonomous.hpp"

#include "auton_screen.hpp"
#include "display.hpp"

void opctrl_drivetrain()
{

	static okapi::Controller c = okapi::Controller(ButtonMapping::drive_controller);
	static okapi::Controller c2 = okapi::Controller(ButtonMapping::backup_controller);
	while (true)
	{

		// if button is pressed
		if (pros::Task::notify_take(true, 0))
			drive.toggleMode();

		drive.drive(c, c2);
		pros::delay(CONFIG_DRIVE::delay.convert(10_ms));
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
		// if (val != 0)
		//{
		//	printf("%x %x %x %x\n", val, claw_task_up, claw_task_down, claw_task_toggle);
		//	printf("%x\n", val & claw_task_up);
		//	printf("%x\n", val & claw_task_down);
		//	printf("%x\n", val & claw_task_toggle);
		//	//printf("%x\n", false * 0b010);
		//	//printf("%x\n", true * 0b010);
		// }

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

		// if (val == 2)
		//{
		//	back_claw.ArmSetRelative(15);
		// }
		// else if (val == 1)
		//{
		//	back_claw.ArmSetRelative(-15);
		// } else {
		//	printf("AAA\n");
		// }

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
	pros::Imu imu_sensor(HARDWARE::IMUPORT);
	while (true)
	{
		printf("pot %f %f\n", HARDWARE::POTL->get(), HARDWARE::POTR->get());
		//  printf("BACK_CLAW %f\n", back_claw.ArmGet());
		// printf("FRONT:\n%s", Visions[Vision::FRONT]->status().c_str());
		// printf("BACK:\n%s", Visions[Vision::BACK]->status().c_str());
		// printf("ultrasonic: %f\n", ultrasonic.get());
		printf("MYIMU %f %d\n", myIMU->get(), static_cast<std::int32_t>(myIMU->get() * 100.0));
		printf("MYIMUy %f %d\n", myIMUy->get(), static_cast<std::int32_t>(myIMU->get() * 100.0));

		printf("temp, torq, power arm: %f %f %f\n", pros::c::motor_get_temperature(HARDWARE::CLAW_ARM_MOTOR1.getPort()), pros::c::motor_get_torque(HARDWARE::CLAW_ARM_MOTOR1.getPort()), pros::c::motor_get_power(HARDWARE::CLAW_ARM_MOTOR1.getPort()));
		printf("drive temperature %f %f %f %f %f %f tq:%f N\n", pros::c::motor_get_temperature(12), pros::c::motor_get_temperature(13), pros::c::motor_get_temperature(15), pros::c::motor_get_temperature(17), pros::c::motor_get_temperature(18), pros::c::motor_get_temperature(19), drive.getForce().convert(1_n));

		printf("ODOMSTATE: %s\n", drive.chassis->getState().str().c_str());

		pros::delay(500);
	}
}
void screen_stuff()
{
	auto rate = okapi::Rate();
	while (true)
	{
		ControllerScreen.oneLoop();
		rate.delayUntil(100_ms);
	}
}

void sg3_warn()
{
	// competition_get_status returns ENABLED,AUTONOMOUS,CONNECTED bits
	// no its CONNECTED, AUTO, DISABLE so 0b100
	// the xor (^) function negates bits, i.e. 1 ^ 1 = 0
	//  Here, 0b101, ENABLED,NO_AUTON,CONNECTED is my wanted state
	//  when it reaches the wanted state (i.e.) status returned is 0b101, the answer is 0
	// until the answer is 0 (i.e. while ans != 0), it loops and waits
	while ((pros::c::competition_get_status() ^ (0b100)) != 0)
	{
		pros::delay(1000);
	}
	// printf("SG# FINISH DELAYING\n");

	// after it finishes looping, i.e. when driver control starts
	// we need to wait for 1:45 - 0:40 = 1:05 (65s) and then buzz at 40s
	pros::delay((1_min + 5_s).convert(1_ms));

	// first rumble partner to pre-warn, then rumble main controller, then re-rumble partner to remind
	int retries = 0;
	do
	{
		errno = 0;
		pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, ". -- .");
		// printf("SG# err %d\n", errno);
		pros::delay(100);
		retries++;
	} while (errno != 0 && retries < 9);
	do
	{
		errno = 0;
		pros::c::controller_rumble(pros::E_CONTROLLER_PARTNER, "-     --");
		pros::delay(100);
		retries++;
	} while (errno != 0 && retries < 9);
}

void initialize()
{
	printf("init\n");
	// pros::lcd::initialize();
	addAutons();
	ez::as::initialize();

	claw.init();
	back_claw.init();

	myIMU->calibrate();
	drive.myIMU = myIMU;
	drive.init();
	drive_task = new pros::Task(opctrl_drivetrain);
	claw_task = new pros::Task(opctrl_claw);
	back_claw_task = new pros::Task(opctrl_back_claw);

	vision_init();
	auton_init();

	printf("inited\n");
	pros::Task _ = pros::Task(print);
	pros::Task _1 = pros::Task(screen_stuff);
	pros::Task _2 = pros::Task(sg3_warn);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled()
{

	// pros::lcd::register_btn0_cb(on_screen_button);

	while (true)
	{
		// pros::lcd::clear();
		// printAutonRoutines();
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
	// pros::lcd::register_btn0_cb(on_screen_button);

	while (true)
	{
		// pros::lcd::clear();
		// printAutonRoutines();
		pros::delay(100);
	}

	// pros::lcd::clear();

	printf("COMPINIT\n");
}

void autonomous()
{

	pre_auton();

	main_auton();

	post_auton();
}

void down_butt()
{
	pre_auton();
	back_line_up();
	post_auton();
	pros::Task::current().suspend();
}

void awp_t()

{

	pre_auton();
	testing_routine();
	post_auton();
	pros::Task::current().suspend();
}
void fi_t()
{
	pre_auton();

	front_intake(50_s, Vision::ANY);
	post_auton();
	pros::Task::current().suspend();
}
void fi2_t()
{
	pre_auton();

	skillsn::currently_carrying = skillsn::NO_GOAL;
	while (true)
	{
		drive.chassis->turnAngle(90_deg);
		drive.chassis->moveDistance(10_in);
	//	skillsn::turnAngle(90_deg);
	//	skillsn::moveDistance(10_in);
	}
	pros::delay(20);
	post_auton();
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

		static okapi::ControllerButton claw_up_button_pt = okapi::ControllerButton(okapi::ControllerId::partner, ButtonMapping::claw_arm_up);

		static okapi::ControllerButton claw_down_button_pt = okapi::ControllerButton(okapi::ControllerId::partner, ButtonMapping::claw_arm_down);

		if (claw_hook_button.changed() || claw_up_button.changed() || claw_down_button.changed() || claw_up_button_pt.changed() || claw_down_button_pt.changed())

			claw_task->notify_ext(
				claw_hook_button.isPressed() * claw_task_toggle |
					(claw_up_button.isPressed() || claw_up_button_pt.isPressed()) * claw_task_up |
					(claw_down_button.isPressed() || claw_down_button_pt.isPressed()) * claw_task_down,
				pros::E_NOTIFY_ACTION_BITS, NULL);

		// static int count = 0;
		//	if (count % 40 == 0)
		//	{
		//		printf("%x\n", claw_hook_button.changedToPressed() * claw_task_toggle |
		//						 claw_up_button.changedToPressed() * claw_task_up |
		//						 claw_down_button.changedToPressed() * claw_task_down);
		//	}
		// count++;

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

		static okapi::ControllerButton back_claw_up_button_pt = okapi::ControllerButton(okapi::ControllerId::partner, ButtonMapping::back_claw_up);
		static okapi::ControllerButton back_claw_down_button_pt = okapi::ControllerButton(okapi::ControllerId::partner, ButtonMapping::back_claw_down);

		static auto m = HARDWARE::BACK_CLAW_MOTOR;
		m.setEncoderUnits(okapi::AbstractMotor::encoderUnits::degrees);
		if (back_claw_up_button.changedToPressed() || back_claw_up_button_pt.changedToPressed())
		{
			back_claw_task->notify_ext(2, pros::E_NOTIFY_ACTION_OWRITE, NULL);
		}
		else if (back_claw_down_button.changedToPressed() || back_claw_down_button_pt.changedToPressed())
		{
			back_claw_task->notify_ext(1, pros::E_NOTIFY_ACTION_OWRITE, NULL);
		}
		static auto printf_state_button = okapi::ControllerButton(ButtonMapping::claw_controller, okapi::ControllerDigital::X);
		if (printf_state_button.changedToPressed())
		{
			back_claw.Toggle();
		};

		static auto back_button = okapi::ControllerButton(ButtonMapping::claw_controller, okapi::ControllerDigital::down);
		if (back_button.changedToPressed())
			monitored_task(pros::Task(down_butt));
		;

		pros::delay(ButtonMapping::delay.convert(1_ms));
	}
}
