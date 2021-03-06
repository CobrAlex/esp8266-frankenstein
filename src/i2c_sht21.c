#include "driver/i2c_master.h"
#include "driver/i2c_sht21.h"
#include "console.h"

#ifdef CONFIG_CMD_SHT21_DEBUG
#define dbg(fmt, ...) LOG(LOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define dbg(fmt, ...)
#endif

bool ICACHE_FLASH_ATTR
SHT21_Init()
{
	if(i2c_master_writeRegister(SHT21_ADDRESS, SHT21_SOFT_RESET, 0))
	{
		// The soft reset takes less than 15ms.
		os_delay_us(SHT21_SOFT_RESET_TIME*1000);
		return true;
	}
	return false;
}

bool ICACHE_FLASH_ATTR
SHT21_Read()
{

	LAST_SHT_TEMPERATURE = i2c_master_readRegister16wait(SHT21_ADDRESS, SHT21_TRIGGER_TEMP_MEASURE_NOHOLD, true) & ~3;
	LAST_SHT_HUMIDITY = i2c_master_readRegister16wait(SHT21_ADDRESS, SHT21_TRIGGER_HUMD_MEASURE_NOHOLD, true) & ~3;
	return true;

	//TODO read data in hold mode
	//LAST_SHT_TEMPERATURE = i2c_master_readRegister16(SHT21_ADDRESS, SHT21_TRIGGER_TEMP_MEASURE_HOLD) & ~3;
	//LAST_SHT_HUMIDITY = i2c_master_readRegister16(SHT21_ADDRESS, SHT21_TRIGGER_TEMP_MEASURE_NOHOLD) & ~3;
	//return true;
}
/*
	LAST_SHT_TEMPERATURE = -46.85 + 175.72 / 65536.0 * (float)temp;
	LAST_SHT_HUMIDITY = -6.0 + 125.0 / 65536.0 * (float)humi;

*/

static int do_i2c_sht21(int argc, const char* const* argv)
{
	if(argc == 1 || strcmp(argv[1], "read") == 0){

		if(SHT21_Read()){
			console_printf( argc == 1 ? "%d %d\n" : "Temperature: %d\nHumidity: %d\n", LAST_SHT_TEMPERATURE, LAST_SHT_HUMIDITY);
		}else{
			console_printf( "failed read value\n" );
		}
	} else

	if(strcmp(argv[1], "init") == 0){

		console_printf( SHT21_Init() ? "Ok\n":"Failed\n" );
	} 

	return 0;
}

CONSOLE_CMD(i2c_sht21, 0, 2, 
		do_i2c_sht21, NULL, NULL, 
		"I2C humidity sensor SHT21"
		HELPSTR_NEWLINE "i2c_sht21 init"
		HELPSTR_NEWLINE "i2c_sht21 [read]"
);
