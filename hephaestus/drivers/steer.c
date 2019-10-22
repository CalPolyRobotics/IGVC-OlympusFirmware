#include <stdbool.h>
#include "steer.h"
#include "tim.h"
#include "adc.h"

#define DEADZONE      ((uint16_t)50u)

#define POT_DEG_SLOPE     ((MAX_RIGHT_POT - MAX_LEFT_POT)/(MAX_RIGHT_DEG - MAX_LEFT_DEG))
#define POT_DEG_INTERCEPT (MAX_LEFT_POT - (MAX_LEFT_DEG * POT_DEG_SLOPE))

#define BANG_BANG_DUTY ((uint8_t)50u)

uint16_t steeringPot = 0u;
uint16_t targetSteerValue = 0u;

static uint16_t deg_to_pot(uint8_t deg);

static void steer_left(uint8_t duty);
static void steer_right(uint8_t duty);
static void steer_stop();

static bool enableSteering = false;

void set_steer_target(uint8_t angle)
{
    targetSteerValue = deg_to_pot(angle);
}

void update_steer_control_loop()
{
    static bool motorOnLeft = false;
    static bool motorOnRight = false;

    if(enableSteering)
    {
        // Get latest steeringPot value
        steeringPot =  adcChnAvg[adcChnReadIdx][STEER_AVG_IDX];

        if(steeringPot < (targetSteerValue - DEADZONE))
        {
            /* Steering is too far left */
            if(!motorOnRight)
            {
                steer_right(BANG_BANG_DUTY);
                motorOnRight = true;
                motorOnLeft = false;
            }

        }
        else if(steeringPot > (targetSteerValue + DEADZONE))
        {
            /* Steering is too far right */
            if(!motorOnLeft)
            {
                steer_left(BANG_BANG_DUTY);
                motorOnLeft = true;
                motorOnRight = false;
            }
        }
        else
        {
            steer_stop();
            motorOnRight = false;
            motorOnLeft = false;
            enableSteering = false;
        }
    }
}

static uint16_t deg_to_pot(uint8_t deg)
{
    return (deg * POT_DEG_SLOPE) + POT_DEG_INTERCEPT;
}

// TODO - verify these are the proper drive directions
static void steer_left(uint8_t duty)
{
    tim1_set_channel_duty(1, 0);
    tim1_set_channel_duty(2, duty);
}

static void steer_right(uint8_t duty)
{
    tim1_set_channel_duty(2, 0);
    tim1_set_channel_duty(1, duty);
}

static void steer_stop()
{
    tim1_set_channel_duty(1, 0);
    tim1_set_channel_duty(2, 0);
}
