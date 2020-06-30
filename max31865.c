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
 * @file    max31865.c
 * @brief   max31865 functions
 * @details Driver code of MAX31865
 *
 * @note    
 */
/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "max31865.h"
#include "spim.h"
#include "gpio.h"
#include "max_spidrv.h"

uint8_t uch_cr,uch_test;    //Use variable at fault detection

//gpio_cfg_t gpio_spim1_cfg;      //Define the ss1 port of SPIM1


/************************************************************************
MAX_31865_WriteRegister used to configuring SS related register status
************************************************************************/
void max31865_writeregister(uint8_t uch_register_address, uint8_t uch_register_value) 
{
	SPIM1_CS_LOW(); //low level output
	SPI_WriteByte(uch_register_address, uch_register_value); //Send Single Byte
	SPIM1_CS_HIGH();//high level output
}

/************************************************************************
MAX_31865_ReadRegister used to read SS related register status
************************************************************************/
uint8_t max31865_readregister(uint8_t uch_register_address)
{
   uint8_t uch_register_data;

 	 SPIM1_CS_LOW();     //low level output
	 uch_register_data = SPI_ReadByte(uch_register_address); //Receive Single Byte
	 SPIM1_CS_HIGH();    //high level output

	 return (uch_register_data);	  	
}

/************************************************************************
maxim_31865_read_nregister used to read SS related multiple register status
************************************************************************/
void max31865_read_nregisters(uint8_t uch_register_address, uint8_t *uch_buff,uint8_t uch_nBytes)
{	
   SPIM1_CS_LOW();    //low level output
	 SPI_Read(uch_register_address, uch_buff, uch_nBytes); //Receive multiple bytes
	 SPIM1_CS_HIGH();   //high level output
}

/************************************************************************
* maxim_31865_init used to initialize MAX31865
************************************************************************/
void max31865_init(max31865_configuration* configuration)
{
	 uint8_t temp=0;
   temp|=configuration->Rtd_wire|configuration->Filter;
	 max31865_writeregister(W_REG_CONFIGURATION_ADDR,temp);
}
/*****************************************************
MAX31865 set fault threshold
******************************************************/
void maxim_set_fault_threshold(float high_threshold,float low_threshold)
{
  //Write the fault threshold register, which can be set as needed
	 max31865_writeregister(W_HIGH_FAULT_THRESHOLD_MSB, high_threshold); //Upper limit
   max31865_writeregister(W_HIGH_FAULT_THRESHOLD_LSB, high_threshold); 
	 max31865_writeregister(W_LOW_FAULT_THRESHOLD_MSB, low_threshold); //Lower limit
	 max31865_writeregister(W_LOW_FAULT_THRESHOLD_LSB, low_threshold);	 
 
}

/*****************************************************
31865 chip stop temperature conversion function
******************************************************/
void max31865_stopconverntion(void)    
{
  uch_cr = max31865_readregister(R_REG_CONFIGURATION_ADDR);
  uch_cr |= Stop_Conversion_Bit;
  max31865_writeregister(W_REG_CONFIGURATION_ADDR, uch_cr);
	maxim_setvbias();
}

/****************************************************
31865 chip start temperature conversion function
*****************************************************/
void max31865_startconverntion(uint8_t uch_conversion_mode, uint8_t fault_decet_mode)  
{
	maxim_setvbias();      //Enable Vbias
	fault_detectioncyclecontrol(fault_decet_mode);  //fault detection
  delay_ms(20);            //At least 10.5 times the time constant + 1 ms
  uch_cr = max31865_readregister(R_REG_CONFIGURATION_ADDR);  //Read configuration register
	uch_cr |= Stop_Conversion_Bit;
	uch_cr |= uch_conversion_mode;
	max31865_writeregister(W_REG_CONFIGURATION_ADDR,uch_cr);   //start convertion
}

/****  clear the fault status of 31865 chip  ****/
void max31865_clearfaultstatus(void)    //clear fault state
{
  uch_cr= max31865_readregister(R_REG_CONFIGURATION_ADDR);
  uch_cr|= Fault_Clear ;
  max31865_writeregister(W_REG_CONFIGURATION_ADDR, uch_cr);
}

/****  enable Vbias  ****/
void maxim_setvbias(void)       //Enable Vbias
{
  uch_cr = max31865_readregister(R_REG_CONFIGURATION_ADDR);
  uch_cr |= Vbias_Enable;
  max31865_writeregister(W_REG_CONFIGURATION_ADDR, uch_cr);
}

/********  disable Vbias  *********/
void maxim_clearvbias(void)
{
  uch_cr = max31865_readregister(R_REG_CONFIGURATION_ADDR);
  uch_cr &= Vbias_Disnable;
  max31865_writeregister(W_REG_CONFIGURATION_ADDR, uch_cr);
}
	
/*****  Loop detection if there are faults  *****/
void fault_detectioncyclecontrol(uint8_t uch_fault_detection_mode)//Fault detection cycle
{
  uch_cr = max31865_readregister(R_REG_CONFIGURATION_ADDR);   //Read configuration register
	uch_test = uch_fault_detection_mode;  //Fault detection mode
	
	/* Confirm if it is manual mode */
	if(uch_test == 0x08)
	{
		delay_ms(10);         //Ensure that Vbias is turned on at least 5 times the time constant
		uch_cr &= Fault_No_Action;
	  uch_cr |= uch_fault_detection_mode;
		max31865_writeregister(W_REG_CONFIGURATION_ADDR,uch_cr);     //Manually detect cycle_1
		delay_ms(10);         //Wait 5 times time constant
		uch_cr &= Fault_No_Action;
		uch_cr |= Fault_Enable_Manual_cl2;   // d2d3Ϊ11
		max31865_writeregister(W_REG_CONFIGURATION_ADDR,uch_cr);     //Manually detect cycle_2
	}
	else
	{
		uch_cr&=Fault_No_Action;
		uch_cr|=uch_fault_detection_mode;
		max31865_writeregister(W_REG_CONFIGURATION_ADDR,uch_cr);     //Automatic detection mode or no action
	}
}
