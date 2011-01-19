#include "Kick.hpp"

#include <stdio.h>
#include <algorithm>

using namespace std;

//Smallest segment that the robot will shoot at
const float Min_Segment = 1.5 * Ball_Diameter; //Tuning Required (Anthony)

//Speed at which the ball is effectively not moving
const float ballVelThreshold = 0.009;   //Tuning Required (Anthony)

//Position behind the ball by an amount where the robot will have space to align itself 
//without hit the ball
const float yOffset = 3 * Robot_Radius; //Tuning Required (Anthony) 

const float xOffset = 0 * Robot_Radius; //Tuning Required (Anthony) 
                                //I don't think there has to be a horizontal
                                //offset

//There seems to be a problem with the dribbler obtaining and keeping the ball
//I changed the code so that the robot drives to a point 3 robot radii behind
//the ball (only for stationary balls thus far have to add prediction for moving
//balls) and then tries to drive straight at the ball to avoid the issue where
//pivoting knocks the ball out of the way but the robot still has trouble
//obtaining the ball even though it hits it dead on with the dribbler 
//I tested driving the robot directly forward as little as 2 inches with the dribbler running
//the whole time and a stationary ball and the robot still lost the ball when the dribbler made contact.
// - Anthony Gendreau


Gameplay::Behaviors::Kick::Kick(GameplayModule *gameplay):
    SingleRobotBehavior(gameplay)
{
	setTargetGoal();
 
        //Whether or not the shot is feasible
        hasShot = false;
        //The segment of the best shot 
        _shotSegment = Geometry2d::Segment(Geometry2d::Point(0,0), Geometry2d::Point(0,0));
	restart();
}

bool Gameplay::Behaviors::Kick::run()
{
	if (!robot || !robot->visible)
	{
		return false;
	}

        //Only end the behavior is the ball can't be seen and the robot doesn't have it
        //This prevents the behavior from stopping when the robot blocks the ball
        if(!ball().valid && !robot->hasBall)
        {
            return false;
        }
     
        //If the ball is being blocked set its location to the location of the robot 
        //Accounting for the direction the robot is facing
        Geometry2d::Point ballPos;

        if(!ball().valid)
        {
           float x = Robot_Radius * sin(robot->angle * DegreesToRadians);
           float y = Robot_Radius * cos(robot->angle * DegreesToRadians);
           Geometry2d::Point pt = Geometry2d::Point(x, y);
           ballPos = robot->pos + pt; 
        }
        else
        {
            ballPos = ball().pos;
        }

        //Get the best ublocked area in the target to aim at
        WindowEvaluator e = WindowEvaluator(state());
        e.run(ballPos, _target);
        Window *w = e.best;
        
        //The target to use
        Geometry2d::Segment target;

        //Prevents the segfault from using a non existent window
        if(w != NULL)
        {
            if(w->segment.length() < Min_Segment)
            {
                hasShot = false;
            }
            else
            {
                hasShot = true;
            }

            target = w->segment;
            _shotSegment = target;
        }
        else
        {
            hasShot = false;
            target = _target;

            //There is not shot therefore set the best segment to a single point
            _shotSegment = Geometry2d::Segment(Geometry2d::Point(0,0), Geometry2d::Point(0,0));
        }

	// Some calculations depend on the order of the target endpoints.
	// Ensure that t0 x t1 > 0.
	// We have to do this each frame since the ball may move to the other side of the target.
	//FIXME - Actually, doesn't that mean the kick/pass is done?
	//FIXME - What about kicking towards a point?  cross product is zero...
	if ((target.pt[0] - ballPos).cross(target.pt[1] - ballPos) < 0)
	{
		swap(target.pt[0], target.pt[1]);
	}

       
//Find the point to use the the first approach method  
        //The targetPoint Always behind the ball
        //Eventually this should take into account where the robot starts in relation to the ball 
        Geometry2d::Point targetPoint;

        //Create the targetPoint by choosing a point behind the ball
        //Account for a moving ball and guess at where the ball will end up
        Geometry2d::Point ballVel = ball().vel;

        //Ball is effectively stationary
        if((fabs(ballVel.x) <= ballVelThreshold) &&
                fabs((ballVel.y) <= ballVelThreshold))
        {
            targetPoint.x = ballPos.x - xOffset; 
            targetPoint.y = ballPos.y - yOffset;
        }
        else //Ball is moving (I don't know how to handle this case yet)
        {
            targetPoint = ballPos;
        }

//Information about the Ball location relative to the robot used in multiple states
        Geometry2d::Point targetCenter = target.center();
			
        // Vector from ball to center of target
	Geometry2d::Point toTarget = targetCenter - ballPos;
                        
        // Robot position relative to the ball
	Geometry2d::Point relPos = robot->pos - ballPos;

        //The Point to compute with
        Geometry2d::Point point = target.pt[0];
			
	//Behind the ball: move to the nearest line containing the ball and a target endpoint.
	//the robot is behind the ball, while the target vectors all point in *front* of the ball.
	if (toTarget.cross(relPos) < 0)
	{
		// Above the center line: nearest endpoint-line includes target.pt[1]
		point = target.pt[1];
      	}

	// State transitions
	switch (_state)
	{
		case State_Approach1:
                {
			bool nearTarget = robot->pos.nearPoint(targetPoint, 0.25);
                      
                        robot->addText(QString("Near Target %1").arg(nearTarget));
                        robot->addText(QString("X Pos %1 Target X %2").arg(robot->pos.x).arg(targetPoint.x));
                        robot->addText(QString("Y Pos %1 Target Y %2").arg(robot->pos.y).arg(targetPoint.y));
                        
                        //Change States when the robot is near enough to the targetPoint
                        if (nearTarget)
			{
				_state = State_Face;
                        }
			break;
                }
		
                case State_Face:
                {

                        Geometry2d::Point b = (point - targetPoint + robot->pos).normalized();
                        float angleError = b.dot(Geometry2d::Point::direction(robot->angle * DegreesToRadians));
                        
			bool nearTarget = robot->pos.nearPoint(targetPoint, 0.25);
                        
                        robot->addText(QString("Angle %1 Threshold %2").arg(angleError).arg(cos(15 * DegreesToRadians)));
                        robot->addText(QString("Near Target %1").arg(nearTarget));
                        robot->addText(QString("X Pos %1 Target X %2").arg(robot->pos.x).arg(targetPoint.x));
                        robot->addText(QString("Y Pos %1 Target Y %2").arg(robot->pos.y).arg(targetPoint.y));
                    
                        //Change States when the robot is facing the right direction acquire the ball
                        //angleError is greater than because cos(0) is 1 which is perfect 
                        if(angleError > cos(15 * DegreesToRadians))
                        {
                            _state = State_Approach2;
                        //    _state = State_Done; //For Debugging Purposes
                        }
                        break;
                }

		case State_Approach2:
                {
                        if ((robot->hasBall || robot->pos.nearPoint(ballPos, .02)) && robot->charged())
			{
				robot->addText("Aim");
				_state = State_Aim;
				_lastError = INFINITY;
			}
                        
			bool nearTarget = robot->pos.nearPoint(targetPoint, 0.30);

                        //Go back to state one if needed
                        if(!nearTarget)
                        {
                            _state = State_Approach1;
                        }
                       
			break;
                }

		case State_Aim:
                        if ((!robot->hasBall && !robot->pos.nearPoint(ballPos, .02)) || !robot->charged())
			{
				_state = State_Approach2;
			}
                        else 
                        {
				state()->drawLine(ballPos, target.pt[0], Qt::red);
				state()->drawLine(ballPos, target.pt[1], Qt::white);
				
				Geometry2d::Point rd = Geometry2d::Point::direction(robot->angle * DegreesToRadians);

				_kickSegment = Geometry2d::Segment(robot->pos, robot->pos + rd * Field_Length);
				state()->drawLine(robot->pos, target.center(), Qt::gray);
				float error = acos(rd.dot((target.center() - robot->pos).normalized())) * RadiansToDegrees;
			
                                //The distance between the trajectory and the target center
                                float distOff = _kickSegment.distTo(target.center());
                                
                                //The width of half the target 
                                float width = target.pt[0].distTo(target.center());

                                robot->addText(QString("Aim %1").arg(error));
				
				if (!isinf(_lastError) )
				{
					//FIXME - Depends on t0 x t1 > 0
					bool inT0 = (target.pt[0] - ballPos).cross(rd) > 0;
					bool inT1 = rd.cross(target.pt[1] - ballPos) > 0;
					robot->addText(QString("in %1 %2").arg(inT0).arg(inT1));

                                        //How close to the center line printout
                                        robot->addText(QString("Width %1 Distance %2").arg(width).arg(distOff));					
                                        
                                        //If the tarjectory is within the target bounds
                                        if (inT0 && inT1)
                                        {
                                            //Shoot if the shot is getting worse or the shot is
                                            //very good (within half of the width of half the window) (Tuning requiredek;

                                            if((error > _lastError) || (distOff < (width * .5)))
		                            {
						// Past the best position
						_state = State_Kick;
					    }
                                        }
				}
				_lastError = error;
			}
			break;
		
		case State_Kick:

			if (!robot->charged())
			{
				_state = State_Done;
			}
                        
                        //If the robot loses the ball whilst trying to kick before the kick is done
                        //go back to approach1 to reacquire the ball
                        else if(!robot->hasBall)
                        {
                            _state = State_Approach1;
                        }
			break;
		
		case State_Done:
			break;
	}
	
	switch (_state)
	{
		case State_Approach1:
                {
                        robot->addText("Approach1");
			
                        //Move to the appropriate point
                        robot->move(targetPoint + (targetPoint - point).normalized() * (Robot_Radius + .07));
                        
		        state()->drawLine(ballPos, point, Qt::red);

                        state()->drawLine(robot->pos, Geometry2d::Point::direction(robot->angle * DegreesToRadians) + robot->pos, Qt::gray);
			
			robot->avoidBall = true;
			break;
                }

                case State_Face:
                {
                        robot->addText("Face");
                        
                        MotionCmd::PivotType dir = (point - ballPos).cross(relPos) > 0 ? MotionCmd::CW : MotionCmd::CCW;
			
                        if (toTarget.cross(relPos) < 0)
			{
				dir = (point - ballPos).cross(relPos) > 0 ? MotionCmd::CCW : MotionCmd::CW;
	               	}
                        
                        robot->pivot(point - ballPos + robot->pos, dir);
                        robot->dribble(127);

                        state()->drawLine(ballPos, point, Qt::red);

                        state()->drawLine(robot->pos, Geometry2d::Point::direction(robot->angle * DegreesToRadians) + robot->pos, Qt::gray);
                     

			robot->avoidBall = true;
                        break;
                }

		case State_Approach2:
			robot->addText("Approach2");
                        robot->setVScale(.5);
                        robot->setWScale(.25);
                        robot->avoidBall = false;
                        robot->move(ballPos);

                        //Should this face be ballPos or the quanity found in Approach1?
			//robot->face(ballPos);
			robot->dribble(127);
			break;
			
		case State_Aim:
		{
			Geometry2d::Point targetCenter = target.center();
			
			// Vector from ball to center of target
			Geometry2d::Point toTarget = targetCenter - ballPos;
			
			// Robot position relative to the ball
			Geometry2d::Point relPos = robot->pos - ballPos;
			
			// True if the robot is in front of the ball
			bool inFrontOfBall = toTarget.perpCCW().cross(relPos) > 0;
			
			MotionCmd::PivotType dir;
			if (inFrontOfBall)
			{
				// Move behind the ball
				dir = (toTarget.cross(relPos) > 0) ? MotionCmd::CCW : MotionCmd::CW;
			} else {
				// Behind the ball: move to the nearest line containing the ball and a target endpoint.
				// Note that the robot is behind the ball, while the target vectors all point in *front* of the ball.
				//FIXME - These assume t0 x t1 > 0.  Enforce this here or above.
				if (toTarget.cross(relPos) > 0)
				{
					// Below the center line: nearest endpoint-line includes target.pt[0]
		        		dir = (target.pt[0] - ballPos).cross(relPos) > 0 ? MotionCmd::CW : MotionCmd::CCW;
				} else {
					// Above the center line: nearest endpoint-line includes target.pt[1]
					dir = (target.pt[1] - ballPos).cross(relPos) > 0 ? MotionCmd::CCW : MotionCmd::CW;
				}
			}
			
			robot->dribble(127);

                        //Robot has trouble handling a moving ball when trying to pivot (I don't know how to fix this)
			robot->pivot(ballPos, dir);
			state()->drawLine(_kickSegment);
			break;
		}
			
		case State_Kick:
			robot->addText("Kick");
                        if(hasShot)
                        {
                            robot->move(ballPos);
		            robot->face(ballPos);
			    robot->dribble(127);
			    robot->kick(255);
                        }
			state()->drawLine(_kickSegment);
                        break;
		
		case State_Done:
			robot->addText("Done");
			state()->drawLine(_kickSegment);
			break;
	}
	
	return _state != State_Done;
}

void Gameplay::Behaviors::Kick::restart()
{
	_state = State_Approach1;
}

void Gameplay::Behaviors::Kick::setTargetGoal()
{
        //Set the target to be a little inside of the goal posts to prevent noise errors from 
        //causing a post shot
	setTarget(Geometry2d::Segment(
		Geometry2d::Point((Field_GoalWidth / 2 - Ball_Diameter), Field_Length),
		Geometry2d::Point((-Field_GoalWidth / 2 + Ball_Diameter), Field_Length)));
}

void Gameplay::Behaviors::Kick::setTarget(const Geometry2d::Segment& seg)
{
	_target = seg;
}
