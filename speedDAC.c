
#include "speedDAC.h"
#include "i2c.h"
#include "comms.h"

#define SPEED_DAC_ADDRESS   0x62

void initSpeedDAC()
{
   HAL_GPIO_WritePin(SPEED_DAC_ENABLE_PORT, SPEED_DAC_ENABLE_PIN, GPIO_PIN_SET); 
}

void toggleSpeedDAC(Packet_t* packet)
{
   HAL_GPIO_TogglePin(SPEED_DAC_ENABLE_PORT, SPEED_DAC_ENABLE_PIN);
}

void setSpeedDAC(uint8_t value)
{
    i2cTransmit(SPEED_DAC_ADDRESS, &value, 1);
}

void speedDACHandler(Packet_t* packet)
{
    uint16_t speed;
    if (packet->header.packetLen - sizeof(PacketHeader_t) == 2)
    {
        speed = packet->data[1];
        setSpeedDAC(speed);
    }
}