import single_robot_behavior
import behavior
import robocup
import constants
import main
import enum


# PassReceive accepts a receive_point as a parameter and gets setup there to catch the ball
# It transitions to the 'aligned' state once it's there within its error thresholds and is steady
# Set its 'ball_kicked' property to True to tell it to dynamically update its position based on where
# the ball is moving and attempt to catch it
# It will move to the 'completed' state if it catches the ball, otherwise it will go to 'failed'
class PassReceive(single_robot_behavior.SingleRobotBehavior):

    # max difference between where we should be facing and where we are facing (in radians)
    FaceAngleErrorThreshold = 8 * constants.DegreesToRadians

    # how much we're allowed to be off in the direction of the pass line
    PositionYErrorThreshold = 0.06

    # how much we're allowed to be off side-to-side from the pass line
    PositionXErrorThreshold = 0.02

    DribbleSpeed = 50

    # we have to be going slower than this to be considered 'steady'
    SteadyMaxVel = 0.02
    SteadyMaxAngleVel = 2   # degrees / second



    class State(enum.Enum):
        aligning = 1    # we're aligning with the planned receive point
        aligned = 2     # being in this state signals that we're ready for the kicker to kick
        receiving = 2   # the ball's been kicked and we're adjusting based on where the ball's moving


    def __init__(self):
        super().__init__(continuous=False)

        for state in PassReceive.State:
            self.add_state(state, behavior.Behavior.State.running)


        self.add_transition(behavior.Behavior.State.start,
            PassReceive.State.aligning,
            lambda: True,
            'immediately')

        self.add_transition(PassReceive.State.aligning,
            PassReceive.State.aligned,
            lambda: self.errors_below_thresholds() and self.is_steady(),
            'steady and in position to receive')

        self.add_transition(PassReceive.State.aligned,
            PassReceive.State.aligning,
            lambda: not self.errors_below_thresholds(),
            'not in receive position')

        for state in [PassReceive.State.aligning, PassReceive.State.aligned]:
            self.add_transition(state,
                PassReceive.State.receiving,
                lambda: self.ball_kicked,
                'ball kicked')

        self.add_transition(PassReceive.State.receiving,
            behavior.Behavior.State.completed,
            lambda: self.has_ball(),
            'ball received!')

        self.add_transition(PassReceive.State.receiving,
            behavior.Behavior.State.failed,
            lambda: False, # FIXME: see if the ball bounced off of us or went past us
            'ball missed :(')



    # set this to True to let the receiver know that the pass has started and the ball's in motion
    # Default: False
    @property
    def ball_kicked(self):
        return self._ball_kicked
    @ball_kicked.setter
    def ball_kicked(self, value):
        self._ball_kicked = value
    

    # The point that the receiver should expect the ball to hit it's mouth
    # Default: None
    @property
    def receive_point(self):
        return self._receive_point
    @receive_point.setter
    def receive_point(self, value):
        self._receive_point = value


    # returns True if we're facing the right direction and in the right position and steady
    def errors_below_thresholds(self):
        if self.receive_point == None:
            return False

        # vector pointing down the pass line toward the kicker
        pass_dir = (self._pass_line.get_pt(0) - self._pass_line.get_pt(1)).normalized()

        pos_error = self.robot.pos - self._target_pos
        x_error = pos_error.dot(pos_error.perp_ccw())
        y_error = pos_error.dot(pos_error)

        return (self._angle_error < PassReceive.FaceAngleErrorThreshold
            and x_error < PassReceive.PositionXErrorThreshold
            and y_error < PassReceive.PositionYErrorThreshold)


    def is_steady(self):
        return (self.robot.vel.mag() < PassReceive.SteadyMaxVel
            and self.robot.angle_vel < PassReceive.SteadyMaxAngleVel)


    # calculates:
    # self._pass_line - the line from the ball along where we think we're going
    # self._target_pos - where the bot should be
    # self._angle_error - difference in where we're facing and where we want to face (in radians)
    def recalculate(self):
        # can't do squat if we don't know what we're supposed to do
        if self.receive_point == None:
            return


        ball = main.ball()

        if self.ball_kicked:
            # when the ball's in motion, the line is based on the ball's velocity
            self._pass_line = robocup.Line(ball.pos, ball.pos + ball.vel)
        else:
            # if the ball hasn't been kicked yet, we assume it's going to go through the receive point
            self._pass_line = robocup.Line(ball.pos, self.receive_point)

        target_angle_rad = (ball.pos - self.robot.pos).angle()
        angle_rad = self.robot.angle * constants.DegreesToRadians
        self._angle_error = target_angle_rad - angle_rad

        # FIXME: target pos is wrong after the ball has been kicked
        #        need to calculate an ACTUAL receive point
        pass_line_dir = (self._pass_line.get_pt(1) - self._pass_line.get_pt(0)).normalized()
        self._target_pos = self.receive_point + pass_line_dir * constants.Robot.Radius



    def on_exit_start(self):
        # reset
        self.ball_kicked = False


    def execute_running(self):
        self.recalculate()

        self.robot.face(main.ball().pos)
        self.robot.move_to(self._target_pos)


    def execute_receiving(self):
        self.set_dribble_speed(PassReceive.DribbleSpeed)
