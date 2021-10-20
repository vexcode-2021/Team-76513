#include "main.h"
#include "../config_consts.hpp"

class Drivetrain
{
private:
    CONFIG_DRIVE::MODE mode = CONFIG_DRIVE::DEFAULT_MODE;

public:
    Drivetrain();
    void init();

    std::shared_ptr<okapi::ChassisController> chassis;

    void drive(okapi::Controller driver_controller);

    void toggleMode();
};
