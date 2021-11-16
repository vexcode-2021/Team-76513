#include "../components/piston.hpp"
#include "../design_consts.hpp"
#include "../config_consts.hpp"

class Claw
{
private:
    Piston piston = Piston(HARDWARE::CLAW_PORT, HARDWARE::CLAW_REVERSED);

    okapi::MotorGroup mtr = okapi::MotorGroup(HARDWARE::CLAW_ARM_MOTORS);

    bool currentlyLTOperating = false;
    int curr = 0;

    int motors = 2;
    std::shared_ptr<okapi::AsyncPositionController<double, double>> controllerl;
    std::shared_ptr<okapi::AsyncPositionController<double, double>> controllerr;

public:
    Claw() {}
    void init()
    {
        mtr.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
        mtr.setEncoderUnits(okapi::AbstractMotor::encoderUnits::degrees);
        //const okapi::IterativePosPIDController::Gains gains = {0.0015, 0.0007, 0.00004};
        //const okapi::IterativePosPIDController::Gains gains = {0.0072,0.008,0.00007 };
        const okapi::IterativePosPIDController::Gains gains = {0.00002,0.008,0.00007 };
        controllerl = okapi::AsyncPosControllerBuilder().withMotor(HARDWARE::CLAW_ARM_MOTOR2).withGains(gains).withSensor(HARDWARE::POTL).build();
        controllerl->setMaxVelocity(20);
        controllerr = okapi::AsyncPosControllerBuilder().withMotor(HARDWARE::CLAW_ARM_MOTOR1).withGains(gains).withSensor(HARDWARE::POTR).build();
        controllerr->setMaxVelocity(20);

        ArmSet(3);
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
    void Motor2Hold(bool t)
    {
        auto mtr = HARDWARE::CLAW_ARM_MOTOR2;
        if (t)
        {
            mtr.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
            motors = 2;
        }
        else
        {
            mtr.setBrakeMode(okapi::AbstractMotor::brakeMode::coast);
            motors = 1;
        }
    }

    void ArmSet(double v)
    {
        //int code;
        //okapi::Motor mttr = HARDWARE::CLAW_ARM_MOTOR1;
        //        if (motors == 1)
        //            code = mttr.moveAbsolute(v / HARDWARE::claw_arm_gear_ratio, 0.5 * CLAW_CONF::arm_top_velocity.convert(1_rpm) / HARDWARE::claw_arm_gear_ratio);
        //        else
        //            code = mtr.moveAbsolute(v / HARDWARE::claw_arm_gear_ratio, 0.5 * CLAW_CONF::arm_top_velocity.convert(1_rpm) / HARDWARE::claw_arm_gear_ratio);
        //

        double val = v;
        double val2 = (val * ((HARDWARE::LMAX - HARDWARE::LMIN) / 90)) + HARDWARE::LMIN;
        double val3 = (val * ((HARDWARE::RMAX - HARDWARE::RMIN) / 90)) + HARDWARE::RMIN;

        printf("%f %f %f\n", val, val2, val3);

        controllerl->setTarget(val2);
        controllerr->setTarget(val3);

        //if (code != 1)
        //{
        //    printf("AAAAAAAAAAAAAAAAAAA %d\n", code);
        //}
    }

    void ArmUp()
    {
        ;
        if (curr < CLAW_CONF::ARM_POS_LEN - 1)
        {
            printf("ArmUp\n");
            ArmSet(CLAW_CONF::armPos[++curr]);
        }
    }
    void ArmDown()
    {
        if (curr > 0)
        {
            printf("ArmDown\n");
            ArmSet(CLAW_CONF::armPos[--curr]);
        }
    }

    void WaitUntilSettled()
    {
        const auto adjustedvel = CLAW_CONF::min_zeroed_velocity.convert(1_rpm) / HARDWARE::claw_arm_gear_ratio;
        printf("%f\n", adjustedvel);
        while (mtr.getActualVelocity() < -adjustedvel || mtr.getActualVelocity() > adjustedvel)
        {
            pros::delay(10);
        }
    }

    void ArmSetNum(const int n)
    {
        if (n >= 0 && n < CLAW_CONF::ARM_POS_LEN)
        {
            curr = n;
            ArmSet(CLAW_CONF::armPos[n]);
        }
    }

    void ArmSoftStop()
    {
        //printf("ArmSoft Stop: %d\n", CLAW_CONF::armPos[curr]);
        //if (!currentlyLTOperating)
        //maybe not needed now that things r only called on change???
        //ArmMove(0);
    }

    void ArmTop(bool await = false)
    {
        ArmSet(CLAW_CONF::armPos[CLAW_CONF::ARM_POS_LEN - 1]);
        //lets just try relying on protect to take care of this for once
        //        mtr.moveAbsolute(HARDWARE::claw_max_angle.convert(1_deg), CLAW_CONF::arm_top_velocity.convert(1_rpm));
        //if (await)
    };

    static void Protect()
    {
        static int count = 0;
        count++;

        const double max_ang = (HARDWARE::claw_max_angle / HARDWARE::claw_arm_gear_ratio).convert(1_deg);

        for (auto const &x : HARDWARE::LIMIT_SWITCHES)
        {

            int port_num = x.second;

            double vel = (port_num / abs(port_num)) * pros::c::motor_get_actual_velocity(abs(port_num)) * HARDWARE::claw_arm_gear_ratio;

            double pos = (port_num / abs(port_num)) * pros::c::motor_get_position(abs(port_num));

            if (CLAW_CONF::PRINT)
            {
                if (!(count % 400))
                {
                    printf("%c %d %f %f %d %d\n", x.first, port_num, vel, pos, count, pros::c::adi_digital_read(x.first));
                }
            }

            static int ct = 0;
            if (ct < 10)
            {
                if (pros::c::adi_digital_read(x.first))
                {
                    printf("INIT TARE\n");
                    pros::c::motor_tare_position(abs(port_num));
                }
                ct++;
            }

            //if switch is pressed AND is moving down
            if (pros::c::adi_digital_get_new_press(x.first))
            {
                if (CLAW_CONF::PRINT)
                    printf("stopping %c %d %f %f %d %d\n", x.first, port_num, vel, pos, count, pros::c::adi_digital_read(x.first));

                pros::c::motor_tare_position(abs(port_num));
                pros::c::motor_move_voltage(abs(port_num), 0);
                //pros::c::motor_move_absolute(abs(port_num), 0 / HARDWARE::claw_arm_gear_ratio, 1);
            }

            if (vel > CLAW_CONF::min_zeroed_velocity.convert(1_rpm) / HARDWARE::claw_arm_gear_ratio / 10 && pos >= max_ang)
            {
                pros::c::motor_move_voltage(abs(port_num), 0);
            }
        }
    }
};
