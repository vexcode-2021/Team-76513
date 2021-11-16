#pragma once

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