#include <avr/io.h>
#include <util/delay.h>

#define MCU_EN_PIN (1 << PA3)
#define MCU_DIR_PIN (1 << PA7)

#define FOR_CTRL_PIN (1 << PB0)
#define REV_CTRL_PIN (1 << PB1)

#define DIR_FOR   1
#define DIR_REV   2

int main()
{

   PORTB = 0;

   DDRA = 0;
   DDRB = (1 << PB0) | (1 << PB1);

   int oldDir = -1;

   while (1)
   {
      if (!(PINA & (MCU_EN_PIN)))
      {
         PORTB = 0;
         oldDir = -1;
      } else {
         if (PINA & (MCU_EN_PIN))
         {
            if (oldDir != DIR_FOR)
            {
               oldDir = DIR_FOR;
               PORTB = 0;
               _delay_ms(250);
               PORTB = FOR_CTRL_PIN;
            }
         } else {
            if (oldDir != DIR_REV)
            {
               oldDir = DIR_REV;
               PORTB = 0;
               _delay_ms(250);
               PORTB = REV_CTRL_PIN;
            }
         }
      }
   }

}
