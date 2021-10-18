#pragma once

namespace HARDWARE
{

    const char CLAW_PORT = 'F';
    const bool CLAW_REVERSED = false;

    const auto drive_gearset = okapi::AbstractMotor::gearset::green;

    const static okapi::ChassisScales &drive_chassis_scale = {{4_in, 11.5_in}, okapi::imev5BlueTPR};

    const std::initializer_list<okapi::Motor> drive_motors_left = {14, 11};
    const std::initializer_list<okapi::Motor> drive_motors_right = {-13, -12};

    const double claw_arm_gear_ratio = 12.0 / 84.0;
    const okapi::QAngle claw_max_angle = 95_deg; //angle from the ground to the angle that is high enough to drop the MOGO on the platform
                                                 //TODO

    //encoderUnits don't matter since they're overriden anyway; gearset has to be set correctly
    const auto CLAW_ARM_MOTORS = {
        okapi::Motor(15, false, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::rotations),
        okapi::Motor(16, true, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::rotations),
    };

    //there needs to be one per motor above
    const std::initializer_list<std::pair<char, int>> LIMIT_SWITCHES = {
        {'H', 15},
        {'G', -16},
    };

}

const int MAX_MOTOR_VOLTAGE = 12;