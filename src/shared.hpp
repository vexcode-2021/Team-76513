#include "main.h"
#include "subsystems/claw.hpp"
#include "subsystems/drivetrain.hpp"

#pragma once

Claw claw;
Drivetrain drive;

okapi::ADIUltrasonic ultrasonic = okapi::ADIUltrasonic(HARDWARE::ULTRASONIC_FRONT_OUTPUT, HARDWARE::ULTRASONIC_FRONT_INPUT, std::make_unique<okapi::MedianFilter<5>>());

pros::Vision *visionsensor = nullptr;