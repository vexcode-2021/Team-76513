#pragma once
#include "basics.hpp"
#include "vision.hpp"

namespace skillsn
{
    enum CARRYING
    {
        NO_GOAL,
        ONE_GOAL,
        TWO_GOAL

    };

    void retract_all()
    {
        back_claw.ArmSetNum(0);
        claw.ArmSetNum(0);

        back_claw.WaitUntilSettled();
        claw.WaitUntilSettled();
    }

    void back_claw_set(int n)
    {
        back_claw.ArmSetNum(n);
    }

    void back_claw_set_wait(int n)
    {
        back_claw_set(n);
        back_claw.WaitUntilSettled();
    }

    void turnToAngle(CARRYING a, okapi::QAngle angle)
    {

        const int arr[] = {90, 77, 63};
        drive.chassis->setMaxVelocity(arr[a]);
        drive.chassis->turnToAngle(angle);
    }

    void moveDistance(CARRYING a, okapi::QLength length)
    {
        const int arr[] = {150, 128, 105};
        drive.chassis->setMaxVelocity(arr[a]);
        drive.chassis->moveDistance(length);
    }
}

void auton_skils()
{
    using namespace skillsn;

    drive.chassis->setState(okapi::OdomState{x : .5_tile, y : 6_in});
    myIMU->setOffset(-90);
    drive.chassis->setMaxVelocity(90);
    pros::delay(20);

    back_claw_set_wait(2); // down claw

    drive.chassis->setMaxVelocity(90);
    drive.chassis->moveDistance(-20_in);
    //drive.chassis->moveDistance()
        back_claw.ArmSetNum(0);

    drive.chassis->setMaxVelocity(60);
    drive.chassis->setSwing(okapi::ChassisController::swing::left);
    drive.turnAngle(90_deg);
    drive.chassis->moveDistance(13.5_in);
    drive.turnAngle(90_deg);
    drive.chassis->setSwing(okapi::ChassisController::swing::none);

    //grap neumogo

    front_intake(4_s, Vision::YELLOW);

    claw.ArmSetNum(3);
    drive.chassis->setMaxVelocity(70);
    // drive.chassis->driveToPoint(okapi::Point{x : 3_tile, y : 5.5_tile}, false, 0_mm);
    drive.chassis->turnToAngle(60_deg);

    drive.chassis->setMaxVelocity(140);
    drive.chassis->moveDistance(62_in);
    claw.ArmSetNum(2);

    claw.WaitUntilSettled();
    claw.Leave();

    //drop neumogo

    drive.chassis->moveDistance(-15_in);
    drive.chassis->setMaxVelocity(70);
    drive.chassis->turnToAngle(0_deg);
    back_claw.ArmSetNum(2);
    back_claw.WaitUntilSettled();

    drive.chassis->setMaxVelocity(140);
    drive.chassis->moveDistance(20_in);
    //retract_all();

    //dropped blue
    //front_intake(5_s, Vision::RED);
    //back_claw.ArmSetNum(1);
    drive.chassis->setMaxVelocity(60);
    drive.chassis->turnToAngle(180_deg);
    back_line_up(3_s, Vision::RED);
    back_claw_set(2);
    drive.chassis->setMaxVelocity(140);
    drive.chassis->moveDistance(-1_tile - 13_in);
    back_claw.ArmSetNum(0);
    //red intake

    drive.chassis->setMaxVelocity(50);
    drive.chassis->turnToAngle(180_deg);
    claw.ArmSetNum(0);
    drive.chassis->moveDistance(14_in);
    drive.chassis->turnToAngle(270_deg);

    front_intake(4_s, Vision::YELLOW);
    //other yellow intake

    claw.ArmSetNum(3);
    drive.chassis->setMaxVelocity(80);
    drive.chassis->turnToAngle(240_deg - 0.2_deg);

    drive.chassis->setMaxVelocity(140);
    drive.chassis->moveDistance(66_in);
    claw.ArmSetNum(2);

    claw.WaitUntilSettled();
    claw.Leave();

    //drop neumogo

    drive.chassis->moveDistance(-7_in);
    drive.chassis->turnToAngle(360_deg);
    back_claw.ArmSetNum(2);
    back_claw.WaitUntilSettled();

    drive.chassis->moveDistance(10_in);
    retract_all();
}
