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
 * $Date: 2018-02-09 12:50:27 -0600 (Fri, 11 Jan 2016) $ 
 * $Revision: Ahuei $
 *
 ******************************************************************************/
 /**
 * @file    max_spidrv.h
 * @brief   max_spidrv functions
 * @details this file is designed for MAX30205 to read the temprature and set different
 *          modes.
 * @note    
 */
#ifndef __MAX_SPIDRV_H
#define __MAX_SPIDRV_H

#ifdef __cplusplus
 extern "C" {
#endif

/**  define the baud rate  **/ 
#define BAUD_RATE           1000000
	 
/**  define the DRDY GPIO port 2_4  **/ 
#define DRDY_GPIO_PORT	    2        
#define DRDY_GPIO_PIN	      PIN_4   
	 
/////////////////////////////////////////////////
#define SPIM1_SS1_GPIO_PORT	              1
#define SPIM1_SS1_GPIO_PIN	              PIN_3
/////////////////////////////////////////////////


/**  define the register address of SPIM1  **/ 
#define spim1_reg_address   0x01

/**  define the fault address  **/ 
#define fault_address       0x07

/***** Functions *****/
/**
 * @brief Spim send initialization
 */
void SPIM_TranslateInit(void);
/**
 * @brief SPI initialization
 *        Success:printf("SPIM Initialized\n")
 *        failed:printf("Error initializing SPIM %d\n", error) 
 */
void SPI_Inital(void);
/**
 * @brief Delay function
 * @param ms   Time required for delay,the unit is ms 
 */
void delay_ms(uint32_t ms);
/**
 * @brief    Set_DRDY_Pin_Input initialization
 *           Success:printf("DRDY_Pin Initialized\n")
 *           failed:printf("Error initializing DRDY_Pin %d\n", error_1) 
 */
void Configure_DRDY_Pin(void);
/**
 * @brief    SRead DRDY pin status
 * @return   DRDY port status
 */
uint32_t Get_DRDYState(void);
/**
 * @brief    SPI communication Configure SS Pin state
 */
void Configure_SS_Pin(void);

/**
 * @brief    SPI communication writes a single byte
 * @param    Write_address     write address to register
 * @param    Write_value       the value written to the register
 */
void SPI_WriteByte(uint8_t Write_address, uint8_t Write_value);
/**
 * @brief    SPI communication reads a single byte
 * @param    Read_address     read address from register
 */
uint8_t SPI_ReadByte(uint8_t Read_address);
/**
 * @brief    SPI communication read multibyte data
 * @param    Read_address     read address from register
 * @param    Read_buff        pointer to auch_read_buff,for memory copy
 * @param    Read_nBytes      read multiple data from register
 */
void SPI_Read(uint8_t Read_address, uint8_t *Read_buff, uint8_t Read_nBytes);

/**
 * @brief    SPI communication Pin output low level
 */
void SPIM1_CS_LOW();

/**
 * @brief    SPI communication Pin output high level
 */
void SPIM1_CS_HIGH();


 #ifdef __cplusplus
}
#endif

#endif
