
#pragma once

void back_piston_extend_retract_seq()
{
	awp_piston->extend();
	pros::delay(250);
	awp_piston->retract();
}
