//=======================================================================================================
//
//                   TITLE:               DS1302 Real Time Clock
//                   FILENAME:            DS1302_RTC.h
//                   PREPARED FOR:        Virtuabotic, LLC
//                   PROGRAMMER(S):       Arduino.cc user "Krodal"
//                                        Joseph Datillo
//                                        T. Daniel Murphy
//                   DEVELOPMENT DATE:    06 January 2014
//                   COMPILER USED:       Arduino 1.0.5 and xCode 5.0.2 (Version 5A3005)
//                   TARGET PLATFORM:     Versilino and Arduino Microcontrollers
//
//=======================================================================================================
//                                        Program Description Begin
//=======================================================================================================
//
//  This header file contains the class declarations for the employment of a DS1302 Real Time Clock
//  library.  This file is written for compatibility for Arduino Boards, as well as Versilino boards.
//  These functions will work on any pin type available on the boards (PWM, COMMUNICATION, DIGITAL, or
//  ANALOG).
//
//                                 This is the Revision Section Begin
//=======================================================================================================
//
//    Date                  Programmer                  Revisions Made
//
//    XX June 2012          Krodal                      Original
//    XX March 2013         Krodal                      Added bcd2bin, bin2bcd_h, and bin2bcd_l functions
//    07 January 2014       Joseph Datillo              Converted file into library format.
//                          T. Daniel Murphy            Added functions to allow for user implentation
//                                                        as a library, vice a stand alone program.
//                                                      Added ability to access individual elements of
//                                                        the time structure.
//                                                      Added functions for printing, and element access.
//
//                                        Acknowledgements Begin
//=======================================================================================================
//  The original program was provided by Arduino.cc user Kodal on URL:
//  http://playground.arduino.cc/Main/DS1302
//
//                                            Reference Links Begin
//=======================================================================================================

//  Datasheet:  http://www.hobbytronics.co.uk/datasheets/DS1302.pdf
//  GET UPDATES: https://www.virtuabotix.com/resources.php?product=versalino

//=======================================================================================================
//                                         Include Guards Begin
//=======================================================================================================
#include "stdio.h"

#ifndef DS1302_RTC_H
#define DS1302_RTC_H

#include <stddef.h>
#include <string.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include <pins_arduino.h>
#endif


//=======================================================================================================
//			                             DS1302_RTC Class Declaration Begin
//=======================================================================================================
//  DS1302_RTC - Creates the Real Time Clock object, and contains all of the functions that are
//                   required for setting, and tracking the time, as well as converting that time into
//                   the current UNIX Time.

class DS1302_RTC
{
  public:
    //+++++++++++++++++++++++++++++++++++++++++++++ Class Constructor +++++++++++++++++++++++++++++++++++++++
    DS1302_RTC(uint8_t inSCLK, uint8_t inIO, uint8_t inC_E);

    //++++++++++++++++++++++++++++++++++++++++++++++ Class Functions ++++++++++++++++++++++++++++++++++++++++
    void    initRTC(uint8_t CLK, uint8_t IO, uint8_t ENABLE);        // Sets the pins and enable them
    void    clock_burst_read(uint8_t *p);                            // Reads clock data, and sets pinmode
    void    clock_burst_write(uint8_t *p);                           // Writes clcok data, and sets pinmode
    uint8_t _read(int address);                                   // Reads a byte from DS1302, sets pinmode
    void    _write(int address, uint8_t data);                       // Writes a byte to DS1302, sets pinmode
    void    _start(void);                                            // Function to help setup start condition
    void    _stop(void);                                             // Function to help stop the communication
    uint8_t toggleread(void);                                    // Function to help read byte with bit
    void    togglewrite(uint8_t data, uint8_t release);             // Function to help wrtie byte with bit
    void    setDS1302Time(uint8_t seconds, uint8_t minutes,          // This function sets the time on the
                       uint8_t hours, uint8_t dayofweek,          // DS1302
                       uint8_t dayofmonth, uint8_t month,
                       int year);
    void updateTime();                                            // This function simply updates the time

    uint8_t read(const uint8_t addr);                             // This reads a register
    //++++++++++++++++++++++++++++++++++++++++++++++ Class variables ++++++++++++++++++++++++++++++++++++++++
    uint8_t SCLK;
    uint8_t IO;
    uint8_t C_E;
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t dayofweek;
    uint8_t dayofmonth;
    uint8_t month;
    int year;
};

#endif // versalinoRTC_H                         
