#pragma once

enum AUTON_ROUTINES
{
	solo_awp,
	awp_left,
	neumogo_front,
	skills,
	auton_routine_none
};
static AUTON_ROUTINES SELECTED_AUTON_ROUTINE = skills;

void on_screen_button()
{
	switch (SELECTED_AUTON_ROUTINE)
	{
	case solo_awp:
		SELECTED_AUTON_ROUTINE = awp_left;
		break;
	case awp_left:
		SELECTED_AUTON_ROUTINE = neumogo_front;
		break;
	case neumogo_front:
		SELECTED_AUTON_ROUTINE = skills;
		break;
	case skills:
		SELECTED_AUTON_ROUTINE = auton_routine_none;
		break;
	case auton_routine_none:
		SELECTED_AUTON_ROUTINE = solo_awp;
		break;
	}
}

void printAutonRoutines()
{
	auto r = SELECTED_AUTON_ROUTINE;

	pros::lcd::print(1, "Solo AWP          %s", r == solo_awp ? "yes" : "no ");
	pros::lcd::print(2, "AWP Left           %s", r == awp_left ? "yes" : "no ");
	pros::lcd::print(3, "Front Neutral MOGO %s", r == neumogo_front ? "yes" : "no ");
	pros::lcd::print(4, "Skills %s", r == skills ? "yes" : "no ");
	pros::lcd::print(5, "None               %s", r == auton_routine_none ? "yes" : "no ");
	pros::lcd::print(6, "make sure arm down btw", r == auton_routine_none ? "yes" : "no ");
}
