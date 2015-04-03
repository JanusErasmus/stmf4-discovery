#include <cyg/kernel/kapi.h>
#include <cyg/kernel/diag.h>
#include <stdio.h>

#include "temp_probe.h"

tempProbe::tempProbe(cyg_uint32 adc_pinspec)
{
	mIdx = 0;
	mTempValid = false;

	CYGHWR_HAL_STM32_GPIO_SET(adc_pinspec);
	CYGHWR_HAL_STM32_CLOCK_ENABLE(CYGHWR_HAL_STM32_CLOCK(APB2, ADC1));

	cyg_thread_delay(100);
	cyg_uint32 reg32;

	HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_SR, reg32);
	diag_printf("SR: 0x%08X\n", reg32);


	//	HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR2, reg32);
	reg32 =
			CYGHWR_HAL_STM32_ADC_CR2_ADON |		//switch on
			0;
	HAL_WRITE_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR2, reg32);

	reg32 =
			CYGHWR_HAL_STM32_ADC_CR1_DISCEN |		//switch on
			0;
	HAL_WRITE_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR1, reg32);

	HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR1, reg32);
	//diag_printf("CR1: 0x%08X\n", reg32);

	HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR2, reg32);
	//diag_printf("CR2: 0x%08X\n", reg32);


	reg32 = 1 |	//select channel 1 as 1st sequence
			0;
	HAL_WRITE_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_SQR3, reg32);

	for(cyg_uint8 k = 0; k < 32; k++)
		mBuff[k] = 0x989;
}

void tempProbe::sample()
{
	cyg_uint32 reg32;

	HAL_WRITE_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_SR, 0x00);

	HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR2, reg32);
	reg32 	|=
			CYGHWR_HAL_STM32_ADC_CR2_SWSTART |		//start conversion
			0;
	HAL_WRITE_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR2, reg32);

	do
	{
		HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_SR, reg32);
	}
	while(!(reg32 & CYGHWR_HAL_STM32_ADC_SR_EOC));

	HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_DR, reg32);

	//diag_printf("ADC 0x%08X\n", reg32);

	mBuff[mIdx] = reg32;
	//diag_printf("%d:0x%04X\n", adcIdx, adcBuff[adcIdx]);

	if(++mIdx >= 32)
	{
		mTempValid = true;
		mIdx = 0;
	}

}


double tempProbe::getTemp()
{
	if(!mTempValid)
		return -1;

	cyg_uint32 adcSum = 0;
	for(cyg_uint8 k = 0; k < 32; k++)
	{
		adcSum += mBuff[k];
	}

	float adcAvg = (float)(adcSum >> 5);

	return (adcAvg * 0.122073125) - 276.0;
}

tempProbe::~tempProbe()
{
}

