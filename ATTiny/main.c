#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usi.h"

#define MCU_EN_PIN (1 << PB2)
#define MCU_EN_PORT PORTB
#define MCU_DIR_PIN (1 << PA7)
#define MCU_DIR_PORT PORTA

#define FOR_CTRL_PIN (1 << PB0)
#define REV_CTRL_PIN (1 << PB1)

#define DIR_FOR   1
#define DIR_REV   2

#define AUTOEN_PIN   (1 << PA5)
#define N_DETECT_PIN (1 << PA2)

#define AUTOEN_PCINT    (1 << PCINT5)
#define N_DETECT_PCINT  (1 << PCINT2)

typedef enum {NEUTRAL = 0, FORWARD, REVERSE} FNR_t;

volatile int outputsEnabled = 0;
volatile FNR_t currentState;

void checkOutputsEnabled();
void writeFNRState();
void writeFNRState_ISR();

int main()
{
   PORTB = 0;

   //DDRA |= (1 << PA6) | (1 << PA4);
   DDRA = 0;
   PORTA |= (1 << PA6) | (1 << PA4);
   DDRB = (1 << PB0) | (1 << PB1);


   PORTB = 0;

   USIinit();
   sei();

   //while(1);

   /*while (1)
   {
      PORTB |= (1 << PB0) | (1 << PB1);
      _delay_ms(4000);
      PORTB &= ~((1 << PB0) | (1 << PB1));
      _delay_ms(2000);
   }*/

   int oldDir = -1;

   checkOutputsEnabled();

   currentState = NEUTRAL;
   writeFNRState();

   //PCMSK0 = AUTOEN_PCINT | N_DETECT_PCINT;
   //GIMSK |= (1 << PCIE0);

   //sei();

   while (1)
   {
      if (!(PINB & (MCU_EN_PIN)))
      {
         currentState = NEUTRAL;
         writeFNRState();

         oldDir = -1;
      } else {
         if (PINA & (MCU_DIR_PIN))
         {
            if (oldDir != DIR_FOR)
            {
               oldDir = DIR_FOR;
               currentState = NEUTRAL;
               writeFNRState();

               _delay_ms(250);

               currentState = FORWARD;
               writeFNRState();
            }
         } else {
            if (oldDir != DIR_REV)
            {
               oldDir = DIR_REV;
               currentState = NEUTRAL;
               writeFNRState();

               _delay_ms(250);

               currentState = REVERSE;
               writeFNRState();
            }
         }
      }

      if (PINA & (1 << PA0))
      {
         PORTB |= (1 << PB2);
      } else {
         PORTB &= ~(1 << PB2);
      }

      if (PINA & (1 << PA1))
      {
         PORTA |= (1 << PA7);
      } else {
         PORTA &= ~(1 << PA7);
      }
   }
}

void writeFNRState()
{
   //cli();
   writeFNRState_ISR();
   //sei();
}

void writeFNRState_ISR()
{
   //if (outputsEnabled)
   //{
      switch (currentState)
      {
         case NEUTRAL:
            PORTB = 0;
            break;
         case FORWARD:
            PORTB = FOR_CTRL_PIN;
            break;
         case REVERSE:
            PORTB = REV_CTRL_PIN;
            break;
      }
   //} else {
      //PORTB = 0;
   //}
}

void checkOutputsEnabled()
{
   outputsEnabled = 1;
   /*if ((PINA & AUTOEN_PIN) && 
       (!(PINA & N_DETECT_PIN)))
   {
      outputsEnabled = 1;
   } else {
      outputsEnabled = 0;
   }*/
}

ISR(PCINT0_vect)
{
   checkOutputsEnabled();
   writeFNRState_ISR();
}

