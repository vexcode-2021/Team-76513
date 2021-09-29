#include "../components/piston.hpp"
#include "../design_consts.hpp"
#include "../config_consts.hpp"

class Claw
{
private:
    Piston piston = Piston(HARDWARE::CLAW_PORT, HARDWARE::CLAW_REVERSED);
    bool state = false;

    okapi::MotorGroup mtr = HARDWARE::CLAW_ARM_MOTORS;

    okapi::ADIButton limit = okapi::ADIButton(HARDWARE::CLAW_BOTTOM_LIMIT_SWITCH);

    bool currentlyLTOperating = false;

public:
    Claw() {}
    void init()
    {
        mtr.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
        mtr.setEncoderUnits(okapi::AbstractMotor::encoderUnits::degrees);
    };
    void Clasp()
    {
        state = true;
        printf("CLAW: CLASPING\n");
        return this->piston.extend();
    };
    void Leave()
    {
        state = false;

        printf("CLAW: LEAVING\n");
        return this->piston.retract();
    };
    void Toggle()
    {
        state ^= 1;
        return state ? this->Clasp() : this->Leave();
    }

    void ArmMove(double v)
    {
        if (v > 0.001 | v < -0.001 | true)
            printf("armmove: %f\n", v * CLAW_CONF::arm_top_velocity.convert(1_rpm) / HARDWARE::claw_arm_gear_ratio);
        v = v > 1 ? 1 : v;
        int ans = mtr.moveVelocity(v * CLAW_CONF::arm_top_velocity.convert(1_rpm) / HARDWARE::claw_arm_gear_ratio);
    };

    void ArmUp()
    {
        printf("ArmUp\n");
        ArmMove(.5);
    }
    void ArmDown()
    {
        printf("ArmDown\n");
        ArmMove(-.5);
    }
    void ArmSoftStop()
    {
        printf("ArmSoft Stop: %d\n", !currentlyLTOperating);
        if (!currentlyLTOperating)
            ArmMove(0);
    }

    void ArmTop(bool await = false)
    {
        mtr.moveAbsolute(HARDWARE::claw_max_angle.convert(1_deg), CLAW_CONF::arm_top_velocity.convert(1_rpm));
        //if (await)
    };

    void ArmBottom()
    {
        mtr.moveAbsolute(0, CLAW_CONF::arm_top_velocity.convert(1_rpm));
    };

    void Protect()
    {
        if (limit.changedToPressed()) //TODO tmrw
            ArmMove(0);
        mtr.tarePosition();
    };
};
