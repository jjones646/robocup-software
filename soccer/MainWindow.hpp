// kate: indent-mode cstyle; indent-width 4; tab-width 4; space-indent false;
// vim:ai ts=4 et

#pragma once

#include <QMainWindow>
#include <QTimer>

#include <FieldView.hpp>
#include <configuration/ConfigFileTab.hpp>

#include "Processor.hpp"
#include "ui_MainWindow.h"

class PlayConfigTab;

class MainWindow : public QMainWindow
{
	Q_OBJECT;
	
	public:
		MainWindow(QWidget *parent = 0);
		
		void processor(Processor *value);
		
		Processor *processor()
		{
			return _processor;
		}
		
		SystemState *state()
		{
			return _processor->state();
		}
		
		PlayConfigTab *playConfigTab() const
		{
			return _playConfigTab;
		}
		
		// Deselects all debug layers
		void allDebugOff();
		
		// Selects all debug layers
		void allDebugOn();
		
	private Q_SLOTS:
		void updateViews();
		
		void on_fieldView_robotSelected(int shell);
		void on_actionRawBalls_toggled(bool state);
		void on_actionRawRobots_toggled(bool state);
		void on_actionCoords_toggled(bool state);
		void on_actionTeamYellow_triggered();
		void on_actionTeamBlue_triggered();
		void on_manualID_currentIndexChanged(int value);
		
		// Field side
		void on_actionDefendPlusX_triggered();
		void on_actionDefendMinusX_triggered();
		
		// Field rotation
		void on_action0_triggered();
		void on_action90_triggered();
		void on_action180_triggered();
		void on_action270_triggered();
		
		// Simulator commands
		void on_actionCenterBall_triggered();
		void on_actionStopBall_triggered();
		
		// Debug layers
		void on_debugLayers_itemChanged(QListWidgetItem *item);
		void on_debugLayers_customContextMenuRequested(const QPoint &pos);
		
		// Referee
		void on_externalReferee_toggled(bool value);
		void on_externalReferee_clicked(bool value);
		
		void on_refHalt_clicked();
		void on_refStop_clicked();
		void on_refReady_clicked();
		void on_refForceStart_clicked();
		void on_refKickoffBlue_clicked();
		void on_refKickoffYellow_clicked();
		void on_refFirstHalf_clicked();
		void on_refOvertime1_clicked();
		void on_refHalftime_clicked();
		void on_refOvertime2_clicked();
		void on_refSecondHalf_clicked();
		void on_refPenaltyShootout_clicked();
		void on_refTimeoutBlue_clicked();
		void on_refTimeoutYellow_clicked();
		void on_refTimeoutEnd_clicked();
		void on_refTimeoutCancel_clicked();
		void on_refDirectBlue_clicked();
		void on_refDirectYellow_clicked();
		void on_refIndirectBlue_clicked();
		void on_refIndirectYellow_clicked();
		void on_refPenaltyBlue_clicked();
		void on_refPenaltyYellow_clicked();
		void on_refGoalBlue_clicked();
		void on_refSubtractGoalBlue_clicked();
		void on_refGoalYellow_clicked();
		void on_refSubtractGoalYellow_clicked();
		void on_refYellowCardBlue_clicked();
		void on_refYellowCardYellow_clicked();
		void on_refRedCardBlue_clicked();
		void on_refRedCardYellow_clicked();
		
	private:
		void addTreeData(QTreeWidgetItem *parent, const google::protobuf::Message &msg);
		
		void updateStatus();
		
		void refCommand(char ch);
		
		typedef enum
		{
			Status_OK,
			Status_Warning,
			Status_Fail
		} StatusType;
		
		void status(QString text, StatusType status);
		
		Ui_MainWindow ui;
		
		Processor *_processor;
		
		bool _treeInitialized;
		
		// When true, External Referee is automatically set.
		// This is cleared by manually changing the checkbox or after the
		// first referee packet is seen and the box is automatically checked.
		bool _autoExternalReferee;

		QTimer _updateTimer;
		ConfigFileTab* _configFileTab;
		PlayConfigTab *_playConfigTab;
};
