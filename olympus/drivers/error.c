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
    Error();//plays the shutdown noise to tell you something went wrong
}


void ErrorHandler(char* type, THREATLevel severity)
{

    if (severity == KILL) {
        killBoard();//kills all functionality of the board
    }

    if (severity == ALERT) {
        Error();//plays the noise to alert you
    }

    if (severity == NOTIFY) {
        setSevenSeg(type);//set the sevenseg to the error
        Error();//plays the song to further alert you
    }

}
