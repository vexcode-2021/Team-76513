#include "../components/piston.hpp"
#include "../design_consts.hpp"

class Claw : private Piston
{
public:
    Claw() : Piston(CONFIG_CLAW_PORT, CONFIG_CLAW_REVERSED){};
    void Clasp() { return extend(); };
    void Leave() { return retract(); };
};