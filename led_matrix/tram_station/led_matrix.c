#ifndef LED_MATRIX
#define LED_MATRIX
#include "led_matrix.h"
#endif

static char s_xIndex[] = {4,5,6,7,3,2,1,0};
static char s_yIndex[] = {0,1,2,3,4,5,6,15,8,9,10,11,12,13,14,7}; //red first, yellow afterwards

void ICACHE_FLASH_ATTR shift_in_bit(bool on){
  write_data_bit(on);
  shift_regs();
}

void ICACHE_FLASH_ATTR display_single_dot(char xPos, char yPos) {
  char i;
  // Y Axis is a sink -> LOW = ON
  for(i = 0; i < 16; i++) {
    shift_in_bit(yPos != s_yIndex[i]);
  }
  for(i = 0; i < 8; i++) {
    shift_in_bit(xPos == s_xIndex[i]);
  }
  update_output();
}

void ICACHE_FLASH_ATTR display_full_matrix(long long yellow, long long red) {
  char x,y;
  for(x = 0; x < 8; x++) {
    for(y = 0; y < 8; y++) {
      if(((1LL)<<(x + y*8)) & red) {
       display_single_dot(x,y);
      }
      else{
        turn_all_on(false);
      }
      os_delay_us(1000000/128/FPS);
    }
    for(y = 0; y < 8; y++) {
      if((1LL<<(x + y*8)) & yellow) {
       display_single_dot(x,8+y);
      }
      else{
        turn_all_on(false);
      }
      os_delay_us(1000000/128/FPS);
    }
  }
}

void ICACHE_FLASH_ATTR turn_all_on(bool on){
  // Y Axis is a sink -> LOW = ON
  write_data_bit(!on);
  int i; 
  for(i = 0; i < 16; i++) {
    shift_regs();
  }
  write_data_bit(on); 
  for(i = 0; i < 8; i++) {
    shift_regs();
  }
  update_output();
}

void ICACHE_FLASH_ATTR update_output(){
    gpio_output_set(0, (1 << UPDATE_PIN), 0, 0);
    gpio_output_set((1 << UPDATE_PIN), 0, 0, 0);
}

void ICACHE_FLASH_ATTR shift_regs(){
    gpio_output_set(0, (1 << SHIFT_PIN), 0, 0);
    gpio_output_set((1 << SHIFT_PIN), 0, 0, 0);
}

void ICACHE_FLASH_ATTR write_data_bit(bool high){
  if(high){
    gpio_output_set((1 << DATA_PIN), 0, 0, 0);
  } else {
    gpio_output_set(0, (1 << DATA_PIN), 0, 0);
  }
}
