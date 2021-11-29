#include "main.h"
#include "subsystems/claw.hpp"
#include "subsystems/backclaw.hpp"
#include "subsystems/drivetrain.hpp"
#include "subsystems/vision.hpp"

#pragma once

Claw claw;
BackClaw back_claw;
Drivetrain drive;

okapi::ADIUltrasonic ultrasonic = okapi::ADIUltrasonic(HARDWARE::ULTRASONIC_FRONT_OUTPUT, HARDWARE::ULTRASONIC_FRONT_INPUT, std::make_unique<okapi::MedianFilter<5>>());

pros::Vision *visionsensor = nullptr;