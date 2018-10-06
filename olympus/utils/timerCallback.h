#ifndef __TIMER_H__
#define __TIMER_H__

typedef enum {
    DISABLE_TIMER = 0,
    CONTINUE_TIMER
} Timer_Return;

typedef Timer_Return (*timerCallback)(void*);

void initIGVCCallbackTimer(void);
int32_t addCallbackTimer(uint32_t count, timerCallback callback, void* parameters);
void serviceCallbackTimer(void);


#endif
