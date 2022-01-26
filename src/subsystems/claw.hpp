#include "../components/piston.hpp"
#include "../design_consts.hpp"
#include "../config_consts.hpp"

class Claw
{
private:
    Piston piston = Piston(HARDWARE::CLAW_PORT, HARDWARE::CLAW_REVERSED, true);

    okapi::MotorGroup mtr = okapi::MotorGroup(HARDWARE::CLAW_ARM_MOTORS);

    int curr = 0;

    std::shared_ptr<okapi::AsyncPositionController<double, double>> controllerl;
    std::shared_ptr<okapi::AsyncPositionController<double, double>> controllerr;

public:
    Claw() {}
    void init()
    {
        mtr.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
        mtr.setEncoderUnits(okapi::AbstractMotor::encoderUnits::degrees);
        // const okapi::IterativePosPIDController::Gains gains = {0.0015, 0.0007, 0.00004};
        // const okapi::IterativePosPIDController::Gains gains = {0.0072,0.008,0.00007 };
        const okapi::IterativePosPIDController::Gains gains = {0.00002, 0.008, 0.00014};
        controllerl = okapi::AsyncPosControllerBuilder().withMotor(HARDWARE::CLAW_ARM_MOTOR2).withGains(gains).withSensor(HARDWARE::POTL).build();
        controllerr = okapi::AsyncPosControllerBuilder().withMotor(HARDWARE::CLAW_ARM_MOTOR1).withGains(gains).withSensor(HARDWARE::POTR).build();

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
        double valR = (val * ((HARDWARE::RMAX - HARDWARE::RMIN) / 90)) + HARDWARE::RMIN;

        printf("%f %f %f\n", val, valL, valR);

        controllerl->setTarget(valL);
        controllerr->setTarget(valR);
    }

    void ArmSetRelative(double n)
    {
        const double minvalL = (CLAW_CONF::armPos[0] * ((HARDWARE::LMAX - HARDWARE::LMIN) / 90)) + HARDWARE::LMIN;
        const double minvalR = (CLAW_CONF::armPos[0] * ((HARDWARE::RMAX - HARDWARE::RMIN) / 90)) + HARDWARE::RMIN;

        double valL = (n * ((HARDWARE::LMAX - HARDWARE::LMIN) / 90));
        double valR = (n * ((HARDWARE::RMAX - HARDWARE::RMIN) / 90));
        controllerl->setTarget(std::max(controllerl->getTarget() + valL, minvalL));
        controllerr->setTarget(std::max(controllerr->getTarget() + valR, minvalR));
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
        if (HARDWARE::PROTO) return;

pros::delay(25);
        controllerr->waitUntilSettled();
        controllerl->waitUntilSettled();
    }

    void ArmSetNum(const int n)
    {

        curr = std::clamp(n, 0, CLAW_CONF::ARM_POS_LEN - 1);
        ArmSet(CLAW_CONF::armPos[curr]);
    }

    void ArmTop()
    {
        ArmSetNum(CLAW_CONF::ARM_POS_LEN - 1);
    };
};
