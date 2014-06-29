#include "robocup-py.hpp"
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <string>
#include <sstream>

using namespace boost::python;

#include <Geometry2d/Point.hpp>
#include <Geometry2d/Rect.hpp>
#include <Geometry2d/CompositeShape.hpp>
#include <Robot.hpp>
#include <SystemState.hpp>
#include <protobuf/LogFrame.pb.h>


//	this is here so boost can work with std::shared_ptr
template<class T> T * get_pointer( std::shared_ptr<T> const& p) {
	return p.get();
}

std::string Point_repr(Geometry2d::Point *self) {
	std::ostringstream ss;
	ss << "Point(";
	ss << self->x;
	ss << ", ";
	ss << self->y;
	ss << ")";
	
	std::string repr(ss.str());
	return repr;
}

std::string Robot_repr(Robot *self) {
	std::ostringstream ss;
	ss << "<Robot ";
	ss << (self->self() ? "us[" : "them[");
	ss << self->shell();
	ss << "], pos=";
	ss << Point_repr(&(self->pos));
	ss << ">";

	std::string repr(ss.str());
	return repr;
}


void OurRobot_move_to(OurRobot *self, Geometry2d::Point *to) {
	self->move(*to);
}

void OurRobot_set_avoid_ball_radius(OurRobot *self, float radius) {
	self->avoidBallRadius(radius);
}

void OurRobot_approach_opponent(OurRobot *self, unsigned shell_id, bool enable_approach) {
	self->approachOpponent(shell_id, enable_approach);
}

bool Rect_contains_rect(Geometry2d::Rect *self, Geometry2d::Rect *other) {
	return self->contains(*other);
}

bool Rect_contains_point(Geometry2d::Rect *self, Geometry2d::Point *pt) {
	return self->contains(*pt);
}

void Point_rotate(Geometry2d::Point *self, Geometry2d::Point *origin, float angle) {
	self->rotate(*origin, angle);
}

void CompositeShape_add_shape(Geometry2d::CompositeShape *self, Geometry2d::Shape *shape) {
	self->add(std::shared_ptr<Geometry2d::Shape>( shape->clone() ));
}

boost::python::tuple Line_wrap_pt(Geometry2d::Line *self) {
	boost::python::list a;
	for (int i = 0; i < 2; i++) {
		a.append(self->pt[i]);
	}
	return boost::python::tuple(a);
}

//	returns None or a Geometry2d::Point
boost::python::object Line_line_intersection(Geometry2d::Line *self, Geometry2d::Line *other) {
	Geometry2d::Point pt;
	if (self->intersects(*other, &pt)) {
		boost::python::object obj(pt);
		return obj;
	} else {
		//	return None
		return boost::python::object();
	}
};


/**
 * The code in this block wraps up c++ classes and makes them
 * accessible to python in the 'robocup' module.
 */
BOOST_PYTHON_MODULE(robocup)
{
	class_<Geometry2d::Point>("Point", init<float, float>())
		.def(init<const Geometry2d::Point &>())
		.def_readwrite("x", &Geometry2d::Point::x)
		.def_readwrite("y", &Geometry2d::Point::y)
		.def(self - self)
		.def(self + self)
		.def("mag", &Geometry2d::Point::mag)
		.def("magsq", &Geometry2d::Point::magsq)
		.def("__repr__", &Point_repr)
		.def("normalized", &Geometry2d::Point::normalized)
		.def("rotate", &Point_rotate)
		.def(self * float())
		.def(self / float())
		.def("perp_ccw", &Geometry2d::Point::perpCCW)
		.def("perp_cw", &Geometry2d::Point::perpCW)
		.def("angle", &Geometry2d::Point::angle)
		.def("dot", &Geometry2d::Point::dot)
		.def("near_point", &Geometry2d::Point::nearPoint)
	;

	class_<Geometry2d::Line>("Line", init<Geometry2d::Point, Geometry2d::Point>())
		.add_property("pt", Line_wrap_pt)
		.def("delta", &Geometry2d::Line::delta)
		.def("line_intersection", &Line_line_intersection)
	;

	class_<Geometry2d::Segment, bases<Geometry2d::Line> >("Segment", init<Geometry2d::Point, Geometry2d::Point>())
		.def("center", &Geometry2d::Segment::center)
		.def("length", &Geometry2d::Segment::length)
		.def("dist_to", &Geometry2d::Segment::distTo)
		.def("nearest_point", &Geometry2d::Segment::nearestPoint)
	;

	class_<Geometry2d::Shape, boost::noncopyable>("Shape")
	;

	class_<Geometry2d::Rect, bases<Geometry2d::Shape> >("Rect", init<Geometry2d::Point, Geometry2d::Point>())
		.def("contains_rect", &Rect_contains_rect)
		.def("min_x", &Geometry2d::Rect::minx)
		.def("min_y", &Geometry2d::Rect::miny)
		.def("max_x", &Geometry2d::Rect::maxx)
		.def("max_y", &Geometry2d::Rect::maxy)
		.def("near_point", &Geometry2d::Rect::nearPoint)
		.def("intersects_rect", &Geometry2d::Rect::intersects)
		.def("contains_point", &Geometry2d::Rect::containsPoint)
	;

	class_<Geometry2d::Circle, bases<Geometry2d::Shape> >("Circle", init<Geometry2d::Point, float>());

	class_<Geometry2d::CompositeShape, bases<Geometry2d::Shape> >("CompositeShape", init<>())
		.def("clear", &Geometry2d::CompositeShape::clear)
		.def("is_empty", &Geometry2d::CompositeShape::empty)
		.def("size", &Geometry2d::CompositeShape::size)
		.def("add_shape", &CompositeShape_add_shape)
		.def("contains_point", &Geometry2d::CompositeShape::containsPoint)
	;

	class_<GameState>("GameState")
		.def_readonly("our_score", &GameState::ourScore)
		.def_readonly("their_score", &GameState::theirScore)
		.def("is_halted", &GameState::halt)
		.def("is_stopped", &GameState::stopped)
		.def("is_playing", &GameState::playing)
		.def("is_kickoff", &GameState::kickoff)
		.def("is_penalty", &GameState::penalty)
		.def("is_direct", &GameState::direct)
		.def("is_indirect", &GameState::indirect)
		.def("is_our_kickoff", &GameState::ourKickoff)
		.def("is_our_penalty", &GameState::ourPenalty)
		.def("is_our_direct", &GameState::ourDirect)
		.def("is_our_indirect", &GameState::ourIndirect)
		.def("is_our_free_kick", &GameState::ourFreeKick)
		.def("is_their_kickoff", &GameState::theirKickoff)
		.def("is_their_penalty", &GameState::theirPenalty)
		.def("is_their_direct", &GameState::theirDirect)
		.def("is_their_indirect", &GameState::theirIndirect)
		.def("is_their_free_kick", &GameState::theirFreeKick)
		.def("is_setup_restart", &GameState::setupRestart)
		.def("can_kick", &GameState::canKick)
		.def("stay_away_from_ball", &GameState::stayAwayFromBall)
		.def("stay_on_side", &GameState::stayOnSide)
		.def("stay_behind_penalty_line", &GameState::stayBehindPenaltyLine)
	;

	class_<Robot>("Robot", init<int, bool>())
		.def("shell_id", &Robot::shell)
		.def("is_ours", &Robot::self)
		.def_readwrite("pos", &Robot::pos)
		.def_readwrite("vel", &Robot::vel)
		.def_readwrite("angle", &Robot::angle)
		.def_readwrite("angle_vel", &Robot::angleVel)
		.def_readwrite("visible", &Robot::visible)
		.def("__repr__", &Robot_repr)
	;

	class_<OurRobot, OurRobot *, std::shared_ptr<OurRobot>, bases<Robot> >("OurRobot", init<int, SystemState*>())
		.def("move_to", &OurRobot_move_to)
		.def("face", &OurRobot::face)
		.def("set_avoid_ball_radius", &OurRobot_set_avoid_ball_radius)
		.def("avoid_all_teammates", &OurRobot::avoidAllTeammates)
		.def("add_text", &OurRobot::addText)
		.def("approach_opponent", &OurRobot_approach_opponent)
	;

	class_<OpponentRobot, OpponentRobot *, std::shared_ptr<OpponentRobot>, bases<Robot> >("OpponentRobot", init<int>());

	class_<Ball, std::shared_ptr<Ball> >("Ball", init<>())
		.def_readonly("pos", &Ball::pos)
		.def_readonly("vel", &Ball::vel)
		.def_readonly("valid", &Ball::valid)
	;

	class_<std::vector<OurRobot *> >("vector_OurRobot")
		.def(vector_indexing_suite<std::vector<OurRobot *> >())
	;

	class_<std::vector<OpponentRobot *> >("vector_OpponentRobot")
		.def(vector_indexing_suite<std::vector<OpponentRobot *> >())
	;

	class_<SystemState>("SystemState")
		.def_readonly("our_robots", &SystemState::self)
		.def_readonly("their_robots", &SystemState::opp)
		.def_readonly("ball", &SystemState::ball)
		.def_readonly("game_state", &SystemState::gameState)
		.def_readonly("timestamp", &SystemState::timestamp)

		//	debug drawing methods
		.def("draw_circle", &SystemState::drawCircle)
		.def("draw_path", &SystemState::drawPath)
		.def("draw_text", &SystemState::drawText)
		.def("draw_shape", &SystemState::drawShape)
	;
}
