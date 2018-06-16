#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"

#define DATA_PIN                         1
#define SHIFT_PIN                        3
#define UPDATE_PIN                       0

#define FPS                              64

void ICACHE_FLASH_ATTR update_output();
void ICACHE_FLASH_ATTR turn_all_on(bool on);
void ICACHE_FLASH_ATTR shift_regs();
void ICACHE_FLASH_ATTR write_data_bit(bool high);
void ICACHE_FLASH_ATTR shift_in_bit(bool on);
void ICACHE_FLASH_ATTR display_single_dot(char xPos, char yPos);
void ICACHE_FLASH_ATTR display_full_matrix(long long yellow, long long red);
