#include <cyg/hal/hal_io.h>
#include <cyg/kernel/diag.h>

#include "stm32cpu.h"

 void stm32cpu::info()
 {
	 diag_printf("CPU Information:\n");

	 cyg_uint64 id = stm32cpu::uniqueID();
	 diag_printf(" - Unique ID: 0x%08X%08X\n", (cyg_uint32)(id >> 32) , (cyg_uint32)(id & 0xFFFFFFFF));

	 diag_printf(" - %6d kB FLASH\n", stm32cpu::flashSize());
	 diag_printf(" - %6d B RAM\n", stm32cpu::ramSize());

	 cyg_uint32 reg32;
	 HAL_READ_UINT32( CYGHWR_HAL_STM32_MCU_ID, reg32);
	 if(CYGHWR_HAL_STM32_MCU_ID_DEV(reg32) == CYGHWR_HAL_STM32_MCU_ID_DEV_MEDIUM)
		 diag_printf(" - Medium Density Line\n");
	 else if(CYGHWR_HAL_STM32_MCU_ID_DEV(reg32) == CYGHWR_HAL_STM32_MCU_ID_DEV_HIGH)
		 diag_printf(" - High Density Line\n");
	 else if(CYGHWR_HAL_STM32_MCU_ID_DEV(reg32) == 0x430)
		 diag_printf(" - XL Density Line\n");
	 else if(CYGHWR_HAL_STM32_MCU_ID_DEV(reg32) == 0x418)
		 diag_printf(" - Connectivity Line\n");

	 diag_printf(" - Revision: 0x%04X\n", CYGHWR_HAL_STM32_MCU_ID_REV(reg32));

 }

 cyg_uint64 stm32cpu::uniqueID()
{
	cyg_uint64 reg64;
	cyg_uint32 reg32;
	HAL_READ_UINT32( 0x1FFFF7E8, reg32);
	HAL_READ_UINT32( 0x1FFFF7E8 + 4, reg32);
	reg64 = reg32;
	HAL_READ_UINT32( 0x1FFFF7E8 + 8, reg32);
	reg64 |= (cyg_uint64)reg32 << 32;

	return reg64;
}

cyg_uint16 stm32cpu::flashSize()
{
	cyg_uint32 reg32;
	HAL_READ_UINT32(CYGHWR_HAL_STM32_DEV_SIG, reg32);

	return CYGHWR_HAL_STM32_DEV_SIG_FSIZE(reg32);
}

cyg_uint16 stm32cpu::ramSize()
{
	cyg_uint32 reg32;
	HAL_READ_UINT32(CYGHWR_HAL_STM32_DEV_SIG, reg32);

	return CYGHWR_HAL_STM32_DEV_SIG_RSIZE(reg32);
}

void stm32cpu::sysResetStatus()
{
	cyg_uint32 reg32;

	HAL_READ_UINT32(CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_CSR, reg32);

//	if(reg32 & CYGHWR_HAL_STM32_RCC_CSR_PINRSTF)
//	{
//		diag_printf("Reset from NRST pin\n");
//	}

	if(reg32 & CYGHWR_HAL_STM32_RCC_CSR_SFTRSTF)
	{
		diag_printf(" - Reset from Software\n");
	}
	if(reg32 & CYGHWR_HAL_STM32_RCC_CSR_IWDGRSTF)
	{
		diag_printf(" - Reset from Independent Watchdog\n");
	}
	if(reg32 & CYGHWR_HAL_STM32_RCC_CSR_WWDGRSTF)
	{
		diag_printf(" - Reset from Window Watchdog\n");
	}
	if(reg32 & CYGHWR_HAL_STM32_RCC_CSR_LPWRRSTF)
	{
		diag_printf(" - Reset from Low power \n");
	}


	//Clear reset flags
	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_CSR, CYGHWR_HAL_STM32_RCC_CSR_RMVF);
}

void stm32cpu::pStatus(cTerm & term, int argc,char * argv[])
{
	term << term.format("Peripheral power status\n");

		cyg_uint32 reg32 = 0;
		HAL_READ_UINT32(CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_AHB1ENR, reg32);
		term << term.format(" AHB1 peripheral clock enable register:\n  - 0x%08X\n", reg32);
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_AHB1ENR_BKPSRAM))
		{
			term << term.format("   - Back Up Interface enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOA))
		{
			term << term.format("   - GPIOA enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOB))
		{
			term << term.format("   - GPIOB enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOC))
		{
			term << term.format("   - GPIOC enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOD))
		{
			term << term.format("   - GPIOD enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOE))
		{
			term << term.format("   - GPIOE enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOF))
		{
			term << term.format("   - GPIOF enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_AHB1ENR_OTGHS))
		{
			term << term.format("   - OTG HS enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_AHB1ENR_OTGHSULPI))
		{
			term << term.format("   - OTG HS ULPI enabled\n");
		}

		HAL_READ_UINT32(CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_AHB2ENR, reg32);
		term << term.format(" AHB2 peripheral clock enable register:\n  - 0x%08X\n", reg32);
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_AHB2ENR_OTGFS))
		{
			term << term.format("   - OTG FS enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_AHB2ENR_RNG))
		{
			term << term.format("   - Random Number generator enabled\n");
		}

		HAL_READ_UINT32(CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_APB1ENR, reg32);
		term << term.format(" APB1 peripheral clock enable register:\n  - 0x%08X\n", reg32);
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_TIM2))
		{
			term << term.format("   - Timer2 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_TIM3))
		{
			term << term.format("   - Timer3 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_TIM4))
		{
			term << term.format("   - Timer4 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_TIM5))
		{
			term << term.format("   - Timer5 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_TIM6))
		{
			term << term.format("   - Timer6 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_TIM7))
		{
			term << term.format("   - Timer7 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_WWDG))
		{
			term << term.format("   - Window Watchdog enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_SPI2))
		{
			term << term.format("   - SPI2 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_SPI3))
		{
			term << term.format("   - SPI3 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_UART2))
		{
			term << term.format("   - UART2 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_UART3))
		{
			term << term.format("   - UART3 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_UART4))
		{
			term << term.format("   - UART4 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_UART5))
		{
			term << term.format("   - UART5 enabled\n");
		}
		if(reg32 & (CYGHWR_HAL_STM32_RCC_APB1ENR_I2C1))
		{
			term << term.format("   - I2C1 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_I2C2))
		{
			term << term.format("   - I2C2 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_CAN1))
		{
			term << term.format("   - CAN1 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_CAN2))
		{
			term << term.format("   - CAN2 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_PWR))
		{
			term << term.format("   - Power Interface  enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB1ENR_DAC))
		{
			term << term.format("   - DAC enabled\n");
		}

		HAL_READ_UINT32(CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_APB2ENR, reg32);
		term << term.format(" APB2 peripheral clock enable register:\n  - 0x%08X\n", reg32);
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB2ENR_SPI1))
		{
			term << term.format("   - SPI1 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB2ENR_UART1))
		{
			term << term.format("   - UART1 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB2ENR_UART6))
		{
			term << term.format("   - UART6 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB2ENR_TIM1))
		{
			term << term.format("   - TIM1 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB2ENR_TIM8))
		{
			term << term.format("   - TIM8 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB2ENR_TIM9))
		{
			term << term.format("   - TIM9 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB2ENR_TIM10))
		{
			term << term.format("   - TIM10 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB2ENR_TIM11))
		{
			term << term.format("   - TIM11 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB2ENR_ADC1))
		{
			term << term.format("   - ADC1 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB2ENR_ADC2))
		{
			term << term.format("   - ADC2 enabled\n");
		}
		if(reg32 & (1 << CYGHWR_HAL_STM32_RCC_APB2ENR_ADC3))
		{
			term << term.format("   - ADC3 enabled\n");
		}
}
