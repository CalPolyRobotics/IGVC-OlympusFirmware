#include <stdbool.h>
#include "gpio.h"
#include "steer.h"

#define DEADZONE      ((uint16_t)50u)

#define POT_DEG_SLOPE     ((MAX_RIGHT_POT - MAX_LEFT_POT)/(MAX_RIGHT_DEG - MAX_LEFT_DEG))
#define POT_DEG_INTERCEPT (MAX_LEFT_POT - (MAX_LEFT_DEG * POT_DEG_SLOPE))

uint16_t steeringPot = 0u;
uint16_t targetSteerValue = 0u;

static uint16_t deg_to_pot(uint8_t deg);

static void steer_left();
static void steer_right();
static void steer_stop();

static bool enableSteering = false;

void set_steer_target(uint8_t angle)
{
    targetSteerValue = deg_to_pot(angle);

    /* Only run control loop if steeringPotValue is set */
    if(steeringPot != 0u)
    {
        enableSteering = true;
    }
}


void update_steer_control_loop()
{
    static bool motorOnLeft = false;
    static bool motorOnRight = false;

    if(enableSteering)
    {
        if(steeringPot < (targetSteerValue - DEADZONE))
        {
            /* Steering is too far left */
            if(!motorOnRight)
            {
                steer_right();
                motorOnRight = true;
                motorOnLeft = false;
            }

        }
        else if(steeringPot > (targetSteerValue + DEADZONE))
        {
            /* Steering is too far right */
            if(!motorOnLeft)
            {
                steer_left();
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

static void steer_left()
{
    HAL_GPIO_WritePin(STEER_PORT, STEER_PINR, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEER_PORT, STEER_PINL, GPIO_PIN_SET);
}

static void steer_right()
{
    HAL_GPIO_WritePin(STEER_PORT, STEER_PINL, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEER_PORT, STEER_PINR, GPIO_PIN_SET);
}

static void steer_stop()
{
    HAL_GPIO_WritePin(STEER_PORT, STEER_PINL | STEER_PINR, GPIO_PIN_RESET);
}
