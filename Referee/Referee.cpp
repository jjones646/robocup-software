#include "Referee.hpp"
#include <math.h>

Referee::Referee() :
	QMainWindow()
{
	setupUi(this);

	/** Initialize game variables */
	Referee::currPeriod=0;
	Referee::currGameState=0;
	Referee::currMilliseconds=0;
	Referee::maxMilliseconds=Referee::_preGameMsecs;

	/** Initialize goals */ 
	Referee::blueGoals=0;
	Referee::yellowGoals=0;

	/** Initialize current time out timers */
	Referee::currBlueTimeOutTimer=0;
	Referee::currYellowTimeOutTimer=0;

	/** Initialize amount of time outs left */
	Referee::blueTimeOuts=4;
	Referee::yellowTimeOuts=4;

	/** connect to timer for sending ref packets out */
	connect(&_txTimer, SIGNAL(timeout()), SLOT(TxSend()));

	/** Connect to the timer that keeps track of the game state. */
	connect(&_gameTimer, SIGNAL(timeout()), SLOT(gameUpdate()));

	/** Connect to the timer that controls blue team's time outs */
	connect(&_yellowTimeOutTimer, SIGNAL(timeout()), SLOT(yellowTimeOutUpdate()));

	/** Connect to the timer that controls yellow team's time outs */
	connect(&_blueTimeOutTimer, SIGNAL(timeout()), SLOT(blueTimeOutUpdate()));

	/** setup transmit timer */
	_txTimer.start(1000/Referee::_hz);

}

Referee::~Referee()
{
	
}

///This is the function that is called when the game timer times out.  This is executed several times per second, based on the value of _hz.
void Referee::TxSend()
{

}

///This function updates the timers for time outs
void Referee::blueTimeOutUpdate()
{
	///Check to see if current time exceeds max time possible in current period.
	if(Referee::currBlueTimeOutTimer >= Referee::_timeOutMsecs) {

		///Stop game timer, set the gamestate to halted and increment the game period.
		_blueTimeOutTimer.stop();
		Referee::startGameTimer();
		///Reset the timer if there are still time outs left.
		if(blueTimeOuts>=1) {
			currBlueTimeOutTimer=0;
			Referee::updateBlueTimeOutRemaining();
			Referee::UpdateBlueTimeOutLabels();
		}
	
	} else {
		///Increment the timer if the game if time is still left.
		Referee::currBlueTimeOutTimer+=100;
		//DEBUG
		//printf("Current milliseconds: %d\n", Referee::currBlueTimeOutTimer);

		///Update our labels.
		Referee::updateBlueTimeOutRemaining();
		Referee::UpdateBlueTimeOutLabels();
	}


}

///This function updats the timers for time outs
void Referee::yellowTimeOutUpdate()
{

}


///This function updates the labels of the game timers when the game is being played.
void Referee::gameUpdate()
{
	///Check to see if current time exceeds max time possible in current period.
	if(Referee::currMilliseconds >= Referee::maxMilliseconds) {

		///Stop game timer, set the gamestate to halted and increment the game period.
		Referee::stopGameTimer();
		Referee::IncrementCurrentPeriod();

	} else {
		///Increment the timer if the game if time is still left.
		Referee::currMilliseconds+=100;
		//DEBUG
		//printf("Current milliseconds: %d\n", Referee::currMilliseconds);

		//Update our labels.
		Referee::updateTimeElapsed();
		Referee::updateTimeRemaining();
	}

}

///Start of blue buttons code
void Referee::on_BlueGoalButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_MinusBlueButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_BlueTimeOutButton_clicked()
{

	///Make sure blue timer is not active
	if(_blueTimeOutTimer.timerId() == -1) {
		///Make sure there are enough time outs, that another time out clock isn't running, and that the game is playing
		if( (blueTimeOuts >= 1) && (_yellowTimeOutTimer.timerId()==-1) && (currGameState==1)) {
			_blueTimeOutTimer.start(100);
			Referee::stopGameTimer();
			Referee::blueTimeOuts--;
			Referee::UpdateBlueTimeOutLabels();
		}
	}
}

void Referee::on_BluePenaltyButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_BlueKickOffButton_clicked()
{

printf("Debug.\n");

}

void Referee:: on_BlueFreeKickButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_BlueIndirectButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_BlueYellowCardButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_BlueRedCardButton_clicked()
{

printf("Debug.\n");

}

///Start of yellow buttons code

void Referee::on_YellowGoalButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_MinusYellowButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_YellowTimeOutButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_YellowPenaltyButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_YellowKickOffButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_YellowFreeKickButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_YellowIndirectButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_YellowYellowCardButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_YellowRedCardButton_clicked()
{

printf("Debug.\n");

}


///Start of game buttons code
void Referee::on_StartTimeButton_clicked()
{
	Referee::startGameTimer();

}

void Referee::on_StopTimeButton_clicked()
{
	Referee::stopGameTimer();

}

void Referee::on_StartGameButton_clicked()
{

printf("Debug.\n");

}

void Referee::on_StopGameButton_clicked()
{

printf("Debug.\n");

}

///This function updates the Time Elapsed button inside the GUI.
void Referee::updateTimeElapsed()
{
	//Create a buffer string for our sprintf
	char buf[256];

	//Convert milliseconds to minutes, seconds and tenths of seconds
	int milliseconds=Referee::currMilliseconds;

	//Find Minutes
	int minutes=floor(milliseconds/(60*1000));
	milliseconds = milliseconds - (minutes*60*1000);

	//Find seconds
	int seconds=floor(milliseconds/(1000));
	milliseconds = milliseconds - (seconds*1000);

	//Find tenths
	int tenths = floor(milliseconds/(100));

	//Format string for use in label
	if(seconds > 10) {
		snprintf(buf, 100, "%d:%d.%d", minutes, seconds, tenths);
	} else {
		snprintf(buf, 100, "%d:0%d.%d", minutes, seconds, tenths);
	}

	//Put string into label
	TimeElapsedText->setText(buf);

	//DEBUG
	//printf("Time Elapsed: %s\n", buf);
		

}

///This function updates the Time Remaining button inside the GUI.
void Referee::updateTimeRemaining()
{
	//Create a buffer string for our sprintf
	char buf[256];
	
	//Convert milliseconds to minutes, seconds and tenths of seconds
	int milliseconds=Referee::maxMilliseconds - Referee::currMilliseconds;

	//Find Minutes
	int minutes=floor(milliseconds/(60*1000));
	milliseconds = milliseconds - (minutes*60*1000);

	//Find seconds
	int seconds=floor(milliseconds/(1000));
	milliseconds = milliseconds - (seconds*1000);

	//Find tenths
	int tenths = floor(milliseconds/(100));
	
	//Format string for use in label.
	if(seconds > 10) {
		snprintf(buf, 100, "%d:%d.%d", minutes, seconds, tenths);
	} else {
		snprintf(buf, 100, "%d:0%d.%d", minutes, seconds, tenths);
	}

	//Put string into label
	TimeRemainingText->setText(buf);

	//DEBUG
	//printf("Time Remaining: %s\n", buf);

}

void Referee::updateBlueTimeOutRemaining()
{
	//Create a buffer string for our sprintf
	char buf[256];
	
	//Convert milliseconds to minutes, seconds and tenths of seconds
	int milliseconds=Referee::_timeOutMsecs - Referee::currBlueTimeOutTimer;

	//Find Minutes
	int minutes=floor(milliseconds/(60*1000));
	milliseconds = milliseconds - (minutes*60*1000);

	//Find seconds
	int seconds=floor(milliseconds/(1000));
	milliseconds = milliseconds - (seconds*1000);

	//Find tenths
	int tenths = floor(milliseconds/(100));
	
	//Format string for use in label.
	if(seconds > 10) {
		snprintf(buf, 100, "%d:%d.%d", minutes, seconds, tenths);
	} else {
		snprintf(buf, 100, "%d:0%d.%d", minutes, seconds, tenths);
	}

	//Put string into label
	TimeRemainingBlueText->setText(buf);

	//DEBUG
	//printf("Time Remaining: %s\n", buf);


}

void Referee::updateYellowTimeOutRemaining()
{

}

void Referee::stopGameTimer()
{
	///The timer has AIDS.  Stop it from reproducing.
	_gameTimer.stop();
	Referee::currGameState=0;
}

void Referee::startGameTimer()
{
	if(_gameTimer.timerId() == -1) {
		//Start the timer if it hasn't been started yet.
		_gameTimer.start(100);
		///MAKE SURE YOU CHANGE THIS!!!!
		currGameState=1;
	} else {
		///Re-start the timer if it has already been started.
		_gameTimer.start();
	}
}

void Referee::incrementBlueGoals()
{

}

void Referee::IncrementYellowGoals()
{

}

void Referee::DecrementBlueGoals()
{

}

void Referee::DecrementYellowGoals()
{

}

void Referee::IncrementCurrentPeriod()
{
	switch(Referee::currPeriod) {
		case 0:
			Referee::currPeriod++;
			Referee::currMilliseconds=0;
			Referee::maxMilliseconds=_firstHalfMsecs;
			Referee::ResetTimeLabels();
			Referee::UpdatePeriodLabels();
			break;
		case 1:
			Referee::currPeriod++;
			Referee::currMilliseconds=0;
			Referee::maxMilliseconds=_halfTimeMsecs;
			Referee::ResetTimeLabels();
			Referee::UpdatePeriodLabels();
			break;
		case 2:
			Referee::currPeriod++;
			Referee::currMilliseconds=0;
			Referee::maxMilliseconds=_secondHalfMsecs;
			Referee::ResetTimeLabels();
			Referee::UpdatePeriodLabels();
			break;
		case 3:
			Referee::currPeriod++;
			Referee::currMilliseconds=0;
			Referee::maxMilliseconds=_firstOvertimeMsecs;
			Referee::ResetTimeLabels();
			Referee::UpdatePeriodLabels();
			break;
		case 4:
			Referee::currPeriod++;
			Referee::currMilliseconds=0;
			Referee::maxMilliseconds=_secondOvertimeMsecs;
			Referee::ResetTimeLabels();
			Referee::UpdatePeriodLabels();
			break;
		case 5:
			Referee::currPeriod=0;
			Referee::currMilliseconds=0;
			Referee::maxMilliseconds=_preGameMsecs;
			Referee::ResetTimeLabels();
			Referee::UpdatePeriodLabels();
			break;
		default:
			Referee::currPeriod=0;
			Referee::currMilliseconds=0;
			Referee::maxMilliseconds=_preGameMsecs;
			Referee::ResetTimeLabels();
			Referee::UpdatePeriodLabels();
			break;
	}
}

void Referee::ResetTimeLabels()
{
	Referee::updateTimeElapsed();
	Referee::updateTimeRemaining();
	Referee::UpdatePeriodLabels();
}

void Referee::UpdatePeriodLabels()
{
	switch(Referee::currPeriod) {
		case 0:
			PeriodText->setText("Pre-Game");
			break;
		case 1:
			PeriodText->setText("First Half");
			break;
		case 2:
			PeriodText->setText("Half Time");
			break;
		case 3:
			PeriodText->setText("Second Half");
			break;
		case 4:
			PeriodText->setText("First Overtime");
			break;
		case 5:
			PeriodText->setText("Pre-Game");
			break;
		default:
			PeriodText->setText("Pre-Game");
			break;
	}
}

void Referee::UpdateBlueTimeOutLabels()
{
	///Update blue time out labels.
	char buf[16];
	snprintf(buf, 8, "%d", blueTimeOuts);
	BlueTimeOutsLeftText->setText(buf);
}

void Referee::UpdateYellowTimeOutLabels()
{

}

