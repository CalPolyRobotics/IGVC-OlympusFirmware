#include <stdio.h>
#include "error.h"

void assert_failed(uint8_t* file, uint32_t line)
{
    runErrorSong();
}

void ErrorHandler(char* type, THREATLevel severity)
{
    // Set seven segment for KILL, ALERT, and NOTIFY
    setSevenSeg(type);

    if(severity == KILL){
        killBoard();
    }else if(severity == ALERT){
        runErrorSong();
        printf("ALERT: %s\r\n", type);
    }else if(severity == NOTIFY){
        printf("NOTIFY: %s\r\n", type);
    }
}
