#pragma once
#include "autons/vision.hpp"
#include "autons/quals.hpp"
#include "autons/skills.hpp"

void basic_goal_grab()
{
	claw.Leave();

	drive.setMaxVelocity(600);
	drive.chassis->model().tank(1, 1);
	while (drive.chassis->getState().y < 2_tile )
		pros::delay(10);

	drive.chassis->model().tank(0, 0);
	claw.Clasp();

	skillsn::moveDistance(-2_tile);
	skillsn::monitorStuckage();
}

void testing_routine()
{
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
