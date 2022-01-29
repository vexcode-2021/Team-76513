#include "../design_consts.hpp"
#include "../components/piston.hpp"
#include "../design_consts.hpp"

class BackClaw
{
private:
    okapi::Motor mtr = HARDWARE::BACK_CLAW_MOTOR;

    int curr = 0;

    std::shared_ptr<okapi::AsyncPositionController<double, double>> controller;

public:
    Piston piston = Piston(HARDWARE::BACK_CLAW_PORT, HARDWARE::BACK_CLAW_REVERSED, true);
    BackClaw() {}
    void init()
    {
        mtr.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
        mtr.setEncoderUnits(okapi::AbstractMotor::encoderUnits::degrees);

        const okapi::IterativePosPIDController::Gains gains = {0.00692, 0.008, 0.00020};
        controller = okapi::AsyncPosControllerBuilder().withMotor(mtr).withGains(gains).build();
    }

    void Clasp()
    {
        return this->piston.extend();
    }

    void Leave()
    {
        this->piston.retract();
    }
    bool Toggle()
    {
        return !this->piston.toggle();
    }

    void ArmSet(double v)
    {
        controller->setTarget(v / HARDWARE::BACK_CLAW_RATIO);
    }

    void ArmSetRelative(double v)
    {
        //TODO change impl to something like that of front claw soon, this behavior is to stay compatible with legacy relative behavior

        ArmSet((controller->getProcessValue() * HARDWARE::BACK_CLAW_RATIO) + v);
    }
    void ArmSetNum(const int n)
    {
        curr = std::clamp(n, 0, BACK_CLAW_CONF::ARM_POS_LEN - 1);
        ArmSet(BACK_CLAW_CONF::armPos[curr]);
    }
    void ArmSetNumWait(const int n)
    {
        ArmSetNum(n);
        WaitUntilSettled();
    }
    void WaitUntilSettled()
    {
        pros::delay(50);
        controller->waitUntilSettled();
        //pros::delay(10);
        //while (!controller->isSettled())
        //{

        //    pros::delay(10);
        //}

        printf("SETTLED %f %f %f %d\n", controller->getError(), controller->getTarget(), controller->getProcessValue(), controller->isSettled());
    }

    double ArmGet()
    {
        return (controller->getProcessValue() * HARDWARE::BACK_CLAW_RATIO);
    }

    void ArmUp()
    {
        ;
        if (curr < BACK_CLAW_CONF::ARM_POS_LEN - 1)
        {
            printf("ArmUp\n");
            ArmSet(BACK_CLAW_CONF::armPos[++curr]);
        }
    }
    void ArmDown()
    {
        if (curr > 0)
        {
            printf("ArmDown\n");
            ArmSet(BACK_CLAW_CONF::armPos[--curr]);
        }
    }
};
