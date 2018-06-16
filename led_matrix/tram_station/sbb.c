#ifndef SBB
#define SBB
#include "sbb.h"
#endif

struct espconn myConnection;
struct _esp_tcp myTcp;
uint32_t sbbServerIp = 0; 
int sbbError = 0;

void ICACHE_FLASH_ATTR sbb_init(void (*super_tcp_connected_cb)(void*), void (*super_tcp_disconnected_cb)(void*)){
  tcp_connected_cb = super_tcp_connected_cb;
  tcp_disconnected_cb = super_tcp_disconnected_cb;
}

int ICACHE_FLASH_ATTR sbb_tcp_connect(struct espconn* conn){
  myTcp.local_port = espconn_port();
  myTcp.remote_port = 80;
  struct ip_info ipconfig;
  wifi_get_ip_info(STATION_IF, &ipconfig);
  if(*((int*)(&ipconfig.ip)) == 0) {
    return ESPCONN_CONN;
  }
  os_memcpy(myTcp.local_ip, &ipconfig.ip, 4);
  
  conn->type = ESPCONN_TCP;
  conn->state = ESPCONN_NONE;
  conn->proto.tcp = &myTcp;
  
  os_memcpy(myTcp.remote_ip, &sbbServerIp, 4);

  char e;
  e = espconn_regist_connectcb(conn, tcp_connected_cb);
  if(e){return e;}

  e = espconn_regist_disconcb(conn, tcp_disconnected_cb);
  if(e){return e;}

  e = espconn_connect(conn);
  return e;
}

void ICACHE_FLASH_ATTR sbb_ip_found(const char* name, ip_addr_t* ipaddr, void* arg){
  struct espconn* conn = arg;
  if(ipaddr == 0) { 
     sbbError = JMR_ESPCONN_DNS_ERROR;
     return;
  } 
  sbbServerIp = *(uint32_t*)ipaddr;
  sbb_tcp_connect(&myConnection);
}

int ICACHE_FLASH_ATTR sbb_send_get_request(struct espconn* conn) {
  return espconn_send(conn, sbbGetRequest, os_strlen(sbbGetRequest));
}

uint32_t ICACHE_FLASH_ATTR sbb_parse_ts_from_http_get_response(void* arg, char* pdata, unsigned short len) {
  const char* body = find_body(pdata);

  //"2018-06-10T19:15:00+0200"
  const char* dateString = find_first_json_value_flat(body, "departure");
  struct jmr_date date;
  parse_date(&date, dateString);
  return get_timestamp(&date);
}

int ICACHE_FLASH_ATTR sbb_connect() {
  int e;
  e = espconn_gethostbyname(&myConnection,"transport.opendata.ch", (ip_addr_t *)&sbbServerIp, sbb_ip_found);
  if(e==ESPCONN_INPROGRESS){
    // /* the ip will be received in the callback */  
    // while(sbbServerIp == 0 && sbbError == 0){
    //    os_delay_us(10000);
    // }
    // if(sbbError) {  
    //   e = sbbError;
     //  sbbError = 0;
         return e;
     //}
  }
  else if(e != ESPCONN_OK){
    return e;
  }
  return sbb_tcp_connect(&myConnection);
}

