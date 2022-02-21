#pragma once
#include "basics.hpp"
#include "vision.hpp"

namespace skillsn
{
    enum CARRYING
    {
        NO_GOAL,
        ONE_GOAL,
        TWO_GOAL,

        SLOW_BC

    } currently_carrying;

    void retract_all()
    {
        back_claw.ArmSetNum(0);
        claw.ArmSetNum(0);

        back_claw.WaitUntilSettled();
        claw.WaitUntilSettled();
    }

    void setPID()
    {
        const int speeds[] = {130, 110, 90, 60};
        drive.chassis->setMaxVelocity(speeds[currently_carrying]);
        const okapi::IterativePosPIDController::Gains mygains[][3] = {
            {{0.0024 * 2, 0 * 2, 0.00001 * 4.3 * 2}, {0.0001 * 2.8 * 2, 0.00005 * 2, 0.00001 * 1.2 * 2}, {0.0001 * 2.5 * 2, 0.00005 * 2, 0.00001 * 1 * 2}},

            {{0.0024 * 1.6, 0 * 2, 0.00001 * 4 * 2}, {0.0001 * 2.95 * 2, 0.00005 * 2, 0.00001 * 1 * 2}, {0.0001 * 2.5 * 2, 0.00005 * 2, 0.00001 * 1 * 2}},

            {{0.0024 * 1.4, 0 * 2, 0.00001 * 4 * 2}, {0.0001 * 2 * 2, 0.00005 * 2, 0.00001 * 1.35 * 2}, {0.0001 * 2.5 * 2, 0.00005 * 2, 0.00001 * 1 * 2}},

            {{0.0024 * 2, 0 * 2, 0.00001 * 4 * 2}, {0.0001 * 2.95 * 2, 0.00005 * 2, 0.00001 * 1 * 2}, {0.0001 * 2.5 * 2, 0.00005 * 2, 0.00001 * 1 * 2}},
        };

        std::static_pointer_cast<okapi::ChassisControllerPID>(std::static_pointer_cast<okapi::DefaultOdomChassisController>(drive.chassis)->getChassisController())->setGains(mygains[currently_carrying][0], mygains[currently_carrying][1], mygains[currently_carrying][2]);
    }

    void turnToAngle(okapi::QAngle angle, okapi::ChassisController::swing swing = okapi::ChassisController::swing::none)
    {

        setPID();
        if (swing != okapi::ChassisController::swing::none)
            drive.chassis->setSwing(swing);

        drive.chassis->turnToAngle(angle);

        if (swing != okapi::ChassisController::swing::none)
            drive.chassis->setSwing(okapi::ChassisController::swing::none);
    }

    void turnAngle(okapi::QAngle angle, okapi::ChassisController::swing swing = okapi::ChassisController::swing::none)
    {

        setPID();
        if (swing != okapi::ChassisController::swing::none)
            drive.chassis->setSwing(swing);

        drive.chassis->turnAngle(angle);

        if (swing != okapi::ChassisController::swing::none)
            drive.chassis->setSwing(okapi::ChassisController::swing::none);
    }

    void moveDistance(okapi::QLength length)
    {
        setPID();
        drive.chassis->moveDistance(length);
    }
}

void auton_skils()
{
    using namespace skillsn;
    Visions[Vision::FRONT]->sensor->set_exposure(32); // TODO remove

    drive.chassis->setState(okapi::OdomState{x : .5_tile, y : 6_in});
    myIMU->setOffset(-90);
    drive.chassis->setMaxVelocity(90);
    pros::delay(20);
    // now initialized

    back_claw.ArmSetNumWait(2); // down claw

    currently_carrying = SLOW_BC;
    moveDistance(-20_in);
    back_claw.ArmSetNum(0); // pick up
    currently_carrying = ONE_GOAL;
    // blue goal is now picked up

    turnToAngle(0_deg, okapi::ChassisController::swing::left);
    moveDistance(13.5_in);
    turnToAngle(90_deg, okapi::ChassisController::swing::left);
    // now it's pointing to the yellow goal
    front_intake(4_s, Vision::YELLOW);
    // grabbed neumogo
    currently_carrying = TWO_GOAL;

    claw.ArmSetNum(3);
    turnToAngle(60.1_deg); // turn towards platform
    moveDistance(62_in);   // drive to platform
    claw.ArmSetNumWait(2); // set it down on platform
    claw.Leave();          // unhook
    // yellow is now on the platform
    currently_carrying = ONE_GOAL;

    moveDistance(-13_in); // move away from platfom
    turnToAngle(0_deg);
    back_claw.ArmSetNumWait(2); // blue is now set down
    currently_carrying = NO_GOAL;
    moveDistance(20_in); // drive away from set down blue goal

    // go to pick up red goal
    turnToAngle(180_deg);
    back_line_up(2_s, Vision::RED);
    back_claw.ArmSetNumWait(2);    // set down
    moveDistance(-1_tile - 14_in); // intake the red
    back_claw.ArmSetNum(0);        // start lifting the red
    // red intake
    currently_carrying = ONE_GOAL;

    turnToAngle(180_deg);
    claw.ArmSetNum(0); // to save time in the future
    moveDistance(14_in);
    turnToAngle(270_deg);

    front_intake(4_s, Vision::YELLOW);
    // other yellow intake
    currently_carrying = TWO_GOAL;

    claw.ArmSetNum(3);
    turnToAngle(240_deg - 0.3_deg);

    moveDistance(65_in);
    claw.ArmSetNumWait(2);
    claw.Leave(); // unhook yellow
    currently_carrying = ONE_GOAL;

    moveDistance(-7_in);
    turnToAngle(360_deg);
    back_claw.ArmSetNumWait(2);
    currently_carrying = NO_GOAL;

    moveDistance(10_in);
    retract_all();
}
