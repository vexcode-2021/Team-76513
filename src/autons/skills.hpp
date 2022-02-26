#pragma once
#include "basics.hpp"
#include "vision.hpp"
#include "../pid_consts.hpp"
#include "../display.hpp"

namespace skillsn
{
    enum CARRYING
    {
        NO_GOAL,
        ONE_GOAL,
        TWO_GOAL,

        SLOW_BC,
        FAST_AF

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
        drive.setMaxVelocity(PID_CONSTS::AUTO_DRIVE_SPEEDS[currently_carrying]);

        auto mygains = PID_CONSTS::AUTO_DRIVE_GAINS;

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

    void monitorStuckage()
    {
        printf("STUCKAGE\n");
        int count = 0;
        while (!drive.chassis->isSettled())
        {
            if (drive.getForce() > 40_n)
                count++;
            else if (count > 0)
                count -= 1;

            // 1 sec of stuck
            if (count > 20)
                drive.chassis->stop();
            // stop will cause return on next loop

            printf("STUCKCOUNT %d\n", count);
            pros::delay(100);
        }
    }
}

void climb()
{
    const okapi::QAngularSpeed LIMIT_ANGLE = 3_deg / 1_s;
    const double DRIVE_SPEED = 0.6;
    const okapi::QLength POST_MOVE_DIST = -0.5_in;

    using namespace skillsn;
    auto med = std::make_unique<okapi::MedianFilter<3>>();
    // auto v = okapi::VelMathFactory::createPtr(360, , 60_ms);
    currently_carrying = SLOW_BC;
    setPID();
    drive.setMaxVelocity(40);
    monitorStuckage();
    okapi::QAngularSpeed dtheta = 1_deg / 1_s;

    auto pid = okapi::IterativePosPIDController({0, ControllerScreen.myvals[1] * 1e-4, ControllerScreen.myvals[2] * 1e-4}, okapi::TimeUtilFactory::withSettledUtilParams(5, 9, 999_ms));
    pid.setIntegralLimits(-DRIVE_SPEED, DRIVE_SPEED);
    pid.setOutputLimits(-DRIVE_SPEED, DRIVE_SPEED);
    pid.setTarget(0);
    pid.step(-25);
    printf("STARTING\n");
    while (!pid.isSettled())
    {
        drive.chassis->getModel()->driveVector(pid.step(med->filter(myIMUy->get())), 0);
        static int count = 0;
        if (!(count % 10))
            printf("print %f %f %f\n", pid.getProcessValue(), pid.getOutput(), pid.getError());

        count++;
        pros::delay(10);
    }
    // while (dtheta < LIMIT_ANGLE || myIMUy->get() < -25)
    //{
    //     dtheta = v->step(myIMUy->get());
    //     drive.chassis->getModel()->driveVector(DRIVE_SPEED, 0);
    //     //moveDistance(1_in);
    //     printf("print %f %f\n", dtheta.convert(1_deg / 1_s), myIMUy->get());
    //     pros::delay(5);
    // }

    drive.chassis->getModel()->driveVector(0.0, 0);
    // moveDistance(POST_MOVE_DIST);
}

void auton_skils()
{
    claw.Leave();
    using namespace skillsn;
    Visions[Vision::FRONT]->sensor->set_exposure(32); // TODO remove

    drive.chassis->setState(okapi::OdomState{x : 5_tile + 2.5_in, y : 5.5_tile});
    myIMU->setOffset(90);

    currently_carrying = NO_GOAL;
    back_claw.ArmSetNumWait(2);
    moveDistance(-8_in);

    back_claw.ArmSetNumWait(1);
    moveDistance(4_in);

    back_claw.ArmSetNumWait(0);
    moveDistance(4_in);

    drive.chassis->setSwing(okapi::ChassisController::swing::left);
    drive.chassis->turnToPoint({4.5_tile, 3_tile});
    drive.chassis->setSwing(okapi::ChassisController::swing::none);
    drive.chassis->driveToPoint({4.5_tile, 3_tile}, false, 13_in);

    currently_carrying = SLOW_BC;
    moveDistance(7_in);
    claw.Clasp();
    claw.ArmSetNum(1);

    currently_carrying = NO_GOAL;
    setPID();
    drive.chassis->driveToPoint({4.7_tile, 2_tile - 13_in});
    back_claw.ArmSetNumWait(2);
    drive.chassis->moveDistanceAsync(15_in);
    monitorStuckage();
    claw.Clasp();
    claw.ArmSetNum(1);

    drive.chassis->driveToPoint({5.5_tile, 1.5_tile}, true, -19_in);
    currently_carrying = SLOW_BC;
    moveDistance(-5_in);
    back_claw.ArmSetNum(0);

    currently_carrying = NO_GOAL;
    setPID();
    drive.chassis->driveToPoint({5.3_tile, 1.5_tile}, true);

    claw.ArmSetNum(3);
    drive.chassis->driveToPoint({5.3_tile, 5.5_tile + 3_in}, true);

    drive.chassis->driveToPoint({4.5_tile, 5.5_tile + 3_in});
    claw.ArmSetNumWait(1);

    currently_carrying = SLOW_BC;
    moveDistance(25_in);

    climb();
}
