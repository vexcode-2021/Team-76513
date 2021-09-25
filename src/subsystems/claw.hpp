#include "../components/piston.hpp"
#include "../design_consts.hpp"

class Claw
{
private:
    Piston piston = Piston(HARDWARE::CLAW_PORT, HARDWARE::CLAW_REVERSED);
    bool state = false;

    pros::Motor mtr1 = HARDWARE::CLAW_ARM_MOTOR1;
    pros::Motor mtr2 = HARDWARE::CLAW_ARM_MOTOR2;

public:
    Claw()
    {
        mtr1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        mtr2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
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

    void ArmMove(float v)
    {
        v = v / MAX_MOTOR_VOLTAGE * 128;
        mtr1.move(v);
        mtr2.move(v);
    };
};
