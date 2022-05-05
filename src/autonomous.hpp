#pragma once
#include "autons/vision.hpp"
#include "autons/quals.hpp"
#include "autons/skills.hpp"

void basic_goal_grabe(bool mode) // mode isn't read
{

	drive.chassis->setState(okapi::OdomState{x : 0_tile, y : 1_tile - 1_in});
	claw.Leave();

	drive.setMaxVelocity(600);
	// drive.chassis->model().tank(.8, .8);
	drive.chassis->model().left(1);
	drive.chassis->model().right(1);
	while (drive.chassis->getState().y < 3_tile - 6_in - 14_in)
		pros::delay(10);

	drive.chassis->model().left(.3);
	drive.chassis->model().right(.3);
	while (drive.chassis->getState().y < 3_tile - 6_in)
		pros::delay(10);

	drive.chassis->model().tank(0, 0);
	claw.Clasp();

	skillsn::moveDistance(-2_tile);
}

void basic_goal_grab()
{
	basic_goal_grabe(true);
}

void basic_neugoal_grab(bool move = true)
{

	drive.chassis->setState(okapi::OdomState{x : 0_tile, y : 0_tile + 0_in});
	claw.Leave();

	drive.setMaxVelocity(600);
	// drive.chassis->model().tank(.8, .8);
	drive.chassis->model().left(1);
	drive.chassis->model().right(1);
	while (drive.chassis->getState().y < 3_tile - 2_in - 19_in)
		pros::delay(10);

	drive.chassis->model().left(.3);
	drive.chassis->model().right(.3);
	while (drive.chassis->getState().y < 3_tile - 2_in - 2_in)
		pros::delay(10);

	drive.chassis->model().tank(0, 0);
	claw.Clasp();

	if (move)
	{
		skillsn::moveDistance(-2_tile);
	}
}
void basic_neugoal_grab_true() {
	basic_neugoal_grab(true);
}

void myawp_left()
{
	using namespace skillsn;
	back_piston_extend_retract_seq();
	pros::delay(1200);
	turnAngle(100_deg, okapi::ChassisController::swing::left);

	myIMU->setOffset(-90);
	basic_goal_grab();
}

void right_awp_grab() {
	using namespace skillsn;
	drive.chassis->setState(okapi::OdomState{x : 5.5_tile, y : 0_tile + 7.5_in});
	myIMU->setOffset(-180);
	currently_carrying = SLOW_BC;
	moveDistance(-8_in);

	back_claw.Clasp();
	awp_piston->extend();
	pros::delay(500);
	awp_piston->retract();

	currently_carrying = NO_GOAL;
	turnAngle(135_deg + 2_deg, okapi::ChassisController::swing::left);
	back_claw.Leave();
	claw.Leave();
	//moveDistance(2_tile + 6_in);
	//currently_carrying = SLOW_BC;
	//moveDistance(8_in);

	drive.chassis->model().left(1);
	drive.chassis->model().right(1);
	while (drive.chassis->getState().y < 3_tile - 2_in - 19_in - 10_in)
		pros::delay(10);

	drive.chassis->model().left(.3);
	drive.chassis->model().right(.3);
	while (drive.chassis->getState().y < 3_tile - 2_in - 2_in - 10_in)
		pros::delay(10);

	claw.Clasp();
	currently_carrying = NO_GOAL;
	moveDistance(-2_tile);
}

void testing_routine()
{

	using namespace skillsn; // don't do anything in real match
	return;
basic_neugoal_grab();return;
		return;

		//grab tall and then short
		// doesn't work because swing needs to traverse a little bit first
	//basic_neugoal_grab();
	//turnAngle(-38_deg - 9.5_deg - 90_deg);
	//claw.Leave();
	//moveDistance(-24.5_in);
	//back_claw.Clasp();
	//pros::delay(300);
	//moveDistance(25_in);

	return;
	// idk
	//turnAngle(-(360_deg - 2 * (5.0 / 7.0 * 180_deg)), okapi::ChassisController::swing::left);
	//moveDistance(-4_in);
	//back_claw.Toggle();
	//pros::delay(200);
	//// turnAngle(-38_deg);// -38 lines it up
	//turnToAngle(-55_deg); // a little more makes it not run into the platform
	//moveDistance(2_tile);

	//	basic_goal_grab();
	return;
	back_claw.Toggle();
	turnAngle(100_deg, okapi::ChassisController::swing::left);

	drive.chassis->setState(okapi::OdomState{x : 0_tile, y : 1_tile});
	myIMU->setOffset(90);
	basic_goal_grab();

	return;
	// auton_skils();
	climb();
	return;

	drive.chassis->moveDistanceAsync(3_tile);
	skillsn::monitorStuckage();
	drive.chassis->moveDistance(-3_in);
	drive.chassis->moveDistance(-12_in);
	drive.chassis->moveDistance(-24_in);

	return;

	const okapi::QAngularSpeed LIMIT_ANGLE = -7_deg / 1_s;
	const double DRIVE_SPEED = 0.45;
	const okapi::QLength POST_MOVE_DIST = -0.5_in;

	using namespace skillsn;

	auto v = okapi::VelMathFactory::createPtr(360, 89_ms);
	currently_carrying = SLOW_BC;
	moveDistance(25_in);
	okapi::QAngularSpeed dtheta = 1_deg / 1_s;
	while (dtheta > LIMIT_ANGLE)
	{
		dtheta = v->step(myIMUy->get());
		drive.chassis->getModel()->driveVector(DRIVE_SPEED, 0);
		printf("print %f\n", dtheta.convert(1_deg / 1_s));
		pros::delay(30);
	}
	drive.chassis->getModel()->driveVector(0.0, 0);
	moveDistance(POST_MOVE_DIST);

	return;

	// right_awp();
	grab_neuu(true);

	currently_carrying = NO_GOAL;
	moveDistance(10_in);
	printf("DONEEEE\n");
	pros::delay(1000);
	currently_carrying = FAST_AF;
	moveDistance(10_in);

	// noooo//grab_neuu(false);

	// drop_left_awp();
}
