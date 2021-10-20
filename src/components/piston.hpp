#include "main.h"

class Piston
{
private:
    pros::ADIDigitalOut port{'-'};
    bool reversed;
    bool state;
    void apply();

public:
     Piston();
     Piston(const char port, const bool reveresd = false);
     void init(const char port, const bool reversed = false);

    void extend();
    void retract();
    void toggle();
};