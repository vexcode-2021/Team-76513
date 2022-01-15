#pragma once
#include "basics.hpp"
#include "vision.hpp"

void auton_skils()
{
    claw.Leave();
    drive.moveDistance(1_tile);

    claw.Clasp();

    drive.moveDistance(3_tile + 7_in);
    claw.Leave();

    drive.turnAngle(35_deg);
    back_claw.ArmSetNum(999);

    drive.moveDistance(-1_tile);
    front_line_up(1.8, 3, 3, false, Vision::YELLOW);

    drive.moveDistance(-1.7_tile);
    drive.turnAngle(75_deg);
    drive.moveDistance(3_tile);
}