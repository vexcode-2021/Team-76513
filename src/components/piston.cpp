#include "piston.hpp"

Piston::Piston()
{
}

Piston::Piston(const char port, bool reversed)
{
    init(port, reversed);
}
void Piston::init(const char port, bool reversed)
{
    this->port = pros::ADIDigitalOut(port, true);
    this->reversed = reversed;
}

void Piston::retract()
{

    state = true;
    apply();
}

void Piston::extend()
{

    state = false;
    apply();
}

void Piston::toggle()
{
    state ^= 1;
    apply();
}
void Piston::apply()
{
    int err = this->port.set_value(state ^ this->reversed);
    if (err != 1)
    {
        printf("AAAAA\n");
    }
}
