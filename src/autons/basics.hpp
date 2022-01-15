
#pragma once

void back_piston_extend_retract_seq()
{
	back_claw.piston.extend();
	pros::delay(250);
	back_claw.piston.retract();
}