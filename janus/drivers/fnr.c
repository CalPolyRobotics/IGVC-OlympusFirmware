#include "fnr.h"

fnr_t getFNR(){
    // Check neutral first since neutral and forward could be high at the same time

    if(HAL_GPIO_ReadPin(GPIO_FNR_SENSE_PORT, GPIO_FORWARD_SENSE_PIN) == GPIO_PIN_SET){
        return FORWARD;
    }

    if(HAL_GPIO_ReadPin(GPIO_FNR_SENSE_PORT, GPIO_REVERSE_SENSE_PIN) == GPIO_PIN_SET){
        return REVERSE;
    }

    if(HAL_GPIO_ReadPin(GPIO_FNR_SENSE_PORT, GPIO_NEUTRAL_SENSE_PIN) == GPIO_PIN_RESET){
        return NEUTRAL;
    }

    return INVALID;
}

void setFNR(fnr_t state){
    if(state == NEUTRAL){
        HAL_GPIO_WritePin(GPIO_FNR_CTRL_PORT, GPIO_FORWARD_CTRL_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIO_FNR_CTRL_PORT, GPIO_REVERSE_CTRL_PIN, GPIO_PIN_RESET);
    }

    if(state == FORWARD){
        HAL_GPIO_WritePin(GPIO_FNR_CTRL_PORT, GPIO_REVERSE_CTRL_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIO_FNR_CTRL_PORT, GPIO_FORWARD_CTRL_PIN, GPIO_PIN_SET);
    }

    if(state == REVERSE){
        HAL_GPIO_WritePin(GPIO_FNR_CTRL_PORT, GPIO_FORWARD_CTRL_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIO_FNR_CTRL_PORT, GPIO_REVERSE_CTRL_PIN, GPIO_PIN_SET);
    }

}
