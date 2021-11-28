#pragma once

namespace HARDWARE
{

    const char CLAW_PORT = 'F';
    const bool CLAW_REVERSED = false;

    const auto drive_gearset = okapi::AbstractMotor::gearset::green;

    const static okapi::ChassisScales &drive_chassis_scale = {{4_in, 11.5_in}, okapi::imev5GreenTPR};

    const std::initializer_list<okapi::Motor> drive_motors_left = {6, 7, 5};
    const std::initializer_list<okapi::Motor> drive_motors_right = {-8, -4};

    const double claw_arm_gear_ratio = 12.0 / 84.0;
    const okapi::QAngle claw_max_angle = 95_deg; //auangle from the ground to the angle that is high enough to drop the MOGO on the platform
                                                 //TODO

    //right
    const auto CLAW_ARM_MOTOR1 = okapi::Motor(2, false, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::rotations);
    //left
    const auto CLAW_ARM_MOTOR2 = okapi::Motor(9, true, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::rotations);
    //encoderUnits don't matter since they're overriden anyway; gearset has to be set correctly
    const auto CLAW_ARM_MOTORS = {
        CLAW_ARM_MOTOR1,
        CLAW_ARM_MOTOR2,
    };

    //there needs to be one per motor above
    const std::initializer_list<std::pair<char, int>> LIMIT_SWITCHES = {
        {'H', 2},
        {'G', -9},
    };

    const char ULTRASONIC_FRONT_OUTPUT = 'C';
    const char ULTRASONIC_FRONT_INPUT = 'D';

    const auto POTL = std::make_shared<okapi::Potentiometer>('B');
    const auto POTR = std::make_shared<okapi::Potentiometer>('A');
    const int LMIN = 456;
    const int LMAX = 2252;

    static int RMIN = 416;
    static int RMAX = 2224;


    const auto BACK_CLAW_MOTOR = okapi::Motor(10, true, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::degrees);
    const double BACK_CLAW_RATIO = 12.0/60.;

    const char BACK_CLAW_PORT = 'H';
    const bool BACK_CLAW_REVERSED = false;


}

const int MAX_MOTOR_VOLTAGE = 12;
