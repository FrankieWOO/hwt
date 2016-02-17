/** 
 * \file maccepa2dof.pde
 * \author Matthew Howard (MH), matthew.howard@ed.ac.uk
 * \date 27/12/10 13:08:53 GMT
 * \ingroup MACCEPA2DOF
 *
 * \brief Arduino program for controlling/reading sensors of the 2-DOF MACCEPA
 * using interrupts via a serial connection. 
 *
 * Use this in combination with the mex or python interfaces to read
 * sensors/send commands and test the robot's response.
 *
 */
#include "defines.h"


#ifdef VARIABLE_DAMPING
/** 
 * \brief Wire library for I2C (two wire control interface) 
 * \sa http://arduino.cc/en/Reference/Wire 
 */
#include <Wire.h>
/** 
 * \brief Address for I2C transmissions. 
 * \note Format of this byte is 0101 A0 A1 A2, where A0,...,A2 are address pins on DS1807 (A0,...,A2).
 */
#define ADDRESS 0x28 // assume address 000 (i.e., A0,...,A2 all grounded).
#endif

union {
	struct {
		/** \brief Servo0 command. */
		unsigned int u0;
		/** \brief Servo1 command. */
		unsigned int u1;
		/** \brief Servo2 command. */
		unsigned int u2;
		/** \brief Servo3 command. */
		unsigned int u3;
#ifdef VARIABLE_DAMPING
		/** \brief Damper0 command.*/
		unsigned int u4;
		/** \brief Damper1 command.*/
		unsigned int u5;
#endif
#ifdef MAGNET
		/** \brief Magnet command.*/
		unsigned int u6;
#endif
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
		/** \brief Base joint potentiometer reading. */
		unsigned int q0;
		/** \brief Second joint potentiometer reading. */
		unsigned int q1;
		/** \brief Servo0 potentiometer reading. */
		unsigned int m0;
		/** \brief Servo1 potentiometer reading. */
		unsigned int m1;
		/** \brief Servo2 potentiometer reading. */
		unsigned int m2;  
		/** \brief Servo3 potentiometer reading. */
		unsigned int m3;
#ifdef CURRENT_SENSING
		/** \brief Servo0 current sensor reading. */
		unsigned int i0;
		/** \brief Servo1 current sensor reading. */
		unsigned int i1;
		/** \brief Servo2 current sensor reading. */
		unsigned int i2;
		/** \brief Servo3 current sensor reading. */
		unsigned int i3;
#endif
	};
	unsigned char bytes[TRANSMIT_LENGTH];
} transmit_buffer;

/** \brief Temporary variable for storing bytes read from the serial. */
unsigned char input;

/** \brief Variable for keeping track of which position in receive struct we are reading from. */
unsigned char receive_position;

/** \brief Variable for keeping track of which position in transmit struct we are sending from. */
unsigned char transmit_position;

/** \brief Sensor accumulator. Each sensor is read at 4x the control frequency. This array contains the sum of these 4 readings, for each sensor. Reading the sensors this way has a filtering effect, that smoothes out noise. */
int y_acc[DIMY];

int i;

/** 
 * \brief Initialisation (called after initial upload or reset button pressed).
 * Sets up serial parameters and communication buffers. Sets up pin modes for
 * controlling servos. Also calls setup_timers() to set up the timers used for
 * PWM.
 */
void setup() {
  delay(100);
  Serial.begin(BAUDRATE);     // Set the serial speed
  Serial.write(0x01);         // Write a bit to the serial using the inbuilt Serial library

  transmit_position=0;                                              // Initialise transmit_position, transmit_buffer
  for(i=0;i<TRANSMIT_LENGTH;i++) { transmit_buffer.bytes[i]=0x00; } // (headers are fixed, body set to zeros).
  transmit_buffer.bytes[0] = TRANSMIT_HEADER0;   
  transmit_buffer.bytes[1] = TRANSMIT_HEADER1;

  for(i=0;i<DIMY;i++) { y_acc[i]=0; } // Initialise sensor accumulators

  receive_position=0; // Initialise receive_position, receive_buffer 
  receive_buffer.u0 =  U_INIT_SERVO0; command_buffer.u0 =  U_INIT_SERVO0; // Set initial command for servo0
  receive_buffer.u1 =  U_INIT_SERVO1; command_buffer.u1 =  U_INIT_SERVO1; // Set initial command for servo1
  receive_buffer.u2 =  U_INIT_SERVO2; command_buffer.u2 =  U_INIT_SERVO2; // Set initial command for servo2
  receive_buffer.u3 =  U_INIT_SERVO3; command_buffer.u3 =  U_INIT_SERVO3; // Set initial command for servo3
#ifdef VARIABLE_DAMPING
  receive_buffer.u4 = U_INIT_DAMPER0; command_buffer.u4 = U_INIT_DAMPER0; // Set initial command for damper0
  receive_buffer.u5 = U_INIT_DAMPER1; command_buffer.u5 = U_INIT_DAMPER1; // Set initial command for damper1
#endif
#ifdef MAGNET
  receive_buffer.u6 = U_INIT_MAGNET ; command_buffer.u6 = U_INIT_MAGNET ; // Set initial command for magnet
#endif
  
  // set up PWM pins
  digitalWrite(OUTPUT_PIN_SERVO0, LOW); pinMode(OUTPUT_PIN_SERVO0, OUTPUT); // Ground the Servo0 pin, and set as an output.
  digitalWrite(OUTPUT_PIN_SERVO1, LOW); pinMode(OUTPUT_PIN_SERVO1, OUTPUT); // Ground the Servo1 pin, and set as an output.
  digitalWrite(OUTPUT_PIN_SERVO2, LOW); pinMode(OUTPUT_PIN_SERVO2, OUTPUT); // Ground the Servo2 pin, and set as an output.
  digitalWrite(OUTPUT_PIN_SERVO3, LOW); pinMode(OUTPUT_PIN_SERVO3, OUTPUT); // Ground the Servo3 pin, and set as an output.
#ifdef MAGNET
  pinMode(OUTPUT_PIN_MAGNET, OUTPUT); digitalWrite(OUTPUT_PIN_MAGNET,U_INIT_MAGNET); // Write to the magnet pin, and set as an output.
  //digitalWrite(OUTPUT_PIN_MAGNET, U_INIT_MAGNET); pinMode(OUTPUT_PIN_MAGNET, OUTPUT); // Write to the magnet pin, and set as an output.
  //digitalWrite(OUTPUT_PIN_MAGNET, HIGH); pinMode(OUTPUT_PIN_MAGNET, OUTPUT); // Write to the magnet pin, and set as an output.
#endif

#ifdef VARIABLE_DAMPING
  // set up wire (for communication to digipot)
  Wire.begin();
#endif

  setup_timers();                       // Set up internal timers/interrupts
}

/** 
 * \brief Set up timers for PWM output to motors (Timer1 and Timer3) and sampling
 * of the ADCs connected to sensors (Timer2). Also configure which interrupts are
 * enabled on these timers.
 */
void setup_timers() {

  uint8_t oldSREG = SREG; // Store status register (SREG)

  cli();                  // Suspend interupts while we set up the registers

  // Set up Timer1 counter control registers (TCCR1A/TCCR1B).
  //  - set WGM11, WGM12, WGM13 -> Fast PWM with ICR1 as TOP (i.e., Wave Generation Mode 14 - see Table 15-4 in datasheet).
  //  - set CS11 -> clock select bit is set to clc_i/o / 8 (see Table 16-6 in datasheet).
  //  - set COM1A1/COM1B1/COM1C1, unset COM1A0/COM1B0/COM1C0 -> OCR1A toggled on Compare Match, disconnect OCR1B/OCR1C (see Table 16-4 in datasheet).  
  TCCR1A  = _BV(WGM11) 
          &~_BV(COM1A0)| _BV(COM1A1) 
          &~_BV(COM1B0)| _BV(COM1B1)
          &~_BV(COM1C0)| _BV(COM1C1);
  TCCR1B  = _BV(WGM13) |  _BV(WGM12)  
          | _BV(CS11); 
  
  // Set up ICR1 - this is the value at which the clock resets.
  ICR1    = clockCyclesPerMicrosecond()*(20000L/8) - 1; // 20000 uS is a bit fast for the refresh, 20ms, but 
                                                        // it keeps us from overflowing ICR1 at 20MHz clocks
                                                        // That "/8" at the end is the prescaler.

  // Set up Timer1 Interrupt Mask Register - controls which interrupts are enabled on Timer1.
  TIMSK1 &= ~_BV(OCIE1A);   // Disable Timer 1 Compare Match A (OCIE1A) interrupt (not used) 
  TIMSK1 &= ~_BV(OCIE1B);   // Disable Timer 1 Compare Match B (OCIE1B) interrupt (not used) 
  TIMSK1 &= ~_BV(OCIE1C);   // Disable Timer 1 Compare Match C (OCIE1C) interrupt (not used) 
  TIMSK1 |=  _BV(TOIE1);    // Enable Timer 1 Overflow (TOIE1) interrupt

  OCR1A   = U_INIT_SERVO0;   // Set initial value to OCR1A register (that determines the PWM signal to Servo 0).
  OCR1B   = U_INIT_SERVO1;   // Set initial value to OCR1B register (that determines the PWM signal to Servo 0).

  // ------------------------------ //

  // Set up Timer3 counter control registers (TCCR3A/TCCR3B).
  //  - set WGM11, WGM12, WGM13 -> Fast PWM with ICR1 as TOP (i.e., Wave Generation Mode 14 - see Table 15-4 in datasheet).
  //  - set CS11 -> clock select bit is set to clc_i/o / 8 (see Table 16-6 in datasheet).
  //  - set COM1A1/COM1B1/COM1C1, unset COM1A0/COM1B0/COM1C0 -> OCR1A toggled on Compare Match, disconnect OCR1B/OCR1C (see Table 16-4 in datasheet).  
  TCCR3A  = _BV(WGM31) 
          &~_BV(COM3A0)| _BV(COM3A1) 
          &~_BV(COM3B0)| _BV(COM3B1)
          &~_BV(COM3C0)| _BV(COM3C1);
  TCCR3B  = _BV(WGM33) |  _BV(WGM32)  
          | _BV(CS31); 
  
  // Set up ICR1 - this is the value at which the clock resets.
  ICR3    = clockCyclesPerMicrosecond()*(20000L/8) - 1; // 20000 uS is a bit fast for the refresh, 20ms, but 
                                                        // it keeps us from overflowing ICR1 at 20MHz clocks
                                                        // That "/8" at the end is the prescaler.

  // Set up Timer3 Interrupt Mask Register - controls which interrupts are enabled on Timer 3.
  TIMSK3 &= ~_BV(OCIE3A);   // Disable Timer 3 Compare Match A (OCIE3A) interrupt (not used) 
  TIMSK3 &= ~_BV(OCIE3B);   // Disable Timer 3 Compare Match B (OCIE3B) interrupt (not used) 
  TIMSK3 &= ~_BV(OCIE3C);   // Disable Timer 3 Compare Match C (OCIE3C) interrupt (not used) 

  OCR3A   = U_INIT_SERVO2;  // Set initial value to OCR1A register (that determines the PWM signal to Servo 0).
  OCR3B   = U_INIT_SERVO3;  // Set initial value to OCR1B register (that determines the PWM signal to Servo 0).

  // ------------------------------ //

  // Set up Timer2. This timer is used to sample readings from the sensors (analogue pins).
  
  ASSR   &= ~_BV(AS2);      // Set clock source (to internal I/O clock, not asynchronous clock)
  
  // Set up Timer2 counter control registers (TCCR2A/TCCR2B).
  TCCR2A  = 0;              // Set Timer 2 to 'normal mode' (i.e., a counter)
  TCCR2B |= _BV(CS22);      // Set prescaler for Timer 2 to clk_T2S/256 (see Table 19-9). 
  TCCR2B |= _BV(CS21);      // 

  // Set up Timer2 Interrupt Mask Register - controls which interrupts are enabled on Timer 2.
  TIMSK2  =  _BV(TOIE2);    // Enable Timer 2 Overflow (TOIE2) interrupt
  TIMSK2 &= ~_BV(OCIE2A);   // Disable Compare Match A (OCIE2A) interrupt (not used)

  SREG    = oldSREG;  // undo cli()
}

/** 
 * \brief Timer1 Overflow interrupt. This is initiated when Timer1 is
 * reset (every 20000 ticks, i.e., every 20 milliseconds).
 * 
 * This triggers the following events:
 *   \li Motor commands are copied from the command buffer to the servos.
 *   \li Sensor readings are copied from the accumulators to the transmit buffer.
 *   \li Timer2 and Timer3 are reset (to ensure synchronisation).
 *   \li The accumulators are reset.
 *   \li The serial transmitter is enabled (triggering the USART Tx Complete interrupt).
 */
ISR(TIMER1_OVF_vect) {
    // apply commands to servos
    OCR1A = command_buffer.u0;   
    OCR1B = command_buffer.u1;   
    OCR3A = command_buffer.u2;   
    OCR3B = command_buffer.u3;
#ifdef VARIABLE_DAMPING
    // apply commands to damper motors
    sei();                               // Set interrupts (these are temporarily suspended as part of the ISR, but we need them for the wire library)
    Wire.beginTransmission(ADDRESS);     // open communication
    Wire.send(0xA9);                     // command word: "write pot0 and pot1 values"
    Wire.send(command_buffer.bytes[ 8]); // write damper0 pot value
    Wire.send(command_buffer.bytes[10]); // write damper1 pot value    
    Wire.endTransmission();              // send (flush buffer) and close communication
#endif
#ifdef MAGNET
    // apply command to magnet
    digitalWrite(OUTPUT_PIN_MAGNET,command_buffer.u6);
//    digitalWrite(OUTPUT_PIN_MAGNET,HIGH);
#endif

    // record sensor readings
    transmit_buffer.q0 = y_acc[0];  
    transmit_buffer.q1 = y_acc[1];  
    transmit_buffer.m0 = y_acc[2];  
    transmit_buffer.m1 = y_acc[3];  
    transmit_buffer.m2 = y_acc[4];  
    transmit_buffer.m3 = y_acc[5];
#ifdef CURRENT_SENSING
    transmit_buffer.i0 = y_acc[6];  
    transmit_buffer.i1 = y_acc[7];  
    transmit_buffer.i2 = y_acc[8];  
    transmit_buffer.i3 = y_acc[9];
#endif

    // synchronise timers
    TCNT3 = 0; // reset timer 3
    TCNT2 = 0; // reset timer 2 
    
    // reset accumulators
    for(i=0;i<DIMY;i++) { y_acc[i]=0; }
    
    // send readings via serial
    transmit_position = 0; // reset the transmit position
    UCSR0B |= _BV(TXCIE0); // Switch on the TXCIE0 bit of the USART control and status register (UCSR0B)).
                           // This triggers the USART Transmit Complete Interrupt (USART_TX_vect)
}

/**
 * \brief Timer2 Overflow interrupt. This is initiated when Timer2 is reset
 * (every 5000 ticks, i.e., every 5 milliseconds).
 *
 * This triggers samples to be read from the analogue pins (ADCs).
 */
ISR(TIMER2_OVF_vect) {   
   y_acc[0] += analogRead(INPUT_PIN_JOINT0_POT);
   y_acc[1] += analogRead(INPUT_PIN_JOINT1_POT);
   y_acc[2] += analogRead(INPUT_PIN_SERVO0_POT);
   y_acc[3] += analogRead(INPUT_PIN_SERVO1_POT);
   y_acc[4] += analogRead(INPUT_PIN_SERVO2_POT);
   y_acc[5] += analogRead(INPUT_PIN_SERVO3_POT);
#ifdef CURRENT_SENSING
   y_acc[6] += analogRead(INPUT_PIN_SERVO0_CURRENT_SENSOR);
   y_acc[7] += analogRead(INPUT_PIN_SERVO1_CURRENT_SENSOR);
   y_acc[8] += analogRead(INPUT_PIN_SERVO2_CURRENT_SENSOR);
   y_acc[9] += analogRead(INPUT_PIN_SERVO3_CURRENT_SENSOR);   
#endif
}

/** 
 * \brief USART Tx Complete Interrupt. 
 *
 * This copies the sensor data in the transmit buffer for serial transmission. When all bytes have been copied, the transmitter is switched off.
 */
ISR(USART0_TX_vect) {
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
			if (receive_position == RECEIVE_LENGTH) {         // if all bytes have been read, command motors, reset receive_position
				cli();                                        // suspend interrupts
				command_buffer = receive_buffer;              // write received data to command_buffer
				sei();                                        // re-enable interrupts
				receive_position = 0;                         // reset reading position to zero
			}
		}
	}
}
