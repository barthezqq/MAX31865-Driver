/*******************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 * $Date: 2019-04-16 12:50:27 -0600 (Tues, 16 Apr 2016) $  
 * $Revision: Ahuei $
 *
 ******************************************************************************/
 /**
 * @file    max30605.h
 * @brief   max30605 functions
 * @details this file is designed for MAX30205 to read the temprature and set different
 *          modes.
 * @note    
 */

#ifndef __MAX31865_H
#define __MAX31865_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct
{
 uint8_t   Vbias;
 uint8_t   Conversion_mode;
 uint8_t   Rtd_wire;
 uint8_t   Filter;
}max31865_configuration;	 

/*****************************************************************************************/
/*                                                  Write Register Address Configuration */
#define W_REG_CONFIGURATION_ADDR          0x80    // MAX31865 write register Configuration address 0x80
#define W_HIGH_FAULT_THRESHOLD_MSB        0x83    // MAX31865 write register High Fault Threshold MSB  address 0x83
#define W_HIGH_FAULT_THRESHOLD_LSB        0x84    // MAX31865 write register High Fault Threshold LSB  address 0x84
#define W_LOW_FAULT_THRESHOLD_MSB         0x85    // MAX31865 write register Low Fault Threshold MSB  address 0x85
#define W_LOW_FAULT_THRESHOLD_LSB         0x86    // MAX31865 write register Low Fault Threshold LSB  address 0x86

/*****************************************************************************************/
/*                                                   Read Register Address Configuration */
#define R_REG_CONFIGURATION_ADDR          0x00    // MAX31865 read register Configuration address 0x00
#define R_HIGH_FAULT_THRESHOLD_MSB        0x03    // MAX31865 read register HIGH_FAULT_THRESHOLD address 0x03
#define R_HIGH_FAULT_THRESHOLD_LSB        0x04    // MAX31865 read register HIGH_FAULT_THRESHOLD_LSB address 0x04
#define R_LOW_FAULT_THRESHOLD_MSB         0x05    // MAX31865 read register LOW_FAULT_THRESHOLD_MSB address 0x05
#define R_LOW_FAULT_THRESHOLD_LSB         0x06    // MAX31865 read register LOW_FAULT_THRESHOLD_LSB address 0x06

/*******************************************************************************/
/*                                           Define ss1 chip select signal 1.3 */
//#define SPIM1_SS1_GPIO_PORT	              1
//#define SPIM1_SS1_GPIO_PIN	              PIN_3

/*******************************************************/
/*                 Temperature calculation coefficient */
#define   A                               0.0039083     
#define   B                               0.0000005775
#define   Para_C                          0.00000000000418301 // C has been used
//register value PT100
#define   REF_RES                         400
#define   R0                              100
/*
//PT1000 reference resistance
#define         REF_RES                   4000
#define         R0                        1000
*/

//Corresponding to configuration Register D4
#define   RTD_Wire_2or4              (uint8_t) 0x00
#define   RTD_Wire_3                 (uint8_t) 0x10

//Corresponding to configuration Register D0
#define   NRF_60Hz_Bit               (uint8_t) 0x00
#define   NRF_50Hz                   (uint8_t) 0x01

#define   Stop_Conversion_Bit        (uint8_t) 0x60 
#define   One_Shot_Conversion        (uint8_t) 0x20 
#define   Automatic_Conversion       (uint8_t) 0x40 

/* Fault detection cycle mode */
//Corresponding to configuration register D3,D2
#define   Fault_No_Action            (uint8_t) 0xF3 
#define   Fault_Enable_Auto          (uint8_t) 0x04
#define   Fault_Enable_Manual_cl1    (uint8_t) 0x08
#define   Fault_Enable_Manual_cl2    (uint8_t) 0x0C

#define   Fault_Clear                (uint8_t) 0x02

/* State of BIAS,enable or disable */
//Corresponding to configuration register D7
#define   Vbias_Enable               (uint8_t) 0x80
#define   Vbias_Disnable             (uint8_t) 0x00

/* Fault type */
//Corresponding to fault Status register
#define  RTD_High_Fault              (uint8_t) 0x80  //RTD High Threshold
#define  RTD_Low_Fault               (uint8_t) 0x40  //RTD Low Threshold
#define  REFIN_n_Fault               (uint8_t) 0x20  //REFIN- > 0.85 x VBIAS
#define  FocBre_REFIN_n_Fault        (uint8_t) 0x10  //REFIN- < 0.85 x VBIAS (FORCE- open)
#define  FocBre_RTDIN_n_Fault        (uint8_t) 0x08  //RTDIN- < 0.85 x VBIAS (FORCE- open)
#define  OVUV_Fault                  (uint8_t) 0x04  //Overvoltage/undervoltage fault
#define  NO_Fault                    (uint8_t) 0x00  
#define  RTD_Arb_Fault               (uint8_t) 0x01  //Arbitrary fault

//#define  UPPER_LIMIT                 (uint8_t) 0xFF
//#define  LOWER_LIMIT                 (uint8_t) 0x00

/***** Functions *****/
/**
 * @brief Initilize 31865
 */
void max31865_init(max31865_configuration* configuration);
/**
 * @brief Stop temperature conversion
 */
void max31865_stopconverntion(void);
/**
 * @brief Start temperature conversion
 * @param uch_conversion_mode  the conversion mode 
 * @param fault_decet_mode     the fault mode
 */
void max31865_startconverntion(uint8_t uch_conversion_mode, uint8_t fault_decet_mode);
/**
 * @brief clear the fault state
 */
void max31865_clearfaultstatus(void);
/**
 * @brief fault detection control
 * @param uch_fault_detection_mode   the fault mode at cycle control dection 
 */
void fault_detectioncyclecontrol(uint8_t uch_fault_detection_mode);
/**
 * @brief  enable Vbias
 */
void maxim_setvbias(void);
/**
 * @brief  disable Vbias
 */
void maxim_clearvbias(void);
/**
 * @brief SPI write a single byte
 * @param uch_register_address       address written to the register  
 * @param uch_register_value         value written to the register
 */
void max31865_writeregister(uint8_t uch_register_address, uint8_t uch_register_value);
/**
 * @brief SPI read a single byte
 * @param uch_register_address       register address for reading data 
 */
uint8_t max31865_readregister(uint8_t uch_register_address);
/**
 * @brief SPI read n bytes data
 * @param uch_register_address       register address for reading data  
 * @param uch_buff                   pointer to SPI_Read communication for memory copy
 * @param uch_nBytes                 read mutiple data from the register
 */
void max31865_read_nregisters(uint8_t uch_register_address, uint8_t *uch_buff,uint8_t uch_nBytes);

/**
 * @brief Set temperature fault threshold
 * @param high_threshold             Upper temperature threshold 
 * @param low_threshold              Lower temperature threshold
 */
void maxim_set_fault_threshold(float high_threshold, float low_threshold);


 #ifdef __cplusplus
}
#endif

#endif
