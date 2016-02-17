/** 
 * \file edinburghvsa.pde
 * \author Matthew Howard (MH), matthew.howard@ed.ac.uk
 * \ingroup Swinger
 *
 * \brief Arduino program for controlling/reading sensors of the Edinburgh VSA
 * using interrupts via a serial connection. 
 *
 * Use this in combination with the mex or python interfaces to read
 * sensors/send commands and test the robot's response.
 *
 * The connections to the arduino board are as follows:
 * \li Motor 1 signal wire (usually yellow) is assumed to be attached to digital pin 9 of the Arduino.
 * \li Motor 2 signal wire attached to digital pin 10.
 * \li Motors 1 and 2 powered by 7.2V (red wire to +7.2V, black to ground).
 * \li Motor 1 potentiometer (purple) attached to analogue pin 4.
 * \li Motor 2 potentiometer (purple) attached to analogue pin 5.
 * \li Accelerometer attached to analogue pin 3.
 * \li Potentiometer attached to analogue pins 0 and 1.
 * \li Accelerometer and potentiometer powered by 5V (can use power out pins on arduino board for this).
 * 
 * \note This is adapted originally from the 'sw2' sketchbook written by Stefan Klanke 
 * for controlling the Edinburgh VSA.
 *
 */
#include "defines.h"

union {
	struct {
		/** \brief Servo0 command. */
		unsigned int u0;
		/** \brief Servo1 command. */
		unsigned int u1;
	};
	unsigned char bytes[RECEIVE_LENGTH];
} 
/** \brief Receive buffer. Bit-string in which commands received from the serial are stored temporarily. */
receive_buffer,
/** \brief Command buffer. Bit-string in which commands are stored (these are copied from receive_buffer). */
command_buffer;

/** 
 * \brief Transmit buffer: bit-string used for sending sensor values back to
 *  the PC via the serial interface. Bit-string is TRANSMIT_LENGTH bytes in length (1x unsigned int = 2x char = 2x bytes = 2x 8 bits). 
 */
union {
	struct {
		/** \brief Header. */
		unsigned int header;
		/** \brief Joint potentiometer reading. */
		unsigned int position;
		/** \brief Joint accelerometer reading. */
		unsigned int acceleration;
		/** \brief Servo0 potentiometer reading. */
		unsigned int m0;
		/** \brief Servo1 potentiometer reading. */
		unsigned int m1;
	};
	unsigned char bytes[TRANSMIT_LENGTH];
} transmit_buffer;

/** \brief Temporary variable for storing bytes read from the serial. */
unsigned char input;

/** \brief Variable for keeping track of which position in receive struct we are reading from. */
unsigned char receive_position;

/** \brief Variable for keeping track of which position in transmit struct we are sending from. */
unsigned char transmit_position;

int posAccu;
int accAccuX;
int accAccuY;
int motAccu1;
int motAccu2;

/** 
 * \brief Initialisation (called after initial upload or reset button pressed) 
 */
void setup() {
  
  Serial.begin(BAUDRATE);     // Set the serial speed
  receive_position = 0;           // reset reading position to zero 
  command_buffer.u0 = U_INIT_SERVO0;     // set initial command for motor 0
  command_buffer.u1 = U_INIT_SERVO1;     // set initial command for motor 1
  posAccu = 0;           // reset sensor measurements to zero
  accAccuX = accAccuY = 0;
  motAccu1 = motAccu2 = 0;
  transmit_buffer.bytes[0] = 0xFF; // set up headers for transmit_buffer
  transmit_buffer.bytes[1] = 0xFF; // 

  digitalWrite(9, LOW);  // set digital pin 9 to LOW (i.e., 0V )
  pinMode(9, OUTPUT);    // set digital pin 9 as an output pin (for sending signal to servo 1).
  digitalWrite(10, LOW); // set digital pin 10 to LOW (i.e., 0V )
  pinMode(10, OUTPUT);   // set digital pin 10 as an output pin (for sending signal to servo 2).

  setup_timers();            // set up internal timers/interrupts
}

/** 
 * \brief Set pins on Arduino for output to motors, set up internal Atmel timer to do fast PWM.
 */
void setup_timers() {

  uint8_t oldSREG = SREG; // Store status register (SREG)

  cli();                  // Suspend interupts while we set up the registers

  TCCR1A = _BV(WGM11); /* Fast PWM, ICR1 is top */
  TCCR1B = _BV(WGM13) | _BV(WGM12) /* Fast PWM, ICR1 is top */
  | _BV(CS11) /* div 8 clock prescaler */
  ;
  OCR1A = U_INIT_SERVO0; /* Set initial value to OCR1A register (that determines the PWM signal to Servo 0). */
  OCR1B = U_INIT_SERVO1; /* Set initial value to OCR1B register (that determines the PWM signal to Servo 1). */
  ICR1 = clockCyclesPerMicrosecond()*(20000L/8) - 1;  // 20000 uS is a bit fast for the refresh, 20ms, but 
                                                      // it keeps us from overflowing ICR1 at 20MHz clocks
                                                      // That "/8" at the end is the prescaler.

  TIMSK1 &=  ~(_BV(OCIE1A) | _BV(OCIE1B) | _BV(TOIE1) );
  TIMSK1|=_BV(TOIE1);
 
  
  TCCR1A = (TCCR1A & ~_BV(COM1A0)) | _BV(COM1A1);
  TCCR1A = (TCCR1A & ~_BV(COM1B0)) | _BV(COM1B1);
  
  ASSR &= ~_BV(AS2);
  
  TCCR2A = 0; // normal    ---  2; //compare match
  TCCR2B = 6; // 256 presacler
  OCR2B = 255;
  
  TIMSK2|=_BV(OCIE2A) |_BV(TOIE2); 
  
  SREG    = oldSREG;  // undo cli()
}

/** \brief Interrupt for reading the motor potentiometer. 
    This is initiated when Atmel Timer 1 is reset (every 20000 ticks, i.e., every 20 milliseconds). 
    */
ISR(TIMER1_OVF_vect) {
   OCR1A = command_buffer.u0; // send command
   OCR1B = command_buffer.u1;
   TCNT2 = 0; /* This resets Timer 2, triggering an interrupt to read the potentiometer values.*/
   transmit_buffer.position = posAccu; // record potentiometer reading
   transmit_buffer.acceleration = accAccuY; // record accelerometer reading
   transmit_buffer.m0 = motAccu1;
   transmit_buffer.m1 = motAccu2;
   posAccu = accAccuY = motAccu1 = motAccu2 = 0; // reset these variables (only needed if using oversampling for noise reduction)
   transmit_position = 1;          // ??
   
   Serial.write(0xFF);       // write the first header
   UCSR0B |= 0x40;
}

ISR(TIMER2_COMPA_vect) {
}

/** \brief Interrupt for reading the motor potentiometer. 
    This is initiated when Atmel Timer 2 is reset. */
ISR(TIMER2_OVF_vect) {   
   posAccu += analogRead(0);
   posAccu += analogRead(1);
   accAccuY += analogRead(3);
   motAccu1 += analogRead(4);
   motAccu2 += analogRead(5);
   
}

ISR(USART_TX_vect) {
   UDR0 = transmit_buffer.bytes[transmit_position++];
   if (transmit_position>=TRANSMIT_LENGTH) {
      UCSR0B &= 0xBF;
   }
}

/** 
 * \brief Main loop. Listens for serial communications, copies these to the command buffer when a full command has been recieved.
 */
void loop() {
	if (Serial.available()>0) {                               // if new data at serial...
		input = Serial.read();                                // ...read byte
		if (input == RECEIVE_HEADER) {                        // header flags a new command
			receive_position = 0;                             // reset receive_position to zero
		} else {
			receive_buffer.bytes[receive_position++] = input; // copy byte to receive buffer
			if (receive_position == RECEIVE_LENGTH) {         // if all bytes have been read, command motors, reset receive_position
				cli();                                        // suspend interrupts
				command_buffer = receive_buffer;              // write received data to command_buffer
				sei();                                        // re-enable interrupts
				receive_position = 0;                         // reset reading position to zero
			}
		}
	}
}
