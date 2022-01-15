#pragma once
#include "autons/vision.hpp"
#include "autons/quals.hpp"
#include "autons/skills.hpp"

void testing_routine()
{
	//	right_awp();
	//drop_left_awp();
    //front_line_up(1.8, 3, 3, false, Vision::YELLOW);
	//return;
	grab_tallneu();
	//drop_left_awp();
	//auton_skils();

	return;

	// profileController->moveTo({{1_tile, 0_tile + 18_in, 90_deg}, {1_tile, 1_tile + 18_in, 90_deg}, {3_tile, 3_tile, 45_deg}}, false, false);

	drive.chassis->setState(okapi::OdomState{x : 5_tile, y : 18_in, theta : 90_deg});
	drive.moveDistance(12_in);
	drive.chassis->setMaxVelocity(100);
	drive.chassis->driveToPoint(okapi::Point{x : 3_tile, y : 3_tile}, false, 4_in);

	front_line_up(2, 2, 2, false, Vision::YELLOW);
	drive.moveDistance(2_in);

	claw.Clasp();
	drive.chassis->driveToPoint(okapi::Point{x : 3_tile, y : 2_tile});
	claw.Leave();
}

