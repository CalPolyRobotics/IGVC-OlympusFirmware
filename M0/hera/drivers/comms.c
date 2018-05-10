#include <stdlib.h>
#include "boot.h"
#include "comms.h"

/** tx or rx DataLengths can be no longer than 253(rx) 253(tx) **/
msgInfo_t msgResp[NUM_MSGS] = {
    {0, 0, NULL}, /** 0 Unused **/
    {0, 0, NULL}, /** 1 Unused **/
    {0, 0, NULL}, /** 2 Unused **/
    {0, 0, NULL}, /** 3 Unused **/
    {0, 0, NULL}, /** 4 Unused **/
    {0, 0, NULL}, /** 5 Unused **/
    {0, 0, NULL}, /** 6 Unused **/
    {0, 0, NULL}, /** 7 Unused **/
    {0, 0, NULL}, /** 8 Unused **/
    {0, 0, NULL}, /** 9 Unused **/
    {0, 0, bootload}, /** 10 Reset To Bootloader **/
};
