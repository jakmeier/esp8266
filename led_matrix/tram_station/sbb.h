//#include "user_config.h"
#include "user_interface.h"
#include "espconn.h"
#include "jmr_espconn.h"

#ifndef JMR_HTTP_HELPER
#define JMR_HTTP_HELPER
#include "jmr_http_helper.h"
#endif

#ifndef JMR_DATE
#define JMR_DATE
#include "jmr_date.h"
#endif

void (*tcp_connected_cb)(void*);
void (*tcp_disconnected_cb)(void*);
void ICACHE_FLASH_ATTR sbb_init();
int ICACHE_FLASH_ATTR sbb_connect();

static char* sbbGetRequest = "GET /v1/connections?from=8591104&limit=1&to=8503003&direct=1&fields[]=connections/from/departure HTTP/1.1\r\nHost: transport.opendata.ch\r\n\r\n";
