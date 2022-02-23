#pragma once

namespace HARDWARE
{

    const char CLAW_PORT = 'F';
    const bool CLAW_REVERSED = true;

    // motor rotation to wheel rotation
    const okapi::AbstractMotor::GearsetRatioPair drive_gearset = {okapi::AbstractMotor::gearset::blue, 7. / 3.};

    const static okapi::ChassisScales &drive_chassis_scale = {{4.125_in, 12.75_in}, okapi::imev5BlueTPR * 7. / 3.};
    const double TURNFACTOR = 1.00;

    const std::initializer_list<okapi::Motor> drive_motors_left = {-17, 18, -19};
    const std::initializer_list<int> drive_motors_left2 = {-17, 18, -19};
    const std::initializer_list<okapi::Motor> drive_motors_right = {12, -13, 15};
    const std::initializer_list<int> drive_motors_right2 = {12, -13, 15};

    const double claw_arm_gear_ratio = 12.0 / 84.0;
    const okapi::QAngle claw_max_angle = 95_deg; // auangle from the ground to the angle that is high enough to drop the MOGO on the platform
                                                 // TODO

    // right
    const auto CLAW_ARM_MOTOR1 = okapi::Motor(1, false, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::rotations);
    // encoderUnits don't matter since they're overriden anyway; gearset has to be set correctly

    // there needs to be one per motor above
    const std::initializer_list<std::pair<char, int>> LIMIT_SWITCHES = {
        {'H', 2},
        {'G', -9},
    };

    const char ULTRASONIC_FRONT_OUTPUT = 'C';
    const char ULTRASONIC_FRONT_INPUT = 'D';

    const auto POTL = std::make_shared<okapi::Potentiometer>('B');
    const auto POTR = std::make_shared<okapi::Potentiometer>('A');
    const int LMIN = 409;
    const int LMAX = 2050;

    const int RMIN = 333;
    const int RMAX = 1969;

    const auto BACK_CLAW_MOTOR = okapi::Motor(16, true, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::degrees);
    const double BACK_CLAW_RATIO = 12.0 / 60.;

    const char BACK_CLAW_PORT = 'E';
    const bool BACK_CLAW_REVERSED = true;

    const int VISION_FRONT = 17;
    const int VISION_BACK = 14;
    const int IMUPORT = 20;

    const bool PROTO = false;

}

const int MAX_MOTOR_VOLTAGE = 12;
