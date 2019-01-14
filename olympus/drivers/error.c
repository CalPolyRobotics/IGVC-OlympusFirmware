#include "error.h"

#ifdef USE_FULL_ASSERT

/*@brief Reportd the name of the source file and the source line number
 * where the assert_param error has occurred
 * @param file: pointer to the source file name
 * @param line: assrt_param error error line source number
 * @retval None */

void assert_failed(uint8_t* file, unit32_t line)
{
    /*User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    Error();
}

#endif
