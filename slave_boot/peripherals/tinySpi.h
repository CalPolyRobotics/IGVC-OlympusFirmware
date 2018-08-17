#ifndef TINYSPI_H_
#define TINYSPI_H_

#include "commsLib.h"

void MX_SPI1_Init();
void SPI1_LL_Init();

wrError_t readResponse(uint8_t *data, uint16_t length);
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
