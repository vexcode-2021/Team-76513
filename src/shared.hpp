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
