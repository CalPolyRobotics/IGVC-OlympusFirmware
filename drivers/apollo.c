#include <stdint.h>
#include "apollo.h"
#include "moduleComms.h"

static MODULE apollo_module = APOLLO;
static uint8_t apollo_tx_buff[256];

typedef struct message_size{
    uint8_t tx_size;
    uint8_t rx_size;
} message_size;

message_size apollo_msg_sizes[11]= {
    // Indexed with APOLLO_MSG
    {1, 1}, /* 0x00 - SET_TURN_SIGNAL */
    {1, 1}, /* 0x01 - SET_HEADLIGHTS */
    {2, 1}, /* 0x02 - SET_MISC_LIGHTS */
    {3, 1}, /* 0x03 - GET_LIGHT_SENSOR */
    {0, 0}, /* 0x04 - Unused */
    {0, 0}, /* 0x05 - Unused */
    {0, 0}, /* 0x06 - Unused */
    {0, 0}, /* 0x07 - Unused */
    {0, 0}, /* 0x08 - Unused */
    {0, 0}, /* 0x09 - Unused */
    {1, 1}, /* 0x0A - BOOTLOAD */
};

void apollo_send_turn_signal(uint8_t byte){
    APOLLO_MSG message_type = SET_TURN_SIGNAL;
    apollo_tx_buff[0] = byte;
    uint8_t tx_size = apollo_msg_sizes[message_type].tx_size;
    uint8_t rx_size = apollo_msg_sizes[message_type].rx_size;
    send_message(apollo_module, message_type, apollo_tx_buff, tx_size, rx_size);
}
