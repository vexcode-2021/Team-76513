#include "drivetrain.hpp"
#include "../design_consts.hpp"

using namespace CONFIG_DRIVE;
using namespace okapi;

Drivetrain::Drivetrain()
{
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
}

double curve(double n)
{
    return n * fabs(n) ;
}

void Drivetrain::drive(Controller m_c)
{
    if (mode == DRIVE_MODE_TANK)
        chassis->getModel()->tank(
            curve(m_c.getAnalog(ButtonMapping::tank_drive[0])),
            curve(m_c.getAnalog(ButtonMapping::tank_drive[1])));
    else if (mode == DRIVE_MODE_ARCADE)
        chassis->getModel()->arcade(
            curve(m_c.getAnalog(ButtonMapping::arcade_drive[0])),
            curve(m_c.getAnalog(ButtonMapping::arcade_drive[1])));
    else
        toggleMode();
}

void Drivetrain::toggleMode()
{
    if (!FIXED_MODE)
        mode = (MODE)((mode + 1) % COUNT_OR_INVALID);
}
