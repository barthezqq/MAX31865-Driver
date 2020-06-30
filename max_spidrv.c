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
 * @file    max_spidrv.c
 * @brief   max31865 functions
 * @details Driver code of MAX31865
 *
 * @note    
 */

/***** Includes *********/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "spim.h"
#include "gpio.h"
#include "max_spidrv.h"


int error;//SPIM Initialized Fault state
int error_1;//DRDY Fault state

/*** GPIO_DRDY Functional structure ***/
gpio_cfg_t GPIO_DRDY;

gpio_cfg_t gpio_spim1_cfg;

/************************************************************************************/
/*                                           Configuring the SPIM function structure*/
spim_req_t SPIM_SendWrite, SPIM_SendReadWrite, SPIM_SendReadRead_Data, SPIM_SendReadRead_nData;

uint8_t             uch_bytes_value;
uint8_t             auch_dress_value[1],auch_num_value[1];
uint8_t             auch_writebyte[2];
uint8_t             auch_read_buff[8];
uint32_t            timm = 0x00008C;

/* SPIM Send Initialization */
void SPIM_TranslateInit(void)
{
	  //Send Write
		SPIM_SendWrite.len = 2;
	  SPIM_SendWrite.rx_data = NULL;
	  SPIM_SendWrite.tx_data = auch_writebyte;
	  SPIM_SendWrite.width = SPIM_WIDTH_1;	
	
	  //Send read-write address
		SPIM_SendReadWrite.len = 1;
	  SPIM_SendReadWrite.rx_data = NULL;
	  SPIM_SendReadWrite.tx_data = auch_dress_value;
	  SPIM_SendReadWrite.width = SPIM_WIDTH_1;
	
	  //Send read-read single-byte data
		SPIM_SendReadRead_Data.len = 1;
	  SPIM_SendReadRead_Data.rx_data =auch_num_value;
	  SPIM_SendReadRead_Data.tx_data =NULL;
	  SPIM_SendReadRead_Data.width = SPIM_WIDTH_1;
		
		//Send read-read multibyte data
		SPIM_SendReadRead_nData.len = uch_bytes_value;
	  SPIM_SendReadRead_nData.rx_data =auch_read_buff;
	  SPIM_SendReadRead_nData.tx_data =NULL;
		SPIM_SendReadRead_nData.width = SPIM_WIDTH_1;
	}		
/**  SPI communication writes a single byte  **/
void SPI_WriteByte(uint8_t Write_address, uint8_t Write_value)
{
  auch_writebyte[0]=	Write_address;
	auch_writebyte[1]=  Write_value;	

	SPIM_Trans(MXC_SPIM1, &SPIM_SendWrite);
	
	//Delay cs signal line length, package certificate selection read
	while (timm != 0 )
    timm--;
	timm = 0x00008C;      //Ensure cs low when transmitting
	                      //timm can be adjusted according to the actual situation
}	

/**  SPI communication reads a single byte  **/
uint8_t SPI_ReadByte(uint8_t Read_address)
{
   uint8_t Read_data;

	 auch_dress_value[0] = Read_address;
	 SPIM_Trans(MXC_SPIM1, &SPIM_SendReadWrite);	   //send SPIM1 address
	
	 SPIM_Trans(MXC_SPIM1,&SPIM_SendReadRead_Data);  //receive SPIM data	
	 Read_data = auch_num_value[0];  
	 return (Read_data);
}	

/**  SPI communication reads multiple bytes  **/
void SPI_Read(uint8_t Read_address, uint8_t *Read_buff, uint8_t Read_nBytes)
{
	 auch_dress_value[0] =	Read_address;
	 SPIM_Trans(MXC_SPIM1, &SPIM_SendReadWrite);	//send SPIM1 address
	
   uch_bytes_value = Read_nBytes;
	 SPIM_SendReadRead_nData.len = uch_bytes_value;
   SPIM_Trans(MXC_SPIM1,&SPIM_SendReadRead_nData);

   memcpy(Read_buff,auch_read_buff,sizeof(auch_read_buff));
}

/**  SPI initialization  **/
void SPI_Inital(void)
{

	
	//Structure type for configuring a SPIM port
	spim_cfg_t SPIM_Port_InitStructure;

	SPIM_Port_InitStructure.mode = 3;        //mode 3
    SPIM_Port_InitStructure.ssel_pol = SPIM_SSEL1_HIGH;  
    SPIM_Port_InitStructure.baud = BAUD_RATE;//Define transfer baudrate

	//System Configuration Object
	sys_cfg_spim_t Sys_IO_Clk_CFG;

    Sys_IO_Clk_CFG.io_cfg = (ioman_cfg_t)IOMAN_SPIM1(1, 0, 1, 0, 0, 1);
    Sys_IO_Clk_CFG.clk_scale = CLKMAN_SCALE_AUTO;

		
    if((error = SPIM_Init(MXC_SPIM1, &SPIM_Port_InitStructure, &Sys_IO_Clk_CFG)) != E_NO_ERROR) 
		{
        printf("Error initializing SPIM %d\n", error);
        while(1) {}
    } 
		else
		{
			Configure_SS_Pin();//Configure SS Pin
			GPIO_OutSet(&gpio_spim1_cfg);//Enable SS Pin output high level.
			SPIM_TranslateInit();
      printf("SPIM Initialized\n");			
    }
}

/** The delay_ms is 1ms. 
The program is related to the selected CPU and can be adjusted according to the clock. **/
void delay_ms(uint32_t ms)
{
  uint32_t count_num;

  //count_num = 0x000640;
  count_num = 0x000960;
  while(ms)
  {
    while (count_num != 0)
      count_num--;
    count_num = 0x000960;
    ms--;
  }
}

/**  Configure gpio port DRDY  **/
void Configure_DRDY_Pin(void)     
{
	GPIO_DRDY.port = DRDY_GPIO_PORT;
  GPIO_DRDY.mask = DRDY_GPIO_PIN;
  GPIO_DRDY.func = GPIO_FUNC_GPIO;
  GPIO_DRDY.pad  = GPIO_PAD_INPUT; 
	
	if((error_1 = GPIO_Config(&GPIO_DRDY)) != E_NO_ERROR) 
	{
		printf("Error initializing DRDY_Pin %d\n", error_1);
    while(1) {}
  } else 
	{
    printf("DRDY_Pin Initialized\n");
  }    
}

/**  return the state of DRDY  **/
uint32_t Get_DRDYState(void)
{
	return(GPIO_InGet(&GPIO_DRDY));      
}

/**  Configuration SS Pin state  **/
void Configure_SS_Pin(void)
{
	gpio_spim1_cfg.port = SPIM1_SS1_GPIO_PORT;
	gpio_spim1_cfg.mask = SPIM1_SS1_GPIO_PIN;
  gpio_spim1_cfg.func = GPIO_FUNC_GPIO;
  gpio_spim1_cfg.pad  = GPIO_PAD_NORMAL;
  GPIO_Config(&gpio_spim1_cfg);
}

/**  Enable the corresponding pin output low level  **/
void SPIM1_CS_LOW()
{
	GPIO_OutClr(&gpio_spim1_cfg);//Structure pointer contains pin information
}

/**  Enable the corresponding pin output high level  **/
void SPIM1_CS_HIGH()
{
	GPIO_OutSet(&gpio_spim1_cfg);//Structure pointer contains pin information
}
