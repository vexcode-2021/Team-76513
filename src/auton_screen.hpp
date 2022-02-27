#pragma once
#include "autonomous.hpp"
#include "111auton_routine.hpp"

void main_auton()
{
	claw.ArmSetNum(0);
	claw.Leave();
	claw.WaitUntilSettled();

	ez::as::auton_selector.call_selected_auton();
	return;
	Visions[Vision::FRONT]->sensor->set_exposure(71); // TODO
	printf("arm SETTLED\n");

	if (SELECTED_AUTON_ROUTINE == awp_right)
		right_awp();
	else if (SELECTED_AUTON_ROUTINE == awp_left)
		drop_left_awp();
	else if (SELECTED_AUTON_ROUTINE == neumogo_from_right)
		grab_neuu(true);
	else if (SELECTED_AUTON_ROUTINE == neumogo_left)
		grab_neuu(false);
	else if (SELECTED_AUTON_ROUTINE == skills)
		auton_skils();
	else
	{
	}
}
void addAutons()
{
	ez::as::auton_selector.add_autons({
		Auton("SKills\n\nDesc.", auton_skils),
		Auton("RIGHT AWP\n\nDesc.", right_awp),
		Auton("LEFT AWP Turn\n\naaaaaaaaaaaaaaa", drop_left_awp),
		Auton("3LEFT AWP Turn\n\naaaaaaaaaaaaaaa", drop_left_awp),
		Auton("4LEFT AWP Turn\n\naaaaaaaaaaaaaaa", drop_left_awp),
		Auton("5RIGHT AWP\n\nDesc.", right_awp),
		Auton("6RIGHT AWP\n\nDesc.", right_awp),
		Auton("7RIGHT AWP\n\nDesc.", right_awp),
	});
}
void on_screen_button()
{
	switch (SELECTED_AUTON_ROUTINE)
	{
	case awp_right:
		SELECTED_AUTON_ROUTINE = awp_left;
		break;
	case awp_left:
		SELECTED_AUTON_ROUTINE = neumogo_from_right;
		break;
	case neumogo_from_right:
		SELECTED_AUTON_ROUTINE = neumogo_left;
		break;
	case neumogo_left:
		SELECTED_AUTON_ROUTINE = skills;
		break;
	case skills:
		SELECTED_AUTON_ROUTINE = auton_routine_none;
		break;
	case auton_routine_none:
		SELECTED_AUTON_ROUTINE = awp_right;
		break;
	}
}

void printAutonRoutines()
{
	auto r = SELECTED_AUTON_ROUTINE;

	pros::lcd::print(1, "AWP Right          %s", r == awp_right ? "yes" : "no ");
	pros::lcd::print(2, "AWP Left           %s", r == awp_left ? "yes" : "no ");
	pros::lcd::print(3, "Neu Tall rig MOGO  %s", r == neumogo_from_right ? "yes" : "no ");
	pros::lcd::print(4, "Neu Tall lef MOGO  %s", r == neumogo_left ? "yes" : "no ");
	pros::lcd::print(5, "Skills             %s", r == skills ? "yes" : "no ");
	pros::lcd::print(6, "None               %s", r == auton_routine_none ? "yes" : "no ");
}
