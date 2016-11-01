#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

#define SLAVE_ADDR 0x1B

typedef enum {WAIT_STARTBIT, WAIT_ADDR, WAIT_ACK, WAIT_DATA, STATE_BAD} WaitState_t;

static WaitState_t state;



void USIinit()
{

    //DDRB |= (1 << PB2);
    //DDRA |= (1 << PA7);
    PORTA |= (1 << PA4);
    DDRA |= (1 << PA4) /*| (1 << PA6)*/;
    USIDR = (0x55);

    USISR = (1 << USISIF) | (1 << USIOIF);

    USICR = (1 << USISIE) | (1 << USIOIE) | \
            (1 << USIWM1) | \
            (1 << USICS1);

    //USICR = 0xF8;

}

void startTimeoutTimer()
{
    TCCR1A = 0;
    TCNT1H = 0;
    TCNT1L = 0;
    TIMSK1 = (1 << TOIE1);
    TCCR1B = (1 << CS10);
}

void stopTimeoutTimer()
{
    TCCR1B = 0;
}

uint8_t mapFNR(){
    return (PINA & 0x7) ^ 0x4;
}


ISR(USI_STR_vect)
{
    //state = WAIT_STARTBIT;
    state = WAIT_ADDR;

    //PORTB &= ~(1 << PB2);
    //PORTA &= ~(1 << PA7);

    USISR |= (1 << USISIF);
    USISR &= 0xF0;
    //USISR |= 0x0F;

    startTimeoutTimer();
}

ISR(USI_OVF_vect)
{
    USISR |= (1 << USIOIF);

    switch (state)
    {
        case WAIT_STARTBIT:
            USISR &= 0xF0;
            state = WAIT_ADDR;
            break;

        case WAIT_ADDR:

            if (USIBR == SLAVE_ADDR)
            //if (USIBR >= 0x10)
            {
                DDRA |= (1 << PA6);
                PORTA &= ~(1 << PA6);
            //} else {
                //PORTB &= ~(1 << PB2);
            }
            state = WAIT_ACK;
            USISR = (USISR & 0xF0) | 14;
            break;

        case WAIT_ACK:
            PORTA |= (1 << PA6);
            //PORTB |= (1 << PB2);
            USIDR = mapFNR();
            state = WAIT_DATA;
            break;

        case WAIT_DATA:
            DDRA &= ~(1 << PA6);
            stopTimeoutTimer();
            //PORTA |= (1 << PA7);
            break;
        case STATE_BAD:
            DDRA &= ~(1 << PA6);
            break;
    }
}

ISR(TIM1_OVF_vect)
{
    state = STATE_BAD;
    DDRA &= ~(1 << PA6);

    stopTimeoutTimer();
}

