#include "main.h"
#include "timer.h"


static __IO uint32_t TimingDelay;

Timer_Return LEDBlink(void* pin)
{
  GPIO_ToggleBits(GPIOA, *(uint32_t*)pin);
  return CONTINUE_TIMER;
}

int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

  RCC->AHB1ENR |= 0xFFFFFFFF;
  RCC->AHB2ENR |= 0xFFFFFFFF;
  RCC->AHB3ENR |= 0xFFFFFFFF;
  RCC->APB1ENR |= 0xFFFFFFFF;
  RCC->APB2ENR |= 0xFFFFFFFF;


  GPIO_InitTypeDef gpio;

  gpio.GPIO_Pin = GPIO_Pin_All;
  gpio.GPIO_Mode = GPIO_Mode_OUT;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOA, &gpio);

  GPIO_ResetBits(GPIOA, GPIO_Pin_5);
  GPIO_ResetBits(GPIOA, GPIO_Pin_7);

  initIGVCUsart();
  initIGVCCallbackTimer();

  uint32_t pin4 = (1 << 4);
  uint32_t pin5 = (1 << 5);
  uint32_t pin7 = (1 << 7);

  addCallbackTimer(2000, LEDBlink, &pin4);
  addCallbackTimer(1000, LEDBlink, &pin5);
  addCallbackTimer(500, LEDBlink, &pin7);
  while(1)
  {
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

void assert_param(int x)
{

}

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
