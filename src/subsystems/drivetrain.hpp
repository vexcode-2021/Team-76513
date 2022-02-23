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
    std::shared_ptr<okapi::IMU> myIMU;
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

    void setBrakeMode(okapi::AbstractMotor::brakeMode b)
    {
        chassis->getModel()->setBrakeMode(b);
    }
    void brake(bool on)
    {
        if (current_drive_mode == DRIVER_CONTROLLER)
            chassis->getModel()->setBrakeMode(on ? okapi::AbstractMotor::brakeMode::hold : okapi::AbstractMotor::brakeMode::coast);
    }

    void setMaxVelocity(double rpm)
    {
        chassis->setMaxVelocity(rpm * HARDWARE::drive_gearset.ratio);
    }

    okapi::QTorque getTorque()
    {
        double sum = 0;
        for (auto i : HARDWARE::drive_motors_left)
            sum += i.getTorque();
        for (auto i : HARDWARE::drive_motors_right)
            sum += i.getTorque();
        return sum * HARDWARE::drive_gearset.ratio * okapi::newton * okapi::meter;
    }
};
