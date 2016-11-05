#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

typedef enum {STEERING_STOP, STEERING_LEFT, STEERING_RIGHT} MotorDirection;

void initSteeringMotor();
void setSteeringMotorDir(MotorDirection dir);

#endif
