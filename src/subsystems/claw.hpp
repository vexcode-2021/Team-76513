#include "../components/piston.hpp"
#include "../design_consts.hpp"
#include "../config_consts.hpp"

//static okapi::ADIButton **buttonsFromStr(const char *a)
//{
//    static okapi::ADIButton *ret[HARDWARE::CLAW_NUM_MOTORS];
//
//    int count = 0;
//    while (a[count++] != '\0')
//        *ret[count - 1] = okapi::ADIButton(*a);
//    return ret;
//}
//
//static okapi::Motor **motorsFromInitList(std::initializer_list<okapi::Motor> a)
//{
//
//    const int s = a.size();
//    static okapi::Motor *ret[s];
//    int count = 0;
//    for (auto i : a)
//        *ret[count++] = i;
//    return ret;
//}

class Claw
{
private:
    Piston piston = Piston(HARDWARE::CLAW_PORT, HARDWARE::CLAW_REVERSED);
    bool state = false;

    okapi::MotorGroup mtr = okapi::MotorGroup(HARDWARE::CLAW_ARM_MOTORS);

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
        //if (!currentlyLTOperating)
        //maybe not needed now that things r only called on change???
            ArmMove(0);
    }

    void ArmTop(bool await = false)
    {
        ArmUp();
        //lets just try relying on protect to take care of this for once
        //        mtr.moveAbsolute(HARDWARE::claw_max_angle.convert(1_deg), CLAW_CONF::arm_top_velocity.convert(1_rpm));
        //if (await)
    };

    void ArmBottom()
    {
        ArmDown();
        //protect take care should
        //mtr.moveAbsolute(0, CLAW_CONF::arm_top_velocity.convert(1_rpm));
    };

    static void Protect()
    {
        for (auto const &x : HARDWARE::LIMIT_SWITCHES)
            //if switch is pressed AND is moving down
            if (pros::c::adi_digital_read(x.first) &&
                pros::c::motor_get_actual_velocity(x.first) * HARDWARE::claw_arm_gear_ratio < -0.1)
                for (auto l : x.second)
                {
                    pros::c::motor_move_voltage(l, 0);
                    pros::c::motor_tare_position(l);
                }

        const double max_ang = (HARDWARE::claw_max_angle / HARDWARE::claw_arm_gear_ratio).convert(1_deg);
        for (auto i : HARDWARE::CLAW_ARM_MOTORS)
        {
            double gear_velocity_rpm = i.getActualVelocity() * HARDWARE::claw_arm_gear_ratio;

            //stops if beyond limit AND moving up
            if (i.getPosition() > max_ang && gear_velocity_rpm > 0.1)
                i.moveVoltage(0);
        }
    }
};
