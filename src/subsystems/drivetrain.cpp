#include "drivetrain.hpp"
#include "../design_consts.hpp"

using namespace CONFIG_DRIVE;
using namespace okapi;

Drivetrain::Drivetrain()
{
}

double curve(double n)
{

    if constexpr (CONFIG_DRIVE::SLOW_MODE)
    {
        return n / 4;
    }
    const double A = 0.2, B = 0.985, C = 3.2, D = 10.4, G = 0.2, F = -0.01;
    return copysign(A / (A * B + C * exp(-D * (fabs(n) - G))) + F, n);
    return n * fabs(n);
}

void Drivetrain::init()
{
    chassis =
        okapi::ChassisControllerBuilder()
            .withMotors(okapi::MotorGroup(HARDWARE::drive_motors_left), okapi::MotorGroup(HARDWARE::drive_motors_right))
            // Green gearset, 4 in wheel diam, 11.5 in wheel track
            .withDimensions(HARDWARE::drive_gearset, HARDWARE::drive_chassis_scale)
            .withOdometry()
            .buildOdometry();

    //for (double i = -1; i <= 1; i += 0.1)
    //    printf("CURVE DEBUG: %f = %f\n", i, curve(i));
}

void Drivetrain::drive(Controller m_c)
{
    auto b1 = m_c.getAnalog(ButtonMapping::tank_drive[0]);
    auto b2 = m_c.getAnalog(ButtonMapping::tank_drive[1]);
    auto b3 = m_c.getAnalog(ButtonMapping::arcade_drive[0]);
    auto b4 = m_c.getAnalog(ButtonMapping::arcade_drive[1]);
    if (current_drive_mode == DRIVER_CONTROLLER)
    {
        if (mode == DRIVE_MODE_TANK)
            chassis->getModel()->tank(
                curve(b1),
                curve(b2));
        else if (mode == DRIVE_MODE_ARCADE)
            chassis->getModel()->arcade(
                curve(b3),
                curve(b4));
        else
            toggleMode();
    }

    static int ct = 0;
    if (fabs(b1) > 0.2 || fabs(b2) > 0.2 || fabs(b3) > 0.2 || fabs(b4) > 0.2)
        ct++;
    else
        ct = 0;
    if (ct >= 10)
        current_drive_mode = DRIVER_CONTROLLER;
}

void Drivetrain::toggleMode()
{
    if (!FIXED_MODE)
        mode = (MODE)((mode + 1) % COUNT_OR_INVALID);
}