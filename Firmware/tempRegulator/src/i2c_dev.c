
#include <cyg/kernel/kapi.h>
#include <cyg/kernel/diag.h>
#include <cyg/io/i2c.h>
#include <cyg/io/i2c_stm32.h>


extern cyg_i2c_bus stm_i2c_bus0;
//extern cyg_i2c_bus stm_i2c_bus1;

//Senserion sensor
CYG_I2C_DEVICE(senserion,
            &stm_i2c_bus0,
            0x80,
            0x00,
            10);

//STM sensor
CYG_I2C_DEVICE(HTS_humidity,
            &stm_i2c_bus0,
            0xBE,
            0x00,
            10);

CYG_STM32_I2C_BUS(stm_i2c_bus0,
        cyg_stm32_i2c_init,
        CYGHWR_HAL_STM32_I2C1,
        CYGNUM_HAL_INTERRUPT_I2C1_EV,
        7,
        CYGHWR_HAL_STM32_I2C1_CLOCK,
        100000);
