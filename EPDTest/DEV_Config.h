/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-02-19
* | Info        :
*
* Based on Waveshare's official 5in83_e-Paper_G/ESP32 demo.
* MODIFIED: the GPIO pin block below is repointed from Waveshare's
* demo-board defaults to THIS project's authoritative wiring
* (see MandyWildlife/config.h and the README). Nothing else changed.
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>



/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

/**
 * GPIO config  (AUTHORITATIVE WIRING — Waveshare 5.83" G (HAT) -> ESP32-S3 Mini)
 * ------------------------------------------------------------------
 *   Waveshare demo default   ->   this device
 *   SCK  13   ->   GPIO12   (CLK)
 *   MOSI 14   ->   GPIO11   (DIN)
 *   CS   15   ->   GPIO10
 *   RST  26   ->   GPIO9
 *   DC   27   ->   GPIO13
 *   BUSY 25   ->   GPIO4
 *   PWR  33   ->   GPIO5
 *
 * Do NOT change these to match a library example — the physical
 * wiring is fixed. Do NOT use GPIO8 or GPIO14.
**/
#define EPD_SCK_PIN  12
#define EPD_MOSI_PIN 11
#define EPD_CS_PIN   10
#define EPD_RST_PIN  9
#define EPD_DC_PIN   13
#define EPD_BUSY_PIN 4

/**
 * The 5.83" G HAT exposes a PWR (power-enable) pin. It is wired to
 * GPIO5 on this device, so keep the 9-pin path enabled.
 */
#define D_9PIN  1
#if D_9PIN
    #define EPD_PWR_PIN 5
#endif


#define GPIO_PIN_SET   1
#define GPIO_PIN_RESET 0

/**
 * GPIO read and write
**/
#define DEV_Digital_Write(_pin, _value) digitalWrite(_pin, _value == 0? LOW:HIGH)
#define DEV_Digital_Read(_pin) digitalRead(_pin)

/**
 * delay x ms
**/
#define DEV_Delay_ms(__xms) delay(__xms)

/*------------------------------------------------------------------------------------------------------*/
UBYTE DEV_Module_Init(void);
void GPIO_Mode(UWORD GPIO_Pin, UWORD Mode);
void DEV_SPI_WriteByte(UBYTE data);
UBYTE DEV_SPI_ReadByte();
void DEV_SPI_Write_nByte(UBYTE *pData, UDOUBLE len);
void DEV_Module_Exit(void);

#endif
