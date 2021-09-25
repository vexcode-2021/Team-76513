#include "drivetrain.hpp"
#include "../design_consts.hpp"

using namespace CONFIG_DRIVE;
using namespace okapi;

Drivetrain::Drivetrain()
{
    chassis =
        okapi::ChassisControllerBuilder()
            .withMotors(okapi::MotorGroup(HARDWARE::drive_motors_left), okapi::MotorGroup(HARDWARE::drive_motors_right))
            // Green gearset, 4 in wheel diam, 11.5 in wheel track
            .withDimensions(okapi::AbstractMotor::gearset::blue, {{4_in, 11.5_in}, okapi::imev5BlueTPR})
            .build();
}

void Drivetrain::drive(Controller m_c)
{
    if (mode == DRIVE_MODE_TANK)
        chassis->getModel()->tank(
            m_c.getAnalog(ButtonMapping::tank_drive[0]),
            m_c.getAnalog(ButtonMapping::tank_drive[1]));
    else if (mode == DRIVE_MODE_ARCADE)
        chassis->getModel()->arcade(
            m_c.getAnalog(ButtonMapping::arcade_drive[0]),
            m_c.getAnalog(ButtonMapping::arcade_drive[1]));
    else
        toggleMode();
}

void Drivetrain::toggleMode()
{
    if (!FIXED_MODE)
        mode = (MODE)((mode + 1) % COUNT_OR_INVALID);
}