#include "main.h"
#include "subsystems/claw.hpp"
#include "subsystems/backclaw.hpp"
#include "subsystems/drivetrain.hpp"
#include "subsystems/vision.hpp"
#include "subsystems/display.hpp"

#pragma once

Claw claw;
BackClaw back_claw;
Drivetrain drive;
std::shared_ptr<okapi::IMU> myIMU = std::make_shared<okapi::IMU>(okapi::IMU(HARDWARE::IMUPORT));
std::shared_ptr<okapi::IMU> myIMUy = std::make_shared<okapi::IMU>(okapi::IMU(HARDWARE::IMUPORT, okapi::IMUAxes::y));

std::shared_ptr<okapi::AsyncMotionProfileController> profileController;

okapi::ADIUltrasonic ultrasonic = okapi::ADIUltrasonic(HARDWARE::ULTRASONIC_FRONT_OUTPUT, HARDWARE::ULTRASONIC_FRONT_INPUT, std::make_unique<okapi::MedianFilter<5>>());

namespace okapi
{
    constexpr QLength tile = 24 * inch;
    namespace literals
    {
        constexpr QLength operator"" _tile(unsigned long long int x)
        {
            return static_cast<double>(x) * tile;
        }

        constexpr QLength operator"" _tile(long double x)
        {
            return static_cast<double>(x) * tile;
        }
    }
}

void pre_auton()
{

    drive.chassis->getModel()->setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
    drive.current_drive_mode = DRIVER_NONE;
}
void post_auton()
{

    drive.chassis->getModel()->setBrakeMode(okapi::AbstractMotor::brakeMode::coast);
    drive.current_drive_mode = DRIVER_CONTROLLER;
}

void auton_init()
{
    profileController =
        okapi::AsyncMotionProfileControllerBuilder()
            .withLimits({
                1.0 / 39.0 * 12.0, // Maximum linear velocity of the Chassis in m/s
                1.0,               // Maximum linear acceleration of the Chassis in m/s/s
                2.0                // Maximum linear jerk of the Chassis in m/s/s/s
            })
            .withOutput(drive.chassis)
            .buildMotionProfileController();
}

void monitored_task(pros::Task a)
{
    std::uint32_t start = pros::millis() + 14500;
    pre_auton();

    pros::delay(10);
    do
    {
        pros::delay(10);
        // printf("%d %d\n", drive.current_drive_mode, DRIVER_CONTROLLER);
    } while (drive.current_drive_mode != DRIVER_CONTROLLER && a.get_state() <= pros::E_TASK_STATE_BLOCKED && start > pros::millis());

    profileController->reset();
    drive.chassis->stop();
    a.remove();
    post_auton();
}
