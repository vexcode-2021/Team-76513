namespace CONFIG_DRIVE
{
    enum MODE
    {
        DRIVE_MODE_TANK,
        DRIVE_MODE_ARCADE,
        COUNT_OR_INVALID
    };

    //don't allow changing the mode from default
    const bool FIXED_MODE = false;

    // default mode that the robot starts in
    const MODE DEFAULT_MODE = DRIVE_MODE_TANK;

    const int delay_msec = 10;

}

namespace ButtonMapping
{

    using namespace okapi;

    const int delay_msec = 20;

    const ControllerId drive_controller = ControllerId::master;

    const ControllerAnalog tank_drive[] = {
        ControllerAnalog::leftX,
        ControllerAnalog::leftY,
    };

    const ControllerAnalog arcade_drive[] = {
        ControllerAnalog::leftY,
        ControllerAnalog::leftX,
    };

    const ControllerDigital drive_mode_switch = ControllerDigital::A;

    //handles both the claw and the arm
    const ControllerId claw_controller = ControllerId::master;

    const ControllerDigital claw_hook_button = ControllerDigital::B;

    const ControllerDigital claw_arm_up = ControllerDigital::R1;
    const ControllerDigital claw_arm_down = ControllerDigital::R2;

}
