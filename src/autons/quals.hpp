#pragma once

#include "basics.hpp"
#include "../config_consts.hpp"
#include "vision.hpp"
#include "skills.hpp"

void fancy_left_awp()
{

    claw.ArmSet(20);
    pros::delay(50);
    claw.WaitUntilSettled();
    drive.moveDistance(6_in);
    claw.Clasp();
    pros::delay(700);
    claw.Leave();
};

void solo_awp_f()

{
    //											4.5 tiles will be tip + 22 for back claw centerish
    drive.chassis->setState(okapi::OdomState{x : 4.5_tile + 22_in, y : 0.5_tile, theta : 0_deg});

    fancy_left_awp();

    drive.chassis->setSwing(okapi::ChassisController::swing::right);
    // drive.chassis->turnToPoint(okapi::Point{x : 0.5_tile, y : 1.5_tile});
    drive.turnAngle(90_deg);
    drive.chassis->setSwing(okapi::ChassisController::swing::none);
    drive.chassis->setMaxVelocity(50);
    drive.moveDistance(-14_in);
    drive.turnAngle(90_deg);
    drive.chassis->setMaxVelocity(150);
    drive.chassis->moveDistance(-3.5_tile);

    back_piston_extend_retract_seq();

    drive.moveDistance(20_in);

    back_claw.ArmSetNum(2);
    back_claw.WaitUntilSettled();
    drive.moveDistance(-20_in);
    drive.turnAngle(180_deg);

    // large num will clamp to max avialible

    // drive.chassis->setMaxVelocity(135);
    // drive.chassis->driveToPoint(okapi::Point{x : 0.5_tile, y : 1.5_tile}, true, 0_in);

    // back_piston_extend_retract_seq();

    // drive.chassis->turnToAngle(270_deg);
    // back_claw.ArmSetNum(1);
}

void drop_left_awp()
{
    myIMU->setOffset(0);
    claw.ArmSetNum(1);
    claw.Leave();
    back_piston_extend_retract_seq();

    using namespace skillsn;
    // if it starts getting stuck //moveDistance(5_in);
    currently_carrying = NO_GOAL;
    turnToAngle(100_deg, okapi::ChassisController::swing::left);
    front_intake(4_s, Vision::YELLOW);
    currently_carrying = ONE_GOAL;
    drive.moveDistance(-2_tile);
}
void grab_tallneu()
{
    // grab_tallneu_base(55_in, 27_in, 65_in);
    // drive.moveDistance(1.5_tile);

    // drive.chassis->setMaxVelocity(75);
    // drive.chassis->moveDistance(.5_tile);
    // claw.Clasp();

    // back_claw.ArmSetNum(999);
    // back_claw.WaitUntilSettled();

    // drive.turnAngle(90_deg);
    // front_line_up(1.3, 3, 3, false, Vision::YELLOW);

    // back_claw.ArmSetNum(1);
    // drive.turnAngle(30_deg);
    // drive.moveDistance(2_tile);
}
void right_awp()
{

    using namespace skillsn;

    myIMU->setOffset(-45);

    back_claw.ArmSetNum(2);
    pros::delay(500);
    currently_carrying = SLOW_BC;
    moveDistance(-22_in);

    back_piston_extend_retract_seq();
    currently_carrying = NO_GOAL;

    moveDistance(10_in);
    back_claw.ArmSetNum(0);

    turnToAngle(45_deg);
    moveDistance(1_tile);

    front_intake(2_s, Vision::YELLOW);
    turnToAngle(45_deg);
    moveDistance(-3_tile);
}

void grab_neuu(bool right = true)
{
    if (true)
    {
        myIMU->setOffset(-90);
        back_claw.ArmSetNum(2);
        pros::delay(300);

        drive.chassis->setMaxVelocity(200);
        drive.chassis->moveDistance(-1.8_tile);

        drive.chassis->setMaxVelocity(90);
        drive.chassis->moveDistance(-7_in);

        back_claw.ArmSetNum(0);
        using namespace skillsn;
        currently_carrying = ONE_GOAL;

        turnToAngle(30_deg, okapi::ChassisController::swing::left);
        turnToAngle(45_deg);
        front_intake(3_s, Vision::YELLOW);
        currently_carrying = TWO_GOAL;

        turnToAngle(45_deg);
        moveDistance(-3_tile);
        return;
    }

    myIMU->setOffset(45);

    drive.chassis->moveDistance(2.4_tile);
    front_intake(3_s, Vision::YELLOW);

    drive.chassis->setMaxVelocity(90);

    drive.chassis->turnToAngle(90_deg);
    drive.chassis->moveDistance(-2_tile);
//
//
//   myIMU->setOffset(90);
//
//    front_intake(4_s, Vision::YELLOW);
//
//
//    drive.chassis->setMaxVelocity(90);
//
//    drive.chassis->turnToAngle(90_deg);
//    drive.chassis->moveDistance(-2_tile);




    //    claw.ArmSetNum(1);
    //    turnToAngle(150_deg - 2 * 360_deg, okapi::ChassisController::swing::right);
    //    front_intake(3_s, Vision::YELLOW);
    //    currently_carrying = TWO_GOAL;
    //
    //    moveDistance(-3_tile);
}