#ifndef JMR_DATE
#define JMR_DATE
#include "jmr_date.h"
#endif

#define MINUTE 60UL
#define HOUR 3600UL
#define DAY 24UL * HOUR
#define YEAR 365UL * DAY
#define LEAP_YEAR (YEAR + DAY)
#define FOUR_YEARS (3UL*YEAR + LEAP_YEAR)
#define START_OF_2000 946684800UL

void ICACHE_FLASH_ATTR parse_date(struct jmr_date* date, const char* str) {
   if(str[0] == '"'){
    str++;
   }
   
   int8_t timeShiftSign = str[19] == '+' ? -1 : str[19] == '-' ? 1 : 0;
   int8_t timeShiftHours =   timeShiftSign * ((str[20]-'0') * 10 + (str[21]-'0')); 
   int8_t timeShiftMinutes = timeShiftSign * ((str[22]-'0') * 10 + (str[23]-'0')); 

   int8_t minutes = (str[14]-'0') * 10 + (str[15]-'0') + timeShiftMinutes;
   int8_t hours = (str[11]-'0') * 10 + (str[12]-'0') + timeShiftHours + minutes / 60;
   if(minutes < 0) {
      minutes += 60;
      hours -=1;
   }
   int8_t days = (str[8]-'0') * 10 + (str[9]-'0') + hours / 24;
   if(hours < 0) {
      hours += 24;
      days -=1;
   }

   date->year = (str[2]-'0') * 10 + (str[3]-'0');
   date->month = (str[5]-'0') * 10 + (str[6]-'0'); // NOT FULLY IMPLEMENTED (TODO?)
   date->day = days; 
   date->hour = hours % 24;
   date->minute = minutes % 60;
   date->second = (str[17]-'0') * 10 + (str[18]-'0');
}

//private
uint32_t days_in_months(uint8_t m, uint8_t y){
  uint32_t result = 0;
  int i;
  for(i = 1; i < m; i++) { 
    switch(i) {
      case 1:
      case 3:
      case 5:
      case 7:
      case 8: 
      case 10: 
      case 12:
         result += 31;
         break;
      case 4:
      case 6:
      case 9:
      case 11:
         result += 30;
         break;
      case 2:
         result += y%4 == 0 ? 29 : 28;
    }
  }
  return result;
}

uint32_t ICACHE_FLASH_ATTR get_timestamp(struct jmr_date* date){
  uint32_t time = START_OF_2000;
  time += date->second;  
  time += date->minute * MINUTE;  
  time += date->hour * HOUR;  
  time += (date->day-1) * DAY;  
  time += days_in_months(date->month, date->year) * DAY;  
  time += (date->year % 4) * YEAR;
  time += (date->year % 4) ? DAY : 0; // Additional day for leap year  
  time += (date->year / 4) * FOUR_YEARS;
  return time;
}
