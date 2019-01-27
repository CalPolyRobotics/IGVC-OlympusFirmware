#include "error.h"

/*@brief Reportd the name of the source file and the source line number
 * where the assert_param error has occurred
 * @param file: pointer to the source file name
 * @param line: assrt_param error error line source number
 * @retval None */

void assert_failed(uint8_t* file, uint32_t line)
{
    /*User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    Error();
}


void ErrorHandler(char* type, THREATLevel severity)
{

    if (severity == KILL) {
        killBoard();
    }

    if (severity == ALERT) {
        Error();
    }

    if (severity == NOTIFY) {
        setSevenSeg(type);
    }

}
