#include "c_types.h"
#include "osapi.h"
/*
   @param const char* rawJson      A JSON string. Must be NULL termintated.
   @param const char* key          The key to look for. Must be NULL termintated.
   @return const char*             Points to the start of the value that belongs to the given key inside the provided JSON
*/
const char* ICACHE_FLASH_ATTR find_first_json_value_flat(const char* rawJson, const char* key);

/*
   @param const char* httpResponse A string representing the resonse from a HTTP GET Request 
   @return const char*             Points to the start of the response body inside the provided string
*/
const char* ICACHE_FLASH_ATTR find_body(const char* httpResponse);
