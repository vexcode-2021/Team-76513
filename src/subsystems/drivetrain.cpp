#include "drivetrain.hpp"
#include "../design_consts.hpp"
#include "../config_consts.hpp"

using namespace CONFIG_DRIVE;
using namespace okapi;

Drivetrain::Drivetrain()
{
}

double curve(double n)
{

    if constexpr (CONFIG_DRIVE::SLOW_MODE)
    {
        return n / 4;
    }
    const double A = 0.2, B = 0.985, C = 3.2, D = 10.4, G = 0.2, F = -0.01;
    return copysign(A / (A * B + C * exp(-D * (fabs(n) - G))) + F, n);
    return n * fabs(n);
}

double curve2(double x)
{
    x *= 127;

    x = (powf(2.718, -(CONFIG_DRIVE::right_curve / 10)) + powf(2.718, (fabs(x) - 127) / 10) * (1 - powf(2.718, -(CONFIG_DRIVE::right_curve / 10)))) * x;
    return x / 127.0;
}

void Drivetrain::init()
{
    Logger::setDefaultLogger(
        std::make_shared<Logger>(
            TimeUtilFactory::createDefault().getTimer(), // It needs a Timer
            "/ser/sout",                                 // Output to the PROS terminal
            Logger::LogLevel::info                       // Show errors and warnings
            ));

    auto l = okapi::MotorGroup(HARDWARE::drive_motors_left);
    auto r = okapi::MotorGroup(HARDWARE::drive_motors_right);
    chassis =
        okapi::ChassisControllerBuilder()
            .withMotors(l, r)
            .withSensors(l.getEncoder(), r.getEncoder(), (shared_ptr<ContinuousRotarySensor>)myIMU)
            // Green gearset, 4 in wheel diam, 11.5 in wheel track
            .withDimensions(HARDWARE::drive_gearset, HARDWARE::drive_chassis_scale)
            //.withGains({0.016, 0, 0.001}, {}, {0,0,0})
            .withGains({0.0024 * 2, 0 * 2, 0.00001 * 5 * 2}, {0.0001 * 2.95 * 2, 0.00005 * 2, 0.00001 * 1 * 2}, {0.0001 * 2.5 * 2, 0.00005 * 2, 0.00001 * 1 * 2})
            .withSlewRate(10.0 / 400.0)
            .withChassisControllerTimeUtilFactory(ConfigurableTimeUtilFactory(50, 5, .2_s))
            .withOdometry()
            .buildOdometry();
    chassis->setDefaultStateMode(okapi::StateMode::CARTESIAN);

    // chassis2 = std::make_shared<Drive>(Drive(HARDWARE::drive_motors_left2, HARDWARE::drive_motors_right2, HARDWARE::IMUPORT, HARDWARE::drive_chassis_scale.wheelDiameter.convert(1_in), 200, 1));

    printf("HIII\n============================\n\n\n\n\n\n");
    // straight - kU = 0.02 pU = 0.5 at 50rpm

    // for (double i = -1; i <= 1; i += 0.1)
    //     printf("CURVE DEBUG: %f = %f\n", i, curve(i));
}

void Drivetrain::drive(Controller m_c, Controller m_d)
{
    auto b3 = m_c.getAnalog(ButtonMapping::arcade_drive[0]);
    auto b4 = m_c.getAnalog(ButtonMapping::arcade_drive[1]);

    auto c3 = m_d.getAnalog(ButtonMapping::arcade_drive[0]);
    auto c4 = m_d.getAnalog(ButtonMapping::arcade_drive[1]);
    if (fabs(c3) > 0.02 || fabs(c4) > 0.02)
        b3 = c3, b4 = c4;

    if (current_drive_mode == DRIVER_CONTROLLER)
    {
        chassis->setMaxVelocity(600);
        // if (mode == DRIVE_MODE_TANK)
        // chassis->getModel()->tank(
        // curve(b1),
        // curve(b2));
        if (mode == DRIVE_MODE_ARCADE)
        {
            // printf("%f %f\n", b3, b4);
            static std::valarray<int32_t> prev = std::valarray<std::int32_t>{0, 0};
            static std::valarray<int32_t> prev2 = std::valarray<std::int32_t>{0, 0};
            // if (fabs(b3) > 0.05 || fabs(b4) > 0.05)
            //{
            prev = {0, 0};
            prev2 = chassis->getModel()->getSensorVals();
            chassis->getModel()->arcade(curve(b3), curve2(b4));
            //}
            // else
            //{
            // TODO
            // const double MYCONST = 0.01;
            // auto v = chassis->getModel()->getSensorVals();

            // prev[0] = ((double)prev2[0] - v[0]) + prev[0] * 0.8;
            // prev[1] = ((double) prev2[1] - v[1]) + prev[1] * 0.8;

            // printf("%d-%d\n", prev2[0], v[0]);
            // printf("%d-%d\n", prev2[1], v[1]);
            // printf("%f %f\n", prev[0], prev[1]);

            ////prev2 = v;

            // chassis->getModel()->tank(prev[0] * MYCONST, prev[1] * MYCONST);
            //}
        }
        else
            toggleMode();
    }

    static int ct = 0;
    if (fabs(b3) > 0.2 || fabs(b4) > 0.2)
        ct++;
    else
        ct = 0;
    if (ct >= 10)
        current_drive_mode = DRIVER_CONTROLLER;
}

void Drivetrain::toggleMode()
{
    if (!FIXED_MODE)
        mode = (MODE)((mode + 1) % COUNT_OR_INVALID);
}
