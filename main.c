#include "main.h"
#include "gpio.h"
#include "timer.h"
#include "i2c.h"
#include "usart.h"


static __IO uint32_t TimingDelay;

Timer_Return LEDBlink(void* pin)
{
  HAL_GPIO_TogglePin(GPIOA, *(uint32_t*)pin);
  return CONTINUE_TIMER;
}

int main(void)
{

  HAL_Init();

  SysTick_Config(HAL_RCC_GetHCLKFreq() / 100);

  RCC->AHB1ENR |= 0xFFFFFFFF;
  RCC->AHB2ENR |= 0xFFFFFFFF;
  RCC->AHB3ENR |= 0xFFFFFFFF;
  RCC->APB1ENR |= 0xFFFFFFFF;
  RCC->APB2ENR |= 0xFFFFFFFF;


  GPIO_InitTypeDef gpio;

  gpio.Pin = GPIO_PIN_All & ~(GPIO_PIN_13 | GPIO_PIN_14);
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOA, &gpio);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_7, 0);

  initIGVCUsart();

  initIGVCCallbackTimer();

  initIGVCI2C();

  uint32_t pin4 = (1 << 4);
  uint32_t pin5 = (1 << 5);
  uint32_t pin7 = (1 << 7);

  addCallbackTimer(2000, LEDBlink, &pin4);
  addCallbackTimer(1000, LEDBlink, &pin5);
  addCallbackTimer(500, LEDBlink, &pin7);

  while(1)
  {

    testI2C();

    Delay(100);
  }

  while(1);
}



/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

/*void assert_param(int x)
{

}*/

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
