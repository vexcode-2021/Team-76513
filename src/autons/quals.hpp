#pragma once

#include "basics.hpp"
#include "../config_consts.hpp"
#include "vision.hpp"

void fancy_left_awp()
{

    claw.ArmSet(20);
    claw.WaitUntilSettled();
    drive.moveDistance(6_in);
    claw.Clasp();
    pros::delay(700);
    claw.Leave();
};

void solo_awp_f()
{
    //											4.5 tiles will be tip + 22 for back claw centerish
    // drive.chassis->setState(okapi::OdomState{x : 4.5_tile + 22_in, y : 0.5_tile, theta : 0_deg});

    fancy_left_awp();

    drive.chassis->setSwing(okapi::ChassisController::swing::right);
    // drive.chassis->turnToPoint(okapi::Point{x : 0.5_tile, y : 1.5_tile});
    drive.turnAngle(90_deg);
    drive.chassis->setSwing(okapi::ChassisController::swing::none);
    drive.moveDistance(-13_in);
    drive.turnAngle(93_deg);
    drive.moveDistance(-3.5_tile - 7_in);
    back_piston_extend_retract_seq();
    back_claw.ArmSetNum(2);
    back_claw.WaitUntilSettled();
    drive.turnAngle(180_deg);

    // large num will clamp to max avialible

    // drive.chassis->setMaxVelocity(135);
    // drive.chassis->driveToPoint(okapi::Point{x : 0.5_tile, y : 1.5_tile}, true, 0_in);

    // back_piston_extend_retract_seq();

    // drive.chassis->turnToAngle(270_deg);
    // back_claw.ArmSetNum(1);
}
void grab_tallneu_base(okapi::QLength a, okapi::QLength b, okapi::QLength c)
{
    back_claw.ArmSetNum(1);

    drive.moveDistance(-a);

    back_claw.ArmSetNum(2);
    front_line_up(1.3, 3, 3, false, Vision::YELLOW);
    drive.chassis->setMaxVelocity(50);
    drive.chassis->moveDistance(-b);

    back_claw.ArmSetNum(1);

    // TODO vision

    pros::delay(100);

    drive.moveDistance(c);
    back_claw.ArmSetNum(2);
}

void drop_left_awp()
{
    back_piston_extend_retract_seq();
    back_piston_extend_retract_seq();

    drive.chassis->setSwing(okapi::ChassisController::swing::right);
    drive.chassis->turnAngle(-83_deg);
    drive.chassis->setSwing(okapi::ChassisController::swing::none);

    back_claw.ArmSetNum(2);

    drive.moveDistance(-1_tile);
    front_line_up(1.3, 3, 3, false, Vision::YELLOW);

    drive.chassis->setMaxVelocity(75);
    drive.chassis->moveDistance(-22_in);

    back_claw.ArmSetNum(1);

    // TODO vision

    pros::delay(100);

    drive.moveDistance(60_in);
    back_claw.ArmSetNum(2);
}
void grab_tallneu()
{
    grab_tallneu_base(55_in, 27_in, 65_in);
}
void right_awp()
{

drive.chassis->setMaxVelocity(50);
    drive.chassis->moveDistance(-20_in);

    back_piston_extend_retract_seq();

    drive.moveDistance(20_in);
    back_claw.ArmSetNum(999);

    back_claw.WaitUntilSettled();

    drive.moveDistance(-20_in);
    back_claw.ArmSet(BACK_CLAW_CONF::n - 35);

    // grab_tallneu();
}