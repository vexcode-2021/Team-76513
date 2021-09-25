#pragma once

namespace HARDWARE
{

    const char CLAW_PORT = 'A';
    const bool CLAW_REVERSED = false;

    const pros::Motor CLAW_ARM_MOTOR1 = pros::Motor(11, pros::E_MOTOR_GEARSET_06, true);
    const pros::Motor CLAW_ARM_MOTOR2 = pros::Motor(19, pros::E_MOTOR_GEARSET_06, false);

    const std::initializer_list<okapi::Motor> drive_motors_left = {1};
    const std::initializer_list<okapi::Motor> drive_motors_right = {-9};
}

const int MAX_MOTOR_VOLTAGE = 12;