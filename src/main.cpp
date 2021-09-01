/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       C:\Users\karmanyaah.malhotra                              */
/*    Created:      Tue Aug 31 2021                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// MotorA               motor         1
// MotorB               motor         10
// MotorC               motor         11
// MotorD               motor         20
// Controller1          controller
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
#include <cmath>

using namespace vex;
class Drive {

public:
  bool isTank = false;
  double proccessVal(double d) {
    d /= 100;
    d = d * d * d;
    return d * 100;
  }
  void speedAll(double s, velocityUnits u) {
    MotorA.setVelocity(s, u);
    MotorB.setVelocity(s, u);
    MotorC.setVelocity(s, u);
    MotorD.setVelocity(s, u);
  }
  void speedAll(double s, double s2, double s3, double s4, velocityUnits u) {
    MotorA.setVelocity(s, u);
    MotorB.setVelocity(s2, u);
    MotorC.setVelocity(s3, u);
    MotorD.setVelocity(s4, u);
  }

  void spinAll() {
    MotorA.spin(forward);
    MotorB.spin(forward);
    MotorC.spin(forward);
    MotorD.spin(forward);
  }
  void moveAll(double dist, rotationUnits u) {

    MotorA.spinFor(forward, dist, u, false);
    MotorB.spinFor(forward, dist, u, false);
    MotorC.spinFor(forward, dist, u, false);
    MotorD.spinFor(forward, dist, u, false);
    while (!(MotorA.isDone() && MotorB.isDone() && MotorC.isDone() &&
             MotorD.isDone())) {
      printf("NOTDONE!! %d %d %d %d\n", MotorA.isDone(), MotorB.isDone(),
             MotorC.isDone(), MotorD.isDone());

      vex::this_thread::sleep_for(5);
      Brain.Screen.clearScreen();
    }
  }

  void arcade(double v, double h) {
    v = proccessVal(v);
    h = proccessVal(h);
    speedAll(v + h, v - h, v + h, v - h, vex::velocityUnits::pct);
  }

  void tank(double v, double h) {
    speedAll(v, h, v, h, vex::velocityUnits::pct);
  }
};
Drive drive;
void tankswitch() { drive.isTank ^= 1; }

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  drive = Drive();
  Controller1.ButtonA.pressed(tankswitch);

  while (true) {
    if (drive.isTank)
      drive.tank(Controller1.Axis3.position(), Controller1.Axis2.position());
    else
      drive.arcade(Controller1.Axis3.position() * -1,
                   Controller1.Axis1.position());

    drive.spinAll();
    vex::this_thread::sleep_for(2); //msec
  }
}
