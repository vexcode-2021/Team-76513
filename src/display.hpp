#include "shared.hpp"
#pragma once
class
{
private:
    pros::Controller master = pros::Controller(pros::E_CONTROLLER_MASTER);
    pros::Controller partner = pros::Controller(pros::E_CONTROLLER_PARTNER);

    bool needupdate = true;

    bool curr_brake = false;

    void brakeStuff()
    {
        if (pros::c::controller_get_digital_new_press(pros::E_CONTROLLER_PARTNER, ButtonMapping::drive_brake_toggle))
        {
            curr_brake ^= 1;
            needupdate = true;
            // xor equals 1 means toggle a boolean
        }
        drive.brake(curr_brake);
    }

    const bool PID_TUNING = true;
    int index = 0;
    std::valarray<double> myvals = {15, 22, 1};
    std::valarray<double> factors = {1e-4, 1e-4, 1e-6};
    okapi::Timer claw_t;

    void pidTune()
    {
        if (pros::c::controller_get_digital_new_press(pros::E_CONTROLLER_PARTNER, pros::E_CONTROLLER_DIGITAL_A))
        {
            index = (index + 1) % 3;
        }
        if (pros::c::controller_get_digital_new_press(pros::E_CONTROLLER_PARTNER, pros::E_CONTROLLER_DIGITAL_UP))
        {
            myvals[index] *= 1.25;
            needupdate = true;
        }
        else if (pros::c::controller_get_digital_new_press(pros::E_CONTROLLER_PARTNER, pros::E_CONTROLLER_DIGITAL_DOWN))
        {
            myvals[index] /= 1.25;
            needupdate = true;
        }
    }
    void mysetpid()
    {
        auto ans = (myvals * factors);
        std::static_pointer_cast<okapi::ChassisControllerPID>(std::static_pointer_cast<okapi::DefaultOdomChassisController>(drive.chassis)->getChassisController())->setGains({ans[0], ans[1], ans[2]}, {}, {});
    }

    void statusPrint()
    {
        if (claw_t.repeat(1_s))
            partner.print(2, 0, "%3.0f", claw.Get());

        if (!needupdate)
            return;
        pros::delay(55);
        if (PID_TUNING)
        {
            master.print(2, 0, "%4.0f %4.0f, %4.0f ", myvals[0], myvals[1], myvals[2]);
        }
        else
        {
            std::string printing = curr_brake ? "br" : "co";
            master.print(2, 0, "%2s", printing);
        }
        mysetpid();
        needupdate = false;
    }

public:
    void oneLoop()
    {
        brakeStuff();

        if (PID_TUNING)
            pidTune();

        statusPrint();
    };
} ControllerScreen;
