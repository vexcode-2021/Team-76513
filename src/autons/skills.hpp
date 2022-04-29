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

      std::shared_ptr<okapi::ChassisControllerPID> chassisPID;

    std::shared_ptr<okapi::ChassisControllerPID> getChassisPID()
    {
        if (chassisPID != nullptr) {
            return chassisPID;
        }
        chassisPID = std::static_pointer_cast<okapi::ChassisControllerPID>(std::static_pointer_cast<okapi::DefaultOdomChassisController>(drive.chassis)->getChassisController());
        return getChassisPID();
    }

  void setPID()
    {
        drive.setMaxVelocity(PID_CONSTS::AUTO_DRIVE_SPEEDS[currently_carrying]);

        auto mygains = PID_CONSTS::AUTO_DRIVE_GAINS;

        getChassisPID()->setGains(mygains[currently_carrying][0], mygains[currently_carrying][1], mygains[currently_carrying][2]);
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
    void moveDistanceAsync(okapi::QLength length)
    {
        setPID();
        drive.chassis->moveDistanceAsync(length);
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
    using namespace skillsn;
    currently_carrying = SLOW_BC;
    setPID();

    const okapi::QAngularSpeed LIMIT_ANGLE = 12_deg / 1_s;
    const double DRIVE_SPEED = 1;
    const okapi::QLength POST_MOVE_DIST = -0.5_in;

    auto med = std::make_unique<okapi::MedianFilter<3>>();
    auto v = okapi::VelMathFactory::createPtr(360, std::make_unique<okapi::PassthroughFilter>(okapi::PassthroughFilter()), 120_ms);

    auto settled = okapi::TimeUtilFactory::withSettledUtilParams(3, 3, 2_s);

    drive.setMaxVelocity(20);

    okapi::QAngularSpeed dtheta = 1_deg / 1_s;

    const double P = DRIVE_SPEED / 25;

    med->filter(myIMUy->get());
    med->filter(myIMUy->get());
    med->filter(myIMUy->get());
    printf("STARTING\n");
    do
    {

        static int count = 0;

        auto imuval = myIMUy->get();
        auto p = P;
        if ((count % 20) < 10)
            p *= 0.5;

        auto val = -med->filter(imuval) * p - 0.015 * dtheta.convert(1_deg / 1_s);
        if (dtheta.abs() > LIMIT_ANGLE)
            val -= 0.03 * dtheta.convert(1_deg / 1_s);
        val = std::clamp(val, -DRIVE_SPEED, DRIVE_SPEED);

        drive.chassis->getModel()->driveVector(val, 0);

        if (!(count % 10))
            printf("print %f %f %f\n", imuval, val, dtheta.convert(1_deg / 1_s));

        count++;

        if (!(count % 3))
            dtheta = v->step(med->getOutput());
        // if (!(count % 20))
        //{
        //     drive.chassis->getModel()->driveVector(0, 0);
        //     pros::delay(120);
        // }

        pros::delay(20);
    } while (!settled.getSettledUtil()->isSettled(med->getOutput()));

    drive.chassis->getModel()->driveVector(0.0, 0);
    printf("CLIMBEEDDDD\n");
}

void custom_stuckage()
{
    pros::delay(500);
    skillsn::monitorStuckage();
}
void auton_skils()
{
    claw.Leave();
    using namespace skillsn;
    // Visions[Vision::FRONT]->sensor->set_exposure(32); // TODO remove

    drive.chassis->setState(okapi::OdomState{x : 5_tile + 2.5_in, y : 5.5_tile});
    myIMU->setOffset(90);

    currently_carrying = NO_GOAL;
    back_claw.ArmSetNumWait(2);
    moveDistance(-8_in);

    back_claw.ArmSetNumWait(1);
    moveDistance(4_in);

    back_claw.ArmSetNumWait(0);

    drive.chassis->setSwing(okapi::ChassisController::swing::left);
    drive.chassis->turnToPoint({4.5_tile, 3_tile});
    drive.chassis->setSwing(okapi::ChassisController::swing::none);
    drive.chassis->driveToPoint({4.5_tile, 3_tile}, false);

    currently_carrying = NO_GOAL;
    setPID();
    drive.chassis->driveToPoint({4.7_tile, 2_tile - 13_in});
    back_claw.ArmSetNumWait(2);
    drive.chassis->moveDistanceAsync(10_in);
    monitorStuckage();
    claw.Leave();

    drive.chassis->driveToPoint({5.5_tile, 1.5_tile}, true, -19_in);
    currently_carrying = SLOW_BC;
    moveDistanceAsync(-12_in);
    monitorStuckage();
    back_claw.ArmSetNum(0);

    currently_carrying = NO_GOAL;
    setPID();
    // drive.chassis->driveToPoint({5.3_tile, 1.5_tile}, true);

    claw.ArmSetNum(3);
    drive.chassis->driveToPoint({5.3_tile, 5.0_tile}, true);
    back_claw.ArmSetNumWait(2);

    moveDistance(10_in);
    auto a = pros::Task(custom_stuckage);
    drive.chassis->driveToPoint({0.5_tile, 4.5_tile + 2_in}, true, -13_in);
    // monitorStuckage();
    back_claw.ArmSetNum(0);

    drive.chassis->driveToPoint({1.5_tile, 4.5_tile}, true);
    turnToAngle(0_deg);

    moveDistance(-75_in);
    turnToAngle(45_deg);
    back_claw.ArmSetNumWait(2);
    moveDistance(200_in);
}
