#ifndef SPI_H_
#define SPI_H_

#include "commsLib.h"

#define STD_SPI_DELAY ((uint32_t)100u)

#define COMMS_SPI SPI1

void MX_COMMS_SPI_Init();
void COMMS_SPI_LL_Init();

wrError_t readByte(uint8_t *data, uint32_t timeout);
wrError_t writeResponse(uint8_t* data, uint16_t length, uint32_t timeout);

/** @brief  Clear the SPI OVR pending flag.
  * @param  __HANDLE__ specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */
#define __SPI_CLEAR_OVRFLAG(__HANDLE__)  \
  do{                                    \
    __IO uint32_t tmpreg_ovr = 0x00U;    \
    tmpreg_ovr = (__HANDLE__)->DR;       \
    tmpreg_ovr = (__HANDLE__)->SR;       \
    (void)(tmpreg_ovr);                  \
  } while(0U)
#endif
