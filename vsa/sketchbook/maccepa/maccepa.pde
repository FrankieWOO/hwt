/** 
 * \file maccepa.pde
 * \author Matthew Howard (MH), matthew.howard@ed.ac.uk
 * \ingroup MACCEPA2DOF
 *
 * \brief Arduino program for controlling/reading sensors of the maccepa 
 * using interrupts via a serial connection. 
 *
 * Use this in combination with the mex or python interfaces to read
 * sensors/send commands and test the robot's response.
 *
 * The connections to the arduino board are as follows:
 * \li Motor 1 signal wire (usually yellow) is assumed to be attached to digital pin 9 of the Arduino. \note This should correspond to the motor controlling equilibrium position!
 * \li Motor 2 signal wire attached to digital pin 10.
 * \li Motors 1 and 2 powered by 7.2V (red wire to +7.2V, black to ground).
 * \li Motor 1 potentiometer (purple) attached to analogue pin 4.
 * \li Motor 2 potentiometer (purple) attached to analogue pin 5.
 * \li Accelerometer attached to analogue pin 3.
 * \li Potentiometer attached to analogue pins 0 and 1.
 * \li Accelerometer and potentiometer powered by 5V (can use power out pins on arduino board for this).
 * 
 */
#include "defines.h"

union {
	struct {
		/** \brief Servo0 command. */
		unsigned int u0;
		/** \brief Servo1 command. */
		unsigned int u1;
#ifdef  VARIABLE_DAMPING
		/** \brief Servo1 command. */
		unsigned int u2;
#endif     /* -----  not VARIABLE_DAMPING  ----- */
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
		/** \brief Joint position sensor (potentiometer) reading. */
		unsigned int position;
		/** \brief Accelerometer reading. */
		unsigned int acceleration;
		/** \brief Servo0 potentiometer reading. */
		unsigned int m0;
		/** \brief Servo1 potentiometer reading. */
		unsigned int m1;
		/** \brief Power sensor reading. */
		unsigned int power;
	};
	unsigned char bytes[TRANSMIT_LENGTH];
} transmit_buffer;

/** \brief Temporary variable for storing bytes read from the serial. */
unsigned char input;

/** \brief Variable for keeping track of which position in receive struct we are reading from. */
unsigned char receive_position;

/** \brief Variable for keeping track of which position in transmit struct we are sending from. */
unsigned char transmit_position;

/** \brief Position sensor accumulator.       */
int posAccu;
/** \brief Accelerometer accumulator.         */
int accAccuY;
/** \brief Power sensor accumulator.          */
int powerAccu;
/** \brief Motor 1 potentiometer accumulator. */
int motAccu1;
/** \brief Motor 1 potentiometer accumulator. */
int motAccu2;

/** 
 * \brief Initialisation (called after initial upload or reset button pressed) 
 */
void setup() {
  
  Serial.begin(BAUDRATE);     // Set the serial speed
  Serial.write(0x01);         // Write a bit to the serial using the inbuilt Serial library
                              // - this sets up the communication register 
                              // \todo this should not be necessary, if we find out how to set the serial properly.
  receive_position = 0;                // Reset command message position counter
  
  command_buffer.u0 = U_INIT_SERVO0; // Set initial command for motor 0
  command_buffer.u1 = U_INIT_SERVO1; // Set initial command for motor 1

  transmit_buffer.bytes[0] = TRANSMIT_HEADER0;   // Set transmit headers
  transmit_buffer.bytes[1] = TRANSMIT_HEADER1;   // 
  
  posAccu = accAccuY = motAccu1 = motAccu2 = powerAccu = 0; // Reset accumulators
  
  transmit_position = 0;           // Reset transmit message position counter
  
  digitalWrite(OUTPUT_PIN_SERVO0 , LOW   ); pinMode(OUTPUT_PIN_SERVO0, OUTPUT); // Ground the Servo0 pin, and set as an output.
  digitalWrite(OUTPUT_PIN_SERVO1 , LOW   ); pinMode(OUTPUT_PIN_SERVO1, OUTPUT); // Ground the Servo1 pin, and set as an output.
#ifdef  VARIABLE_DAMPING
  pinMode     (OUTPUT_PIN_DAMPER0, OUTPUT); analogWrite(OUTPUT_PIN_DAMPER0, U_INIT_DAMPER0); // Set the Damper0 pin as output, initialise damping command
#endif     /* -----  not VARIABLE_DAMPING  ----- */
  
  setup_timers();             // Set up internal timers/interrupts  
}

/** 
 * \brief Set up Atemel timers/interrupts. This uses the low-level Atmel API to configure the 
 * microprocessor registers for doing PWM (to control the servos) and for reading the sensors.
 */
void setup_timers() {

  uint8_t oldSREG = SREG; // Store status register (SREG)

  cli();                  // Suspend interupts while we set up the registers

  // Set up Timer 1 interrupts. 
  // We put Timer 1 into fast PWM mode. This means that the value in TCNT register
  // is incremented every p clock cycles, where p is the clock prescaler,
  // until the value contained in ICR1 is reached. At this point the Timer 1 
  // overflow interrupt is triggered and TCNT is reset.
  
  // Set up Timer 1 counter control registers (TCCR1A/TCCR1B).
  TCCR1A  = _BV(WGM11) & ~_BV(COM1A0) | _BV(COM1A1) & ~_BV(COM1B0) | _BV(COM1B1);
  TCCR1B  = _BV(WGM13) |  _BV(WGM12)  | _BV(CS11); 
  
  // Set up ICR1 - this is the value at which the clock resets.
  ICR1    = clockCyclesPerMicrosecond()*(20000L/8) - 1; // 20000 uS is a bit fast for the refresh, 20ms, but 
                                                        // it keeps us from overflowing ICR1 at 20MHz clocks
                                                        // That "/8" at the end is the prescaler.

  OCR1A   = U_INIT_SERVO0;   // Set initial value to OCR1A register (that determines the PWM signal to Servo 0).
  OCR1B   = U_INIT_SERVO1;   // Set initial value to OCR1B register (that determines the PWM signal to Servo 1).

  TIMSK1 &=  ~_BV(OCIE1A);   // Disable Timer 1 Compare Match A (OCIE1A) interrupt (not used) 
  TIMSK1 &=  ~_BV(OCIE1B);   // Disable Timer 1 Compare Match B (OCIE1B) interrupt (not used) 
  TIMSK1 |=   _BV(TOIE1);    // Enable Timer 1 Overflow (TOIE1) interrupt
 
  // Set up Timer 2 interrupts.  
  ASSR   &= ~_BV(AS2);     // Set clock source to internal I/O clock
  
  // Set up Timer 2 counter control registers (TCCR1A/TCCR1B).
  TCCR2A  = 0;               // Set Timer 2 to normal mode
  TCCR2B |= _BV(CS02);      // Set prescaler for Timer 2. 
  TCCR2B |= _BV(CS01);      // This sets 'external clock source on T0 pin, falling edge.' - not really sure what this means.
  
  TIMSK2 |=  _BV(TOIE2);     // Enable Timer 2 Overflow (TOIE2) interrupt
  TIMSK2 &= ~_BV(OCIE2A);    // Disable Compare Match A (OCIE2A) interrupt (not used)
  
  SREG    = oldSREG;  // undo cli()
}

/** \brief Timer 1 Overflow interrupt. This is initiated when Atmel Timer 1 is reset (every 20000 ticks, i.e., every 20 milliseconds).
 * 
 *   This triggers the following events:
 *     \li The motor commands are copied from the command message buffer to the servos.
 *     \li Timer 2 is reset.
 *     \li The sensor readings are copied from the accumulators to the transmit message buffer.
 *     \li The accumulators are reset.
 */
ISR(TIMER1_OVF_vect) {
  
   // Copy command from message buffer to registers (i.e., send command to servos)
   OCR1A = command_buffer.u0;
   OCR1B = command_buffer.u1;
#ifdef  VARIABLE_DAMPING
   analogWrite(OUTPUT_PIN_DAMPER0, command_buffer.u2); // Set the Damper0 command
#endif     /* -----  not VARIABLE_DAMPING  ----- */
  
   // Copy sensor readings into message buffer
   transmit_buffer.position     = posAccu;  
   transmit_buffer.acceleration = accAccuY; 
   transmit_buffer.m0       = motAccu1;
   transmit_buffer.m1       = motAccu2;
   transmit_buffer.power        = powerAccu;

   // Reset Timer 2
   TCNT2 = 0; 
   
   // Reset accumulators
   posAccu = accAccuY = motAccu1 = motAccu2 = powerAccu = 0; 
   
   // Reset transmit message position counter
   transmit_position = 0;   
   
   // Switch on the TXCIE0 bit of the USART control and status register (UCSR0B)).
   // This triggers the USART Transmit Complete Interrupt (USART_TX_vect)
   UCSR0B |= _BV(TXCIE0);
}

/** \brief Interrupt for reading the sensors. 
 * 
 * This is initiated when Atmel Timer 2 is reset, which is 4x as often as the Timer 1 interrupt. This means that in one control cycle, we read the sensors 4 times. We can exploit this for noise reduction, by accumulating the readings and taking their average. This is automatically done when using regression for the system identification.
 */
ISR(TIMER2_OVF_vect) {   
   posAccu  += analogRead(INPUT_PIN_JOINT_POT0);
   posAccu  += analogRead(INPUT_PIN_JOINT_POT1);
   powerAccu+= analogRead(INPUT_PIN_CURRENT   );
   accAccuY += analogRead(INPUT_PIN_JOINT_ACC ); 
   motAccu1 += analogRead(INPUT_PIN_SERVO0_POT);
   motAccu2 += analogRead(INPUT_PIN_SERVO1_POT);
}

/** \brief Interrupt for sending data via the serial.
 *
 * This is initiated by the USART Tx Complete Interrupt, triggered by setting TXCIE0 bit in UCSR0B
 *
 * The data contained in the transmit message buffer is 
 * copied (byte-wise) to the USART data register (UDR0) 
 * and sent via the serial.
 */
ISR(USART_TX_vect) {
  
  // Copy next byte of transmit message buffer to serial register (i.e., send)
  UDR0 = transmit_buffer.bytes[transmit_position++];
  
  // Once we have reached the end of the transmit buffer, switch the TXCIE0 bit of UCSR0B. 
  // This resets the USART control.
  if (transmit_position>=TRANSMIT_LENGTH) {
      UCSR0B &= ~_BV(TXCIE0); 
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
			if (receive_position == RECEIVE_LENGTH) {            // if all bytes have been read, command motors, reset receive_position
				cli();                                        // suspend interrupts
				command_buffer = receive_buffer;              // write received data to command_buffer
				sei();                                        // re-enable interrupts
				receive_position = 0;                         // reset reading position to zero
			}
		}
	}
}
