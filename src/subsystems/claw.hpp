#include "../components/piston.hpp"
#include "../design_consts.hpp"
#include "../config_consts.hpp"

class Claw
{
private:
    Piston piston = Piston(HARDWARE::CLAW_PORT, HARDWARE::CLAW_REVERSED);
    bool state = false;

    okapi::MotorGroup mtr = HARDWARE::CLAW_ARM_MOTORS;

public:
    Claw()
    {
        mtr.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
    };
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

    void ArmMove(double v)
    {
        v = v > 1 ? 1 : v;
        mtr.moveVelocity(v * CLAW_CONF::arm_top_velocity.convert(1_rpm));
    };
};
