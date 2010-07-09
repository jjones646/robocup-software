// kate: indent-mode cstyle; indent-width 4; tab-width 4; space-indent false;
// vim:ai ts=4 et

#include "Processor.hpp"
#include <Processor.moc>

#include <QMutexLocker>

#include <netdb.h>
#include <arpa/inet.h>
#include <Network/Network.hpp>

#include <Vision.hpp>
#include <Constants.hpp>
#include <Utils.hpp>

#include <modeling/WorldModel.hpp>
#include <gameplay/GameplayModule.hpp>

#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#include <protobuf/messages_robocup_ssl_detection.pb.h>
#include <protobuf/messages_robocup_ssl_wrapper.pb.h>
#include <protobuf/messages_robocup_ssl_geometry.pb.h>
#include <protobuf/RadioTx.pb.h>
#include <protobuf/RadioRx.pb.h>

using namespace std;
using namespace boost;

Processor::Processor(Team t, QString filename, QObject *mainWindow) :
	_running(true),
	_team(t),
// 	_sender(Network::Address, Network::addTeamOffset(_team, Network::RadioTx)),
	_config(new ConfigFile(filename))
{
	_mainWindow = mainWindow;
	vision_addr = "224.5.23.2";
	_reverseId = 0;
	_framePeriod = 1000000 / 60;
	_lastFrameTime = 0;
	
	Geometry2d::Point trans;
	if (_team == Blue)
	{
		_teamAngle = -90;
		trans = Geometry2d::Point(0, Constants::Field::Length / 2.0f);
	} else {
		// Assume yellow
		_teamAngle = 90;
		trans = Geometry2d::Point(0, Constants::Field::Length / 2.0f);
	}

	//transformations from world to team space
	_teamTrans = Geometry2d::TransformMatrix::translate(trans);
	_teamTrans *= Geometry2d::TransformMatrix::rotate(_teamAngle);

	_flipField = false;

	//set the team
	_state.team = _team;

	// Start not controlling any robot
	_state.manualID = -1;
	
	//initially no camera does the triggering
	_trigger = false;

	_joystick = make_shared<JoystickInput>("/dev/input/robocupPad", &_state);
	
	_state.autonomous = !_joystick->valid();

	QMetaObject::connectSlotsByName(this);

	try
	{
		_config->load();
	}
	catch (std::runtime_error& re)
	{
		printf("Config Load Error: %s\n", re.what());
	}

	_visionSocket.bind(10002);
	struct ip_mreqn mreq;
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr.s_addr = inet_addr(vision_addr.toAscii());
	setsockopt(_visionSocket.socketDescriptor(), IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

	_radioSocket.bind(Network::addTeamOffset(_team, Network::RadioTx));
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr.s_addr = inet_addr(Network::Address);
	setsockopt(_radioSocket.socketDescriptor(), IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

	//setup the modules
/*	_modelingModule = make_shared<Modeling::WorldModel>(&_state, _config->worldModel);
	_stateIDModule = make_shared<StateIdentification::StateIDModule>(&_state);
	_motionModule = make_shared<Motion::MotionModule>(&_state, _config->motionModule);
	_refereeModule = make_shared<RefereeModule>(&_state);
	_gameplayModule = make_shared<Gameplay::GameplayModule>(&_state, _config->motionModule);*/
}

Processor::~Processor()
{
	_running = false;
	wait();
}

void Processor::run()
{
	//setup receiver of packets for vision and radio
/*	Network::PacketReceiver receiver;
	receiver.addType(vision_addr.toAscii(), 10002, this,
			&Processor::visionHandler);
	receiver.addType(Network::Address,
			Network::addTeamOffset(_team, Network::RadioRx),
			this, &Processor::radioHandler);*/
// 	RefereeModule* raw = dynamic_cast<RefereeModule*>(_refereeModule.get());
// 	receiver.addType(RefereeAddress, RefereePort, 
// 			raw, &RefereeModule::packet);

	uint64_t lastTime = Utils::timestamp();
	while (_running)
	{
		// Read all pending data from the gamepad
		while (_joystick->poll())
		{
		}
		
		while (_visionSocket.hasPendingDatagrams())
		{
			vector<uint8_t> buf;
			buf.resize(_visionSocket.pendingDatagramSize());
			_visionSocket.readDatagram((char *)&buf[0], buf.size());
			visionPacket(&buf);
		}
		
#if 0
		// Clear radio commands
		for (int r = 0; r < 5; ++r)
		{
			_state.self[r].radioTx = Packet::RadioTx::Robot();
		}

		if (_modelingModule)
		{
			_modelingModule->run();
		}
		
		for (int r = 0; r < 5; ++r)
		{
			if (_state.self[r].valid)
			{
				// Mixed play IDs
				if (_state.self[r].shell == 4 || _state.self[r].shell == 11)
				{
					_gameplayModule->self[r]->exclude = true;
				}
				
				ConfigFile::shared_robot rcfg = _config->robot(_state.self[r].shell);
				
				if (rcfg)
				{
					_state.self[r].config = *rcfg;

					// set the config information
					switch (rcfg->rev) {
					case ConfigFile::rev2008:
						_state.self[r].rev =  Packet::LogFrame::Robot::rev2008;
						break;
					case ConfigFile::rev2010:
						_state.self[r].rev =  Packet::LogFrame::Robot::rev2010;
					}
				}
			}
		}
		
		if (_refereeModule)
		{
			_refereeModule->run();
		}
		
		if (_stateIDModule)
		{
			_stateIDModule->run();
		}

		if (_gameplayModule)
		{
			_gameplayModule->run();
		}

		if (_motionModule)
		{
			_motionModule->run();
		}
#endif

		captureState();
		
		// Send motion commands to the robots
		sendRadioData();

		uint64_t curTime = Utils::timestamp();
		int _lastFrameTime = curTime - lastTime;
		if (_lastFrameTime < _framePeriod)
		{
			usleep(_framePeriod - _lastFrameTime);
		} else {
			printf("Took too long: %d us\n", _lastFrameTime);
		}
	}
}

void Processor::captureState()
{
	QMutexLocker locker(&_frameMutex);
	_lastFrame = _state;
	qApp->postEvent(_mainWindow, new QEvent(QEvent::User));
}

Packet::LogFrame Processor::lastFrame()
{
	QMutexLocker locker(&_frameMutex);
	return _lastFrame;
}

void Processor::sendRadioData()
{
	Packet::RadioTx tx;

	tx.set_reverse_board_id(_state.self[_reverseId].shell);
	_reverseId = (_reverseId + 1) % Constants::Robots_Per_Team;
	
	for (int i = 0; i < Constants::Robots_Per_Team; ++i)
	{
		*tx.add_robots() = _state.self[i].radioTx;
	}

	bool halt;
	if (!_state.autonomous)
	{
		// Manual
		halt = false;
	} else {
		// Auto
		halt = _state.gameState.halt();
	}

#if 0
	if (halt)
	{
		// Force all motor speeds to zero
		for (int r = 0; r < 5; ++r)
		{
			for (int m = 0; m < 4; ++m)
			{
				tx.robots[r].motors[m] = 0;
			}
		}
	}
#endif

// 	_sender.send(tx);
}

void Processor::visionPacket(const std::vector<uint8_t>* buf)
{
	SSL_WrapperPacket wrapper;
	if (!wrapper.ParseFromArray(&buf->at(0), buf->size()))
	{
		printf("Bad vision packet\n");
		return;
	}
	
	if (!wrapper.has_detection())
	{
		// Geometry only - we don't care
		return;
	}
	
	const SSL_DetectionFrame &detection = wrapper.detection();
	
	Packet::Vision visionPacket;
	visionPacket.camera = detection.camera_id();
	visionPacket.timestamp = (uint64_t)(detection.t_capture() * 1.0e6);
	
	BOOST_FOREACH(const SSL_DetectionRobot& robot, detection.robots_yellow())
	{
		if (robot.confidence() == 0)
		{
			continue;
		}
		
		Packet::Vision::Robot r;
		r.pos.x = robot.x()/1000.0;
		r.pos.y = robot.y()/1000.0;
		r.angle = robot.orientation() * RadiansToDegrees;
		r.shell = robot.robot_id();
		
		visionPacket.yellow.push_back(r);
	}
	
	BOOST_FOREACH(const SSL_DetectionRobot& robot, detection.robots_blue())
	{
		if (robot.confidence() == 0)
		{
			continue;
		}
		
		Packet::Vision::Robot r;
		r.pos.x = robot.x()/1000.0;
		r.pos.y = robot.y()/1000.0;
		r.angle = robot.orientation() * RadiansToDegrees;
		r.shell = robot.robot_id();
		
		visionPacket.blue.push_back(r);
	}
	
	BOOST_FOREACH(const SSL_DetectionBall& ball, detection.balls())
	{
		if (ball.confidence() == 0)
		{
			continue;
		}
		
		Packet::Vision::Ball b;
		b.pos.x = ball.x()/1000.0;
		b.pos.y = ball.y()/1000.0;
		visionPacket.balls.push_back(b);
	}

	//populate the state
	if (visionPacket.camera >= _state.rawVision.size())
	{
		_state.rawVision.resize(visionPacket.camera + 1);
	}
	_state.rawVision[visionPacket.camera] = visionPacket;

	//convert last frame to teamspace
	toTeamSpace(_state.rawVision[visionPacket.camera]);

	if (visionPacket.camera == 0)
	{
		_state.timestamp = visionPacket.timestamp;
		_trigger = true;
	}
}

void Processor::radioHandler(const Packet::RadioRx* packet)
{
#if 0
	//received radio packets
	for (unsigned int i=0 ; i<5 ; ++i)
	{
		if (_state.self[i].shell == packet->board_id)
		{
			_state.self[i].radioRx = *packet;
			break;
		}
	}
#endif
}

void Processor::toTeamSpace(Packet::Vision& vision)
{
	//translates raw vision into team space
	//means modeling doesn't need to do it
	for (unsigned int i = 0; i < vision.blue.size(); ++i)
	{
		Packet::Vision::Robot& r = vision.blue[i];

		if (_flipField)
		{
			r.pos *= -1;
			r.angle = Utils::fixAngleDegrees(r.angle + 180);
		}

		r.pos = _teamTrans * r.pos;
		r.angle = Utils::fixAngleDegrees(_teamAngle + r.angle);
	}

	for (unsigned int i = 0; i < vision.yellow.size(); ++i)
	{
		Packet::Vision::Robot& r = vision.yellow[i];

		if (_flipField)
		{
			r.pos *= -1;
			r.angle = Utils::fixAngleDegrees(r.angle + 180);
		}

		r.pos = _teamTrans * r.pos;
		r.angle = Utils::fixAngleDegrees(_teamAngle + r.angle);
	}

	for (unsigned int i = 0; i < vision.balls.size(); ++i)
	{
		Packet::Vision::Ball& b = vision.balls[i];

		if (_flipField)
		{
			b.pos *= -1;
		}

		b.pos = _teamTrans * b.pos;
	}
}

void Processor::flipField(bool flip)
{
	_flipField = flip;
}
