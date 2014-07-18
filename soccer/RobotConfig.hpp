#pragma once

#include <Configuration.hpp>

/**
 * @brief Configuration per robot model
 */
class RobotConfig {
public:
	RobotConfig(Configuration *config, QString prefix);
	~RobotConfig();
	
	struct PID {
		PID(Configuration *config, QString prefix);
		
		ConfigDouble *p;
		ConfigDouble *i;
		ConfigInt *i_windup;	///	how many past errors to store.  -1 means store all
		ConfigDouble *d;
	};

	struct Kicker {
		Kicker(Configuration *config, QString prefix);

		///	these limits are applied before sending the actual commands to the robots
		ConfigDouble *maxKick;
		ConfigDouble *maxChip;
		// ConfigDouble *passKick;
	};
	
	PID translation;
	PID rotation;

	Kicker kicker;

	///	convert from real units to bot "units"
	ConfigDouble *velMultiplier;
	ConfigDouble *angleVelMultiplier;

	//	when pivoting, we multiply the calculated x-velocity
	//	of the robot by this value before sending it to the robot
	ConfigDouble *pivotVelMultiplier;
};


/**
 * Provides per-robot overrides for a robot
 * Should be updated for hardware revision
 */
class RobotStatus {
public:
	RobotStatus(Configuration *config, QString prefix);
	~RobotStatus() {}

	ConfigBool *chipper_enabled;
	ConfigBool *kicker_enabled;
	ConfigBool *ball_sense_enabled;
	ConfigBool *dribbler_enabled;
};
