#include <stdint.h>
#include "tim.h"

static void brake_drive_release(uint8_t duty);
static void brake_drive_press(uint8_t duty);
static void brake_drive_stop();

// TODO - verify these are the proper drive directions
static void brake_drive_release(uint8_t duty)
{
    tim1_set_channel_duty(3, 0);
    tim1_set_channel_duty(4, duty);
}

static void break_drive_press(uint8_t duty)
{
    tim1_set_channel_duty(3, 0);
    tim1_set_channel_duty(4, duty);
}

static void brake_drive_stop()
{
    tim1_set_channel_duty(3, 0);
    tim1_set_channel_duty(4, 0);
}
