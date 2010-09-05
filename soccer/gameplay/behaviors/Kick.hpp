#pragma once

#include <gameplay/Behavior.hpp>

namespace Gameplay
{
	namespace Behaviors
	{
		class Kick: public SingleRobotBehavior
		{
			public:
				Kick(GameplayModule *gameplay);
				
				virtual bool run();
				
				void restart();
				
				// Kicks at the opponent's goal
				void setTargetGoal();
				
				// Kicks at a robot (pass)
				void setTarget(Robot *r);
				
				// Kicks towards a point
				void setTarget(Geometry2d::Point pt);
			
			private:
				enum State
				{
					State_Approach1,
					State_Approach2,
					State_Aim,
					State_Kick,
					State_Done
				};
				
				State _state;
				float _lastError;
				
				Geometry2d::Segment _target;
		};
	}
}
