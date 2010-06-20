#include "ClearBall.hpp"
#include <boost/foreach.hpp>

using namespace std;


Gameplay::Plays::ClearBall::ClearBall(GameplayModule *gameplay):
	Play(gameplay, 4),
	_kicker(gameplay),
	_fullback1(gameplay, Behaviors::Fullback::Left),
	_kicker1(gameplay),
	_kicker2(gameplay),
	_oppDistMin(1.5),
	_selfDistMax(0.75)
{
	// may want to comment this out
	_kicker.aimType(Behaviors::Kick::ONETOUCH);
}

bool Gameplay::Plays::ClearBall::applicable()
{
	bool refApplicable =_gameplay->state()->gameState.playing();
	bool gameplayApplicable = true; //_gameplay->state()->stateID.posession == Packet::LogFrame::DEFENSE; // is this correct?

	return refApplicable && gameplayApplicable;
}

bool Gameplay::Plays::ClearBall::assign(set<Robot *> &available)
{
	if(available.size() <= 0){return false;}

	float selfBallDistMin = 999;
	Geometry2d::Point ballPos = _gameplay->state()->ball.pos;
	Robot* closest;

	// calculate closest (non-goalie) self robot to ball
	Robot* goalie = (_gameplay->goalie() ? _gameplay->goalie()->robot() : (Robot*)0);
	BOOST_FOREACH(Robot *r, _gameplay->self){
		float ballDist = ballPos.distTo(r->pos());
		if(r!=goalie && selfBallDistMin > ballDist){
			selfBallDistMin = ballDist;
			closest = r;
		}
	}

	if(!closest){return false;}

	_kicker.assignOne(closest);
	available.erase(closest);
	if(!_fullback1.assign(available)){return false;};
	if(!_kicker1.assign(available)){return false;};
	if(!_kicker2.assign(available)){return false;};

	_robots.insert(_kicker.robot());
	_robots.insert(_fullback1.robot());
	_robots.insert(_kicker1.robot());
	_robots.insert(_kicker2.robot());

	return _robots.size() >= _minRobots;
}

bool Gameplay::Plays::ClearBall::run()
{
	// check if the robot is in done state
	if (_kicker.getState() == Gameplay::Behaviors::Kick::Done)
		return false;

	// run the kick play
	_kicker.run();
	return true;
}

float Gameplay::Plays::ClearBall::score(Robot *r)
{
	cout << "dist is: " << _gameplay->state()->ball.pos.distTo(r->pos()) << std::endl;
	return _gameplay->state()->ball.pos.distTo(r->pos());
}

float Gameplay::Plays::ClearBall::score()
{
	float selfBallDistMin = 999;
	float oppBallDistMin = 999;
	Geometry2d::Point ballPos = _gameplay->state()->ball.pos;

	// calculate closest (non-goalie) self robot to ball
	Robot* goalie = (_gameplay->goalie() ? _gameplay->goalie()->robot() : (Robot*)0);
	BOOST_FOREACH(Robot *r, _gameplay->self){
		float ballDist = ballPos.distTo(r->pos());
		if(r!=goalie && selfBallDistMin > ballDist){
			selfBallDistMin = ballDist;
		}
	}

	// calculate closest opp to ball
	BOOST_FOREACH(Robot *r, _gameplay->opp){
		float ballDist = ballPos.distTo(r->pos());
		if(oppBallDistMin > ballDist){
			oppBallDistMin = ballDist;
		}
	}

	if(selfBallDistMin < _selfDistMax && oppBallDistMin > _oppDistMin){
		return 0.0;
	}else{
		return 999;
	}
}
