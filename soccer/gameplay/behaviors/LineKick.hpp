#pragma once

#include <gameplay/Behavior.hpp>

namespace Gameplay
{
	namespace Behaviors
	{
		class LineKick: public SingleRobotBehavior
		{
			public:
				static void createConfiguration(Configuration *cfg);

				LineKick(GameplayModule *gameplay);
				
				virtual bool run();
				
				bool done() const
				{
					return _state == State_Done;
				}
				
				void restart();
				
				Geometry2d::Point target;
				
				/** kick parameter flags */
				bool use_chipper;
				uint8_t kick_power;

			private:
				enum
				{
					State_Setup,
					State_Charge,
					State_Done
				} _state;

				static ConfigDouble *_drive_around_dist;
				static ConfigDouble *_setup_to_charge_thresh;
				static ConfigDouble *_escape_charge_thresh;
				static ConfigDouble *_setup_ball_avoid;
				static ConfigDouble *_accel_bias;
				static ConfigDouble *_facing_thresh;
		};
	}
}
