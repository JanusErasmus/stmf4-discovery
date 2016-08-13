#include <cyg/kernel/diag.h>
#include <cyg/kernel/kapi.h>
#include <stdio.h>
#include <stdlib.h>

#include "pwm_port.h"
#include "definitions.h"

#ifdef CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_SOURCE_HSE
#define PWM_TIM3_CLK (((CYGARC_HAL_CORTEXM_STM32_INPUT_CLOCK * CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_MUL) / CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV)  / CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK1_DIV ) * 2
#else
#warning "PWM not supported from this clock source"
#endif

#define PWM_ARR	0xFFFF//0x1F62 = 1kHz	//0xFFFF ~= 125Hz
#define PWM_STEP 0x05
#define PWM_TIME 1

cPWM* cPWM::__instance = NULL;

void cPWM::init()
{
	diag_printf("Initializing cPWM\n");

	if(!__instance)
	{
		__instance = new cPWM();
	}
}

cPWM* cPWM::get()
{
	return __instance;
}

#define PRINT_REG(_base,_offset) diag_printf(#_offset": 0x%08X\n", *((volatile CYG_WORD32 *)(_base + _offset)));

cPWM::cPWM()
{
	mValue = PWM_MIDDLE;

	CYGHWR_HAL_STM32_CLOCK_ENABLE(CYGHWR_HAL_STM32_CLOCK(APB1, TIM3));

	//Use TIM3_CH1 pin to PC6
	mEnablePinSPec = CYGHWR_HAL_STM32_PIN_ALTFN_OUT(B, 0, 2, PUSHPULL, NONE, 2MHZ);
	CYGHWR_HAL_STM32_GPIO_OUT (mEnablePinSPec, 0);
	CYGHWR_HAL_STM32_GPIO_SET (mEnablePinSPec);


	//set Auto Reload register
	mARR = PWM_ARR;
	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_TIM3 + CYGHWR_HAL_STM32_TIM_ARR, mARR);
	//PRINT_REG(CYGHWR_HAL_STM32_TIM3, CYGHWR_HAL_STM32_TIM_ARR);

	//set Capture and compare
	cyg_uint32 reg32 = 0x00;
	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_TIM3 + CYGHWR_HAL_STM32_TIM_CCR3, reg32);
	//PRINT_REG(CYGHWR_HAL_STM32_TIM3, CYGHWR_HAL_STM32_TIM_CCR4);

	reg32 = 15;
	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_TIM3 + CYGHWR_HAL_STM32_TIM_PSC, reg32);

	reg32 = (0x06 <<  4) |//select PWM mode 1 on channel 3
			(0x01 <<  3); //enable Output compare pre load
	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_TIM3 + CYGHWR_HAL_STM32_TIM_CCMR2, reg32);
	//PRINT_REG(CYGHWR_HAL_STM32_TIM3, CYGHWR_HAL_STM32_TIM_CCMR2);


	reg32 = (0x01 << 8);// |	//enable compare channel 3
			//(0x01 << 9) ;	//set channel 1 polarity active low
	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_TIM3 + CYGHWR_HAL_STM32_TIM_CCER, reg32);
	//PRINT_REG(CYGHWR_HAL_STM32_TIM3, CYGHWR_HAL_STM32_TIM_CCER);


	reg32 = (0x01 << 7) |	//enable auto re laod / pre load
			0x01;			//enable counter 3
	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_TIM3 + CYGHWR_HAL_STM32_TIM_CR1, reg32);
	//PRINT_REG(CYGHWR_HAL_STM32_TIM3, CYGHWR_HAL_STM32_TIM_CR1);

}

void cPWM::setValue(cyg_uint32 duty)
{
	int count = 0;
	cyg_int32 delta = 0;

	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_TIM3 + CYGHWR_HAL_STM32_TIM_CCR3, mValue);

	//increase mValue
	if(mValue > duty)
	{
		delta = (-1) * PWM_STEP;
		count = (mValue - duty) / PWM_STEP;
	}
	else
	{
		delta = PWM_STEP;
		count = (duty - mValue) / PWM_STEP;
	}

	//	diag_printf("delta %d, cnt: %d\n", delta, count);
	//	diag_printf("mValue: 0x%04X\n", mValue);

	for(int k = 0; k < count; k++)
	{
		mValue += delta;

		if(mValue > PWM_LEFT)
			mValue = PWM_LEFT;

		if(mValue < PWM_RIGHT)
			mValue = PWM_RIGHT;

		HAL_WRITE_UINT32(CYGHWR_HAL_STM32_TIM3 + CYGHWR_HAL_STM32_TIM_CCR3, mValue);
		cyg_thread_delay(PWM_TIME);
	}
	cyg_thread_delay(100);
	diag_printf("mValue: 0x%04X\n", mValue);

	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_TIM3 + CYGHWR_HAL_STM32_TIM_CCR3, 0);
}

void cPWM::setFrequency(cyg_uint32 f)
{
//	if (f < 125)
//	{
//		diag_printf("cPWM: Lowest frequency with current setup is 125Hz\n");
//		f = 125;
//	}

	mARR = PWM_TIM3_CLK / f;

	//set Auto Reload register
	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_TIM3 + CYGHWR_HAL_STM32_TIM_ARR, mARR);
}

void cPWM::debug(cTerm & t,int argc,char *argv[])
{
	if(!__instance)
		return;

	cyg_uint32 duty = PWM_MIDDLE;

	if(!strcmp(argv[0], "r"))
	{
			duty = PWM_RIGHT;
	}

	if(!strcmp(argv[0], "l"))
	{
			duty = PWM_LEFT;
	}

	diag_printf("duty = 0x%04X\n", duty);
	__instance->setValue(duty);

}
