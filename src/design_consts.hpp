#pragma once

namespace HARDWARE
{

    const char CLAW_PORT = 'A';
    const bool CLAW_REVERSED = false;

    const std::initializer_list<okapi::Motor> drive_motors_left = {1};
    const std::initializer_list<okapi::Motor> drive_motors_right = {-9};

    const double claw_arm_gear_ratio = 12.0 / 84.0;
    const okapi::QAngle claw_max_angle = 90_deg; //angle from the ground to the angle that is high enough to drop the MOGO on the platform
                                                 //TODO

    //encoderUnits don't matter since they're override anyway; gearset has to be set correctly
    const auto CLAW_ARM_MOTORS = {
        okapi::Motor(11, true, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::rotations),
        okapi::Motor(19, false, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::rotations),
    };

    //there needs to be one per motor above
    const std::map<char, std::initializer_list<int>> LIMIT_SWITCHES = {
        {'B', {11, 19}},
    };

}

const int MAX_MOTOR_VOLTAGE = 12;