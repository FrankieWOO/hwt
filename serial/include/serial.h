/** \file serial.h
    \brief Simple serial port library for Windows and Linux, written in plain C.

	(C) 2008 Stefan Klanke
*/

#ifndef __SERIAL_H
#define __SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef WIN32

#include <windows.h>

typedef struct {
   HANDLE comPort;
   DCB oldDCB;
   COMMTIMEOUTS oldTimeOuts;
} SerialPort;

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

typedef struct {
   int comPort;
   struct termios oldTermios;
} SerialPort;

#endif

int serialOpenByNumber(SerialPort *SP, int port);
/** 
 * \brief Open serial port by name. 
 * \param[out] SP Serial port struct.
 * \param[in] name Name of serial port device (e.g. /dev/ttyUSB0).
 * \return 1 if opening is successful, 0 otherwise.
*/
int serialOpenByName(SerialPort *SP, const char *name);
int serialClose(SerialPort *SP);
/** 
 * \brief Write to serial port. 
 * \param[in] SP Serial port struct.
 * \param[in] size Size of buffer (?).
 * \param[in] buffer Pointer to data buffer.
 * \return 1 if write is successful, 0 otherwise.
*/
int serialWrite(SerialPort *SP, int size, void *buffer);
/** 
 * \brief Read from serial port. 
 * \param[in] SP Serial port struct.
 * \param[in] size Size of buffer (?).
 * \param[out] buffer Pointer to data buffer.
 * \return 1 if read is successful, 0 otherwise.
*/
int serialRead(SerialPort *SP, int size, void *buffer);
/** 
 * \brief Set serial port parameters. 
 * \param[in] SP Serial port struct.
 * \param[in] baudrate Baudrate.
 * \param[in] bits No. data bits.
 * \param[in] parity Parity (0 or 1).
 * \param[in] stops No. stop bits (1 or 2).
 * \param[in] timeout Timeout (in units of 0.1 sec).
 * \return 1 if setting is successful, 0 otherwise.
*/
int serialSetParameters(SerialPort *SP, int baudrate, int bits, int parity, int stops, int timeout);
void serialFlushInput(SerialPort *SP);
void serialFlushOutput(SerialPort *SP);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
