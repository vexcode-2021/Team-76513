#include "main.h"
#include "../config_consts.hpp"

enum DRIVER {
    DRIVER_CONTROLLER,
    DRIVER_NONE
};

class Drivetrain
{
private:
    CONFIG_DRIVE::MODE mode = CONFIG_DRIVE::DEFAULT_MODE;

public:
DRIVER current_drive_mode = DRIVER_CONTROLLER;
    Drivetrain();
    void init();

    std::shared_ptr<okapi::OdomChassisController> chassis;

    void drive(okapi::Controller driver_controller);

    void toggleMode();
};
