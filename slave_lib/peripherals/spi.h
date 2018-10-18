#ifndef SPI_H_
#define SPI_H_

#include <stdbool.h>
#include "commsLib.h"

#define COMMS_SPI SPI1

#define STD_SPI_DELAY ((uint32_t)100u)

void MX_COMMS_SPI_Init();
void COMMS_SPI_LL_Init();
void COMMS_SPI_ReInit();

void flushSPIRx();
void flushSPITx();

void setSPIInt();
void clearSPIInt();

wrError_t readByte(uint8_t *data);
wrError_t writeResponse(uint8_t* data, uint16_t length);

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
