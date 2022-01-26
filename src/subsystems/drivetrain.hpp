#include "main.h"
#include "../config_consts.hpp"

enum DRIVER
{
    DRIVER_CONTROLLER,
    DRIVER_NONE
};

class Drivetrain
{
private:
    CONFIG_DRIVE::MODE mode = CONFIG_DRIVE::DEFAULT_MODE;

public:
    DRIVER current_drive_mode = DRIVER_CONTROLLER;
    std::shared_ptr<okapi::IMU> myIMU ;
    Drivetrain();
    std::shared_ptr<Drive> chassis2;
    void init();

    std::shared_ptr<okapi::OdomChassisController> chassis;

    void drive(okapi::Controller driver_controller, okapi::Controller c_backup);

    void toggleMode();

    void moveDistance(okapi::QLength itarget)
    {

        chassis->setMaxVelocity(150);
        chassis->moveDistance(itarget);
    };
    void turnAngle(okapi::QAngle itarget)
    {
        chassis->setMaxVelocity(120);
        chassis->turnAngle(itarget * HARDWARE::TURNFACTOR);
    };
};
