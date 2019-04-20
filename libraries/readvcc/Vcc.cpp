/*
  Vcc - A supply voltage measuring library for Arduino

  Created by Ivo Pullens, Emmission, 2014
  
  Inspired by:
  http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Vcc.h"

Vcc::Vcc( const float correction )
  : m_correction(correction)
{
}

#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define ADMUX_VCCWRT1V1 (_BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1))
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
#define ADMUX_VCCWRT1V1 (_BV(MUX5) | _BV(MUX0))
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#define ADMUX_VCCWRT1V1 (_BV(MUX3) | _BV(MUX2))
#else
#define ADMUX_VCCWRT1V1 (_BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1))
#endif  

float Vcc::Read_Volts(void)
{
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  if (ADMUX != ADMUX_VCCWRT1V1)
  {
    ADMUX = ADMUX_VCCWRT1V1;

    // Bandgap reference start-up time: max 70us
    // Wait for Vref to settle.
    delayMicroseconds(350); 
  }
  
  // Start conversion and wait for it to finish.
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA,ADSC)) {};
    
  // Result is now stored in ADC.
  
  // Calculate Vcc (in V)
  float vcc = 1.1*1024.0 / ADC;

  // Apply compensation
  vcc *= m_correction;

  return vcc;
}

float Vcc::Read_Perc(const float range_min, const float range_max, const boolean clip)
{
  // Read Vcc and convert to percentage
  float perc = 100.0 * (Read_Volts()-range_min) / (range_max-range_min);
  // Clip to [0..100]% range, when requested.
  if (clip)
    perc = constrain(perc, 0.0, 100.0);

  return perc;
}

byte* Vcc::Get_Battery_Icon(const int selectedIcon)
{
  if ( selectedIcon > 0 ){
    return Icon[selectedIcon];
  }
  //read the voltage and convert it to volt
  float curvolt = Read_Volts();

  if(curvolt >= 4.99){
    return Icon[0];
  }
  if(curvolt < 4.99 && curvolt > 4.76){
    return Icon[1];
  }
  if(curvolt <= 4.76 && curvolt > 4.53){
    return Icon[2];
  }
  if(curvolt <= 4.53 && curvolt > 4.3){
    return Icon[3];
  }
  if(curvolt <= 4.3 && curvolt > 4.07){
    return Icon[4];
  }
  if(curvolt <= 4.07 && curvolt > 3.84){
    return Icon[5];
  }
  if(curvolt <= 3.84 && curvolt > 3.61){
    return Icon[6];
  }

  return Icon[7];
}

byte Vcc::Icon[9][8] = {
  { // AC
    B01110,
    B11111,
    B10101,
    B10001,
    B11011,
    B11011,
    B11111,
    B11111,
  },
  { // BATT FULL
    B01110,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
  },
  { // 80%
    B01110,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
  },
  { // 60%
    B01110,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
  },
  { // 45%
    B01110,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
  },
  { // 30%
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
  },
  { // 15%
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
  },
  { // 0%
    B01110,
    B10001,
    B10101,
    B10101,
    B10001,
    B10101,
    B10001,
    B11111,
  },
  { // UNK
    B01110,
    B11011,
    B10101,
    B11101,
    B11011,
    B11111,
    B11011,
    B11111,
  }
};