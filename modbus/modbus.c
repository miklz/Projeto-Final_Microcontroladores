/*
 * modbus.c
 *
 *  Created on: 26 de jun de 2019
 *      Author: Tanaka
 */
#define  F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "avr_usart.h"
#include "modbus.h"

uint16_t CRC16_2(uint8_t *buf, int len)
{
  uint32_t crc = 0xFFFF;
  int i;

  for (i = 0; i < len; i++)
  {
    crc ^= (uint16_t)buf[i];          // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

void transmite_dado(uint16_t dado, uint8_t sensor)
{
	uint8_t i, pkg[8];
	uint16_t crc;

	pkg[0] = 0x15;
	pkg[1] = 0x01;
	pkg[2] = 0;
	pkg[3] = sensor;
	pkg[4] = dado>>8;
	pkg[5] = dado & 0xFF;

	crc = CRC16_2(pkg,6);

	pkg[6] = crc >> 8;
	pkg[7] = crc & 0xff;

	for (i=0; i < 8; i++)
		USART_tx(pkg[i]);
}

uint8_t le_dado(uint8_t adress)
{
	uint8_t i, pkg[8], rx_pkg[16];
	uint16_t crc;
	
	pkg[0] = 0x15;
	pkg[1] = 0x02;
	pkg[2] = 0x00;
	pkg[3] = adress;
	pkg[4] = 0x00;
	pkg[5] = adress;

	crc = CRC16_2(pkg,6);

	pkg[6] = crc >> 8;
	pkg[7] = crc & 0xff;

	for (i=0; i < 8; i++)
		USART_tx(pkg[i]);

	for (i=0; i < 14;i++)
		rx_pkg[i] = USART_rx();
		
	return rx_pkg[5];
}

uint16_t converte_hex_dec(uint8_t valor)
{
	uint16_t a,b;
	a = (valor>>4)*10;
	b = valor & 0x0F;

	return a+b;
}
