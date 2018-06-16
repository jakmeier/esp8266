#include "c_types.h"

struct jmr_date {
  unsigned int year   :  6; // 00-63
  unsigned int month  :  4; // 0-15
  unsigned int day    :  5; // 0-31
  unsigned int hour   :  5; // 0-31
  unsigned int minute :  6; // 0-63
  unsigned int second :  6; // 0-63
};

/*                                    
  Parses a date string in the format "2018-06-10T22:22:00+0200" to a 32bit jmr_date 
    @param struct jmr_date* date    The memory locatio nto store the date structure
    @param char* str                The string to be parsed

  *Note*: This function will not work properly for dates whose time shift changes the month
*/
void ICACHE_FLASH_ATTR parse_date(struct jmr_date* date, const char* str);

/*
  Calculates the UNIX timestamp representing the date time of the struct jmr_date
*/
uint32_t ICACHE_FLASH_ATTR get_timestamp(struct jmr_date* date);
