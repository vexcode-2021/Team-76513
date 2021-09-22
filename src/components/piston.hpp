#include "main.h"

class Piston
{
private:
    pros::ADIDigitalOut port{'A'};
    bool reversed;

public:
    Piston(const char *port, bool reversed = false);

    void extend();
    void retract();
};