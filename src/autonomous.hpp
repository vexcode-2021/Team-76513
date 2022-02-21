#pragma once
#include "autons/vision.hpp"
#include "autons/quals.hpp"
#include "autons/skills.hpp"

void testing_routine()
{

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
