#include "piston.hpp"

Piston::Piston(const char* port, bool reversed)
{
    this->port = pros::ADIDigitalOut(port[0]);
    this->reversed = reversed;
}

void Piston::retract()
{
    int err = this->port.set_value(true ^ this->reversed);
    if (err != 1)
    {
        printf("AAAAA\n");
    }
}

void Piston::extend()
{
    int err = this->port.set_value(false ^ this->reversed);
    if (err != 1)
    {
        printf("AAAAA\n");
    }
}