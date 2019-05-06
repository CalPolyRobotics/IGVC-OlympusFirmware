#include <stdio.h>

#include "comms.h"
#include "submoduleComms.h"

#include "apollo.h"
#include "config.h"
#include "error.h"
#include "janus.h"
#include "hephaestus.h"
#include "hera.h"
#include "status.h"

static int checkSubmoduleStatus(module_t module);

void checkAllSubmodules()
{
    if(!checkSubmoduleStatus(APOLLO))
    {
        printf("Status check failed for Apollo\r\n");
        ErrorHandler(APOLLO_STATUS_FAIL, NOTIFY);
    }

    if(!checkSubmoduleStatus(HEPHAESTUS))
    {
        printf("Status check failed for Hephaestus\r\n");
        ErrorHandler(HEPHAESTUS_STATUS_FAIL, NOTIFY);
    }

    if(!checkSubmoduleStatus(HERA))
    {
        printf("Status check failed for Hera\r\n");
        ErrorHandler(HERA_STATUS_FAIL, NOTIFY);
    }

    if(!checkSubmoduleStatus(JANUS))
    {
        printf("Status check failed for Janus\r\n");
        ErrorHandler(JANUS_STATUS_FAIL, NOTIFY);
    }
}

/**
 * Returns whether or not the given module responded with a ready status byte
 * @retval: true  - Received valid status
 *          false - Read timed out
 **/
static int checkSubmoduleStatus(module_t module)
{
    commsStatus_t (*getStatus)(void);

    switch(module)
    {
        case APOLLO:
            getStatus = getApolloStatus;
            break;
        case HEPHAESTUS:
            getStatus = getHephaestusStatus;
            break;
        case HERA:
            getStatus = getHeraStatus;
            break;
        case JANUS:
            getStatus = getJanusStatus;
            break;
        default:
            return false;
    }

    return getStatus() == COMMS_OK;
}
