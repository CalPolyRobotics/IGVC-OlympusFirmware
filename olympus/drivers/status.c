#include <stdio.h>

#include "comms.h"
#include "submoduleComms.h"

#include "apollo.h"
#include "config.h"
#include "error.h"
#include "janus.h"
#include "hephaestus.h"
#include "hermes.h"
#include "status.h"
#include "zeus.h"

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
        ErrorHandler(HEPH_STATUS_FAIL, NOTIFY);
    }

    if(!checkSubmoduleStatus(HERMES))
    {
        printf("Status check failed for Hermes\r\n");
        ErrorHandler(HERMES_STATUS_FAIL, NOTIFY);
    }

    if(!checkSubmoduleStatus(JANUS))
    {
        printf("Status check failed for Janus\r\n");
        ErrorHandler(JANUS_STATUS_FAIL, NOTIFY);
    }

    if(!checkSubmoduleStatus(ZEUS))
    {
        printf("Status check failed for Zeus\r\n");
        ErrorHandler(ZEUS_STATUS_FAIL, NOTIFY);
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
        case HERMES:
            getStatus = getHermesStatus;
            break;
        case JANUS:
            getStatus = getJanusStatus;
            break;
        case ZEUS:
            getStatus = getZeusStatus;
            break;
        default:
            return false;
    }

    return getStatus() == COMMS_OK;
}
