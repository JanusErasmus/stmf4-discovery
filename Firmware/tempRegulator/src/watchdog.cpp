#include <cyg/kernel/diag.h>

#include "watchdog.h"

void cWatchDog::start()
{
    cyg_uint32 reg32;

    //switch on LSI
    HAL_WRITE_UINT32(CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_CSR, 0x01);
    do
    {
        HAL_READ_UINT32(CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_CSR, reg32);
        //diag_printf("RCC_CSR: 0x%08X\n", reg32);
        //cyg_thread_delay(100);
    }while(!(reg32 & 0x02));
    //diag_printf("RCC_CSR: 0x%08X\n", reg32);

    //diag_printf("Watchdog started\n");
    //Start watchdog timer
    HAL_WRITE_UINT32(CYGHWR_HAL_STM32_IWDG + CYGHWR_HAL_STM32_IWDG_KR, 0xCCCC);
    HAL_WRITE_UINT32(CYGHWR_HAL_STM32_IWDG + CYGHWR_HAL_STM32_IWDG_KR, 0x5555);

    //wait for
    do
    {
        HAL_READ_UINT32(CYGHWR_HAL_STM32_IWDG + CYGHWR_HAL_STM32_IWDG_SR, reg32);
        //diag_printf("IWD_SR: 0x%08X\n", reg32);
        //cyg_thread_delay(100);
    }while(reg32 & 0x01);
    //diag_printf("IWD_SR -> 0x%08X\n", reg32);

    HAL_WRITE_UINT32(CYGHWR_HAL_STM32_IWDG + CYGHWR_HAL_STM32_IWDG_PR, 0x07);

    do
    {
        HAL_READ_UINT32(CYGHWR_HAL_STM32_IWDG + CYGHWR_HAL_STM32_IWDG_SR, reg32);
        //diag_printf("IWD_SR: 0x%08X\n", reg32);
        //cyg_thread_delay(100);
    }while(reg32 & 0x01);
    //diag_printf("IWD_SR: 0x%08X\n", reg32);

//    HAL_READ_UINT32(CYGHWR_HAL_STM32_IWDG + CYGHWR_HAL_STM32_IWDG_PR, reg32);
//    diag_printf("IWD_PR: 0x%08X\n", reg32);
//    HAL_READ_UINT32(CYGHWR_HAL_STM32_IWDG + CYGHWR_HAL_STM32_IWDG_RLR, reg32);
//    diag_printf("IWD_RLR: 0x%08X\n", reg32);
}

void cWatchDog::kick()
{
    HAL_WRITE_UINT32(CYGHWR_HAL_STM32_IWDG + CYGHWR_HAL_STM32_IWDG_KR, 0xAAAA);
}






