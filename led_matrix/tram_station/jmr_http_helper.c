#ifndef JMR_HTTP_HELPER
#define JMR_HTTP_HELPER
#include "jmr_http_helper.h"
#endif

int ICACHE_FLASH_ATTR str_equal(const char* a, const char* b, int len){
  int i;
  for(i = 0; i < len; i++){
    if(a[i] != b[i]) { return 0; }
  }
  return -1;
}

const char* ICACHE_FLASH_ATTR find_first_json_value_flat(const char* rawJson, const char* key){
  const char* p = rawJson;
  int keyLen = os_strlen(key);
  while(*p){
     if(*p == '"'){
       p++;
       if(str_equal(p, key, keyLen)){
         return p + keyLen + 2;
       }
     }
     p++;
  }
}

const char* ICACHE_FLASH_ATTR find_body(const char* httpResponse){
  const char * p = httpResponse;
  while(*p){
    if(*p == '\r'){
      p++;
      if(*p == '\n'){
        p++;
        if(*p == '\r'){
          p++;
          if(*p == '\n'){
            return p+1;
          }
        }
      }
    }
    p++;
  }
  return 0;
}
