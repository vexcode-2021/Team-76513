#include "../components/piston.hpp"
#include "../design_consts.hpp"
#include "../config_consts.hpp"

class Claw
{
private:
    Piston piston = Piston(HARDWARE::CLAW_PORT, HARDWARE::CLAW_REVERSED, false);

    okapi::Motor mtr = HARDWARE::CLAW_ARM_MOTOR1;

    int curr = 0;

    std::shared_ptr<okapi::AsyncPositionController<double, double>> controller;

public:
    Claw() {}
    void init()
    {
        mtr.setBrakeMode(okapi::AbstractMotor::brakeMode::coast);
        mtr.setEncoderUnits(okapi::AbstractMotor::encoderUnits::degrees);
        const okapi::IterativePosPIDController::Gains gains = {0.00002, 0.008, 0.00014};
        controller = okapi::AsyncPosControllerBuilder().withMotor(mtr).withGains(gains).withSensor(HARDWARE::POTL).build();

        ArmSetNum(0);
    };
    void Clasp()
    {
        printf("CLAW: CLASPING\n");
        return this->piston.extend();
    };
    void Leave()
    {

        printf("CLAW: LEAVING\n");
        this->piston.retract();
    };
    bool Toggle()
    {
        printf("CLAW: TOGGLE\n");
        return !this->piston.toggle();
    }

    void ArmSet(double v)
    {
        double val = v;
        double valL = (val * ((HARDWARE::LMAX - HARDWARE::LMIN) / 90)) + HARDWARE::LMIN;

        //printf("%f %f\n", val, valL);

        controller->setTarget(valL);
    }

    void ArmSetRelative(double n)
    {
        const double minvalL = (CLAW_CONF::armPos[0] * ((HARDWARE::LMAX - HARDWARE::LMIN) / 90)) + HARDWARE::LMIN;

        double valL = (n * ((HARDWARE::LMAX - HARDWARE::LMIN) / 90));
        controller->setTarget(std::max(controller->getProcessValue() + valL, minvalL));
    }
    double Get()
    {
        return controller->getProcessValue() * (90.0 / (HARDWARE::LMAX - HARDWARE::LMIN)) - 22;
        // idk y -22 but it helps
    }

    void ArmUp()
    {
        printf("ArmUp\n");
        ArmSetNum(curr + 1);
    }
    void ArmDown()
    {
        printf("ArmDown\n");
        ArmSetNum(curr - 1);
    }

    void WaitUntilSettled()
    {
        if (HARDWARE::PROTO)
            return;

        pros::delay(25);
        controller->waitUntilSettled();
    }

    void ArmSetNum(const int n)
    {

        curr = std::clamp(n, 0, CLAW_CONF::ARM_POS_LEN - 1);
        ArmSet(CLAW_CONF::armPos[curr]);
    }
    void ArmSetNumWait(const int n)
    {
        ArmSetNum(n);
        WaitUntilSettled();
    }
    void ArmTop()
    {
        ArmSetNum(CLAW_CONF::ARM_POS_LEN - 1);
    };
};
