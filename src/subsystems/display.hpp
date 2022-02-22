class
{
private:
    pros::Controller master();
    pros::Controller partner = pros::Controller(pros::E_CONTROLLER_PARTNER);

    std::vector<double> vals = {};

    bool curr_brake = false;

    void brakeStuff()
    {
        if (pros::c::controller_get_digital_new_press(pros::E_CONTROLLER_PARTNER, ButtonMapping::drive_brake_toggle))
        {
            curr_brake ^= 1;
            //xor equals 1 means toggle a boolean
        }
        drive.brake(curr_brake);
    }

    void statusPrint()
    {
        std::string printing = curr_brake ? "br" : "co";

        partner.print(1, 0, "%2s", printing.c_str());
    }

public:
    void oneLoop()
    {
        brakeStuff();

        statusPrint();
    };
} ControllerScreen;