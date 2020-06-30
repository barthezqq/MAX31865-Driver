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
 * @file    main.c
 * @brief   SPI MAX31865 Example
 * @details This example implemented Read and Write functions of MAX31865 through SPI interface 
 *          with MAX32625-EVKIT board.
 *          P1.0(MCU): SPIM1 SCK
 *          P1.1(MCU): SPIM1 MOSI
 *          P1.2(MCU): SPIM1 SISO
 *          P1.3(MCU): SPIM1 SS(CS)
 *          P2.4(MCU): SPIM1 DRDY
 *          VDD and GND
 *          //Connect pull up resister with power source for SPI bus by using JP23 and JP24 jumpers on MAX32625-EVKIT board.
 *          //Please note to check D8 and D7 first, D8 and D7 are indicators for power good.
 * @note    //Other devices may share same SPI bus, further configuration is required if multidevice are on the bus
 *          
 */

/***** Includes *********/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "mxc_config.h"
#include "clkman.h"
#include "ioman.h"
#include "spim.h"
#include "spis.h"
#include "gpio.h"
#include "max_spidrv.h"
#include "max31865.h"    

/***** Define the variables you need to use *****/
uint8_t               uch_reg_buffer[8];
uint32_t              temperature_value;
uint8_t               uch_fault_detection;
uint8_t               uch_RTDh, uch_RTDl;
uint32_t              ADC_Code;
float                 f_temperature_read;
float                 f_Rt;
float                 f_ADC_value;
uint32_t              DRDYdata = 1;
uint32_t              Normaldata = 16;

#define Temp_threshold               0x2000

max31865_configuration  configuration;

/*********************** Functions **************************/
int main(void)
{			
  /*Initialization*/
	SPI_Inital();// Initialize SPI, use SPIM1

	/* 31865 configuration register initialization configuration */
	configuration.Rtd_wire  =  RTD_Wire_2or4;
    configuration.Filter    =  NRF_50Hz;
	max31865_init(&configuration);
 
	Configure_DRDY_Pin();// Set DRDY corresponding pin as input

	/* Set fault threshold high and low */
	maxim_set_fault_threshold(400,0);

	/*Read the conversion temperature in a cyclic manner*/
	while (1)
	{
		//Set two conversion delays, 0.5s each time
		delay_ms(5000);

		//Enable single conversion
        max31865_startconverntion(One_Shot_Conversion, Fault_Enable_Auto);
		
		//Waiting for conversion to end
		while((DRDYdata = Get_DRDYState()) == Normaldata);
 
		max31865_read_nregisters(spim1_reg_address, uch_reg_buffer, 2);  //Read conversion data
		
		uch_RTDh = uch_reg_buffer[0];                                    //Assign the read result to the corresponding register
		uch_RTDl = uch_reg_buffer[1];
		uch_fault_detection = max31865_readregister(fault_address);      //Read fault status
		
		max31865_stopconverntion();

		
		if((uch_RTDl & 0x01) == NO_Fault)
		{
			//Calculate the conversion measurement temperature
			ADC_Code = (uch_RTDh << 8 | uch_RTDl) >> 1;
			f_ADC_value = ADC_Code;
			f_Rt = f_ADC_value*REF_RES/32768.0f;

			if(ADC_Code > Temp_threshold)
			{
				f_temperature_read = (-A+sqrt(A*A+4*B*(1-f_Rt/R0)))/(-2*B); //pow(A,2)
			}else
			{
				f_temperature_read=(0-R0*A+sqrt(pow(R0,2)*pow(A,2)-4*R0*B*(R0-f_Rt)))/(2*R0*B);
			}
			printf("RTD temperature is:%8.1f\n", f_temperature_read); 				
		}
		else
		{
			printf("Fault Exist\n");

			//Output shows the type of fault 
			if((uch_fault_detection & RTD_High_Fault) == RTD_High_Fault) 
				printf("RTD High Fault\n");			
    	if((uch_fault_detection & RTD_Low_Fault) == RTD_Low_Fault)
				printf("RTD Low Fault\n");	
    	if((uch_fault_detection & REFIN_n_Fault) == REFIN_n_Fault)
				printf("REFIN- Voltage Fault\n");
    	if((uch_fault_detection & FocBre_REFIN_n_Fault) == FocBre_REFIN_n_Fault)
				printf("When Force- Break, REFIN- Voltage Fault\n");
			if((uch_fault_detection & FocBre_RTDIN_n_Fault) == FocBre_RTDIN_n_Fault)
				printf("When Force- Break, RTDIN- Voltage Fault\n");
			if((uch_fault_detection & OVUV_Fault) == OVUV_Fault)
				printf("Overvoltage or Undervoltage Input Fault\n");
		}
		max31865_clearfaultstatus();//Clear fault status
  	}
}
