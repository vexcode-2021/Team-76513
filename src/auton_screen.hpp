#pragma once
#include "autonomous.hpp"

enum AUTON_ROUTINES
{
	awp_right,
	awp_left,
	neumogo_from_right,
	skills,
	auton_routine_none
};
static AUTON_ROUTINES SELECTED_AUTON_ROUTINE = skills;

void main_auton()
{
	claw.ArmSetNum(0);
	claw.Leave();
	pros::delay(100);
	claw.WaitUntilSettled();
	printf("arm SETTLED\n");

	if (SELECTED_AUTON_ROUTINE == awp_right)
		right_awp();
	else if (SELECTED_AUTON_ROUTINE == awp_left)
		drop_left_awp();
	else if (SELECTED_AUTON_ROUTINE == neumogo_from_right)
		grab_tallneu();
	else if (SELECTED_AUTON_ROUTINE == skills)
		auton_skils();
	else
	{
	}
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
	pros::lcd::print(3, "Neutral Tall MOGO  %s", r == neumogo_from_right ? "yes" : "no ");
	pros::lcd::print(4, "Skills             %s", r == skills ? "yes" : "no ");
	pros::lcd::print(5, "None               %s", r == auton_routine_none ? "yes" : "no ");
}
