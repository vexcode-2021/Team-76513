#include "main.h"

class Piston
{
private:
    pros::ADIDigitalOut port{'-'};
    bool reversed;

public:
    Piston(const char port, const bool reversed = false);

    void extend();
    void retract();
};