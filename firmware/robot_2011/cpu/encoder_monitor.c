// Each 2011 drive module has an encoder on the motor and hall effect sensors in the motor.
// These both indicate speed but with different resolutions.
// If the hall effect sensors fail, the motor either will not drive or will stall.
// If the encoder fails or the encoders are not plugged in to the right connector,
// the speed controller will drive the motor at an inappropriate speed.
//
// We compare the speeds reported by the hall effect sensors and the optical encoder.
// The speeds must always agree to within the resolutions of the sensors plus quantization and timing error.

#include "encoder_monitor.h"
#include "fpga.h"
#include "status.h"

static const int Encoder_Ticks_Per_Rev = 1440;
static const int Hall_Ticks_Per_Rev = 48;

// Margin of error for each type of sensor.
// The margins are different because the encoder count rate appears to vary due to shaft offset/angle error.
static const int Hall_Margin = 6;
static const int Encoder_Margin = 2;

uint8_t encoder_faults;
int em_err_hall[5], em_err_enc[5], em_err_out[5];

void encoder_monitor()
{
	if (base2008)
	{
		// 2008: Make sure the encoder inputs are idle (allow one count for noise).
		// If the switch is set to 2008 but on a 2011 base, this will kill all the motors.
		for (int i = 0; i < 4; ++i)
		{
			if (encoder_delta[i] < -1 || encoder_delta[i] > 1)
			{
				encoder_faults |= 1 << i;
			}
		}
	} else {
		// 2011: Make sure the encoders work
		for (int i = 0; i < 4; ++i)
		{
			int enc_min = (hall_delta[i] - Hall_Margin) * Encoder_Ticks_Per_Rev / Hall_Ticks_Per_Rev - Encoder_Margin;
			int enc_max = (hall_delta[i] + Hall_Margin) * Encoder_Ticks_Per_Rev / Hall_Ticks_Per_Rev + Encoder_Margin;
			if (encoder_delta[i] < enc_min || encoder_delta[i] > enc_max)
			{
				em_err_hall[i] = hall_delta[i];
				em_err_enc[i] = encoder_delta[i];
				em_err_out[i] = motor_out[i];
				
				encoder_faults |= 1 << i;
			}
		}
	}
}