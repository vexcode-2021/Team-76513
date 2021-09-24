#include "../components/piston.hpp"
#include "../design_consts.hpp"

class
{
private:
    Piston piston = Piston(CONFIG_CLAW_PORT, CONFIG_CLAW_REVERSED);
    bool state = false;

public:
    void Clasp()
    {
        state = true;
        return this->piston.extend();
    };
    void Leave()
    {
        state = false;
        return this->piston.retract();
    };
    void Toggle()
    {
        state ^= 1;
        return state ? this->piston.retract() : this->piston.extend();
    }

    void ArmUP() {};
    void ArmUp(int velocity) {};

    void ArmDown() {};

} claw;
