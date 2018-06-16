#include "main.h"

#define JMR_DEBUG 1

// 0,1,2 allowed as task priority. Only one handler per prio can be registered.
#define MATRIX_DISPLAY_TASK_PRIO         0
#define MATRIX_DISPLAY_LOOP_QUEUE_LEN    1
#define INTERNAL_LED_PIN                 2

enum State {INIT, INIT_DONE, RECONNECTING, TCP_CONNECTED, TCP_DISCONNECTED, TCP_ERROR, WIFI_DISCONNECTED, REGISTRATION_ERROR, UNKNOWN_ERROR, SLEEPING, DEBUG};

static os_timer_t some_timer;
os_event_t MATRIX_DISPLAY_LOOP_QUEUE[MATRIX_DISPLAY_LOOP_QUEUE_LEN];
static uint64_t s_yellow = -1;
static uint64_t s_red = 0;
static int s_timer = 0;
static uint32_t s_departureTimestamp = 0;
static int s_lastUpdate = -99999;
static enum State s_state = INIT;


void ICACHE_FLASH_ATTR user_receive_cb(void* arg, char* pdata, unsigned short len) {
   if(len > 0 && pdata != NULL){
     s_departureTimestamp = sbb_parse_ts_from_http_get_response(arg, pdata, len);
     s_lastUpdate = 0;
   } else {
     s_state = TCP_ERROR;
   }
}

void ICACHE_FLASH_ATTR user_tcp_connected_cb(void* arg) {
  struct espconn* conn = arg;
  int e;

  s_state = TCP_CONNECTED;
  
  e = espconn_regist_recvcb(conn, user_receive_cb);
  if(e) {s_state = REGISTRATION_ERROR;}  
  
  e = sbb_send_get_request(conn);
  if(e) {s_state = REGISTRATION_ERROR;}  
}

void ICACHE_FLASH_ATTR user_tcp_disconnected_cb(void* arg) {
   if(s_state == TCP_CONNECTED){
     s_state = TCP_DISCONNECTED;
     s_red = 0;
   }
}

inline void update_tram_departure_time(){
  int e = sbb_connect();
  if(e && e != ESPCONN_INPROGRESS) { s_state = UNKNOWN_ERROR; }
}

void ICACHE_FLASH_ATTR wifi_event_handler(System_Event_t *evt) {
  switch(evt->event){
    case EVENT_STAMODE_CONNECTED:
      break;
    case EVENT_STAMODE_DISCONNECTED:
      s_state = WIFI_DISCONNECTED;
      wifi_station_connect();
      break;
    case EVENT_STAMODE_GOT_IP:
      s_state = INIT_DONE;
      sbb_init(user_tcp_connected_cb, user_tcp_disconnected_cb); 
      update_tram_departure_time();
      break;
    default: break;
  }
}

void ICACHE_FLASH_ATTR init_wifi_station() {
  const char ssid[32] = "JM-Home-FRITZ-Box";
  const char password[64] = PASSWORD;
  struct station_config stationConf;

  wifi_set_opmode( STATION_MODE );
  stationConf.bssid_set = 0;
  os_memcpy(&stationConf.ssid, ssid, 32);
  os_memcpy(&stationConf.password, password, 64);
  wifi_station_set_config(&stationConf);
  wifi_set_event_handler_cb(wifi_event_handler);
}

void ICACHE_FLASH_ATTR init_clock() {
  ip_addr_t addr;
  sntp_setservername(0,	"pool.ntp.org");
  sntp_setservername(1,	"ntp.metas.ch");
  ipaddr_aton("162.168.178.1", &addr);
  sntp_setserver(2,&addr);
  sntp_set_timezone(0);
  sntp_init();
}

void ICACHE_FLASH_ATTR display_state(int lightsDisplayed){
  switch (s_state){
    case INIT:
      switch (s_timer % 3) {
        case 0: s_yellow = (1LL<<9) + (1LL<<14) + (1LL<<49) + (1LL<<54); break;
        case 1: s_yellow = (1LL<<18) + (1LL<<21) + (1LL<<42) + (1LL<<45); break;
        case 2: s_yellow = (1LL<<27) + (1LL<<28) + (1LL<<35) + (1LL<<36); break;
      } 
       break;
    case INIT_DONE:
      switch (s_timer % 8) {
        case 0: s_yellow = (1LL<<26); break;
        case 1: s_yellow = (1LL<<27); break;
        case 2: s_yellow = (1LL<<28); break;
        case 3: s_yellow = (1LL<<29); break;
        case 4: s_yellow = (1LL<<37); break;
        case 5: s_yellow = (1LL<<36); break;
        case 6: s_yellow = (1LL<<35); break;
        case 7: s_yellow = (1LL<<34); break;
       } 
       break;
    case SLEEPING:
        s_yellow = ((s_timer % 10) < 5) ? (1LL<<27) + (1LL<<28) + (1LL<<35) + (1LL<<36) : 0; 
       break;
    case TCP_CONNECTED:
    case RECONNECTING:
       s_red = (s_timer % 2) ? (1LL<<19) : (1LL<<20);
    case TCP_DISCONNECTED:
       // v v v v v v v v v v v v v v v v v v v v v v Tram station board
       s_yellow = (-1ULL) >> (64 - lightsDisplayed);
       // A A A A A A A A A A A A A A A A A A A A A A  
       break;
    case TCP_ERROR:
       s_red = (1LL<<18) + (1LL<<19) + (1LL<<20) + (1LL<<21)
             + (1LL<<26)                         + (1LL<<29) 
             + (1LL<<34)                         + (1LL<<37)
             + (1LL<<42) + (1LL<<43) + (1LL<<44) + (1LL<<45);
       break;
    case WIFI_DISCONNECTED:
       s_red = (s_timer % 2) ? (1LL<<27) + (1LL<<28) + (1LL<<35) + (1LL<<36) : 0;
       break;
    case UNKNOWN_ERROR:
       s_red = (s_timer % 2) ? (1LL<<10) + (1LL<<13) + (1LL<<34) + (1LL<<37) : 0;
       break;
    case REGISTRATION_ERROR:
       s_red = (s_timer % 2) ? (1LL<<18) + (1LL<<27) : (1LL<<19) + (1LL<<26);
       break;
    case DEBUG:
       break;
    default: // should never happen
      switch (s_timer % 4) {
        case 0: s_red = 1LL<<27; break;
        case 1: s_red = 1LL<<28; break;
        case 2: s_red = 1LL<<35; break;
        case 3: s_red = 1LL<<36; break;
      }
  }
}

void ICACHE_FLASH_ATTR user_loop(void *arg)
{
  s_timer++;
  s_lastUpdate++;
  int lightsOn = 1;
  if(s_lastUpdate >= 0){
    uint32_t nowTS = sntp_get_current_timestamp();
    if(nowTS != 0) {
      lightsOn = ((int64_t)s_departureTimestamp - (int64_t)nowTS) / 10;
      if((lightsOn < 0 || time_to_reconnect()) && is_ready_to_reconnect_tcp()){
        s_state = RECONNECTING;
        update_tram_departure_time();
      }
      if(lightsOn > 64) {
        #ifdef JMR_DEBUG
          s_red = lightsOn; 
        #endif
        s_state = SLEEPING;
      }
      #ifdef JMR_DEBUG
        if(lightsOn < 0) {
          //s_state = DEBUG;
          s_red = nowTS - s_departureTimestamp; 
          lightsOn = 1; 
        }
      #endif
    }
  }
  if(lightsOn < 0) {lightsOn = 0;}
  display_state(lightsOn);
}

inline int time_to_reconnect(){
  int upperTimeLimit = (s_state == SLEEPING) ? SLEEP_WAIT_BETWEEN_CALLS_S : WAIT_BETWEEN_CALLS_S;
  return (s_lastUpdate/2 > upperTimeLimit);
}

inline int is_ready_to_reconnect_tcp(){
  return ( 
   (s_state != RECONNECTING 
     && s_state != INIT 
     && s_state != INIT_DONE
     && s_state != TCP_CONNECTED
     && s_state != DEBUG)
   && s_lastUpdate/2 > MIN_WAIT_BETWEEN_CALLS_S
  );
}

void ICACHE_FLASH_ATTR display_matrix(os_event_t *e) {
  display_full_matrix(s_yellow, s_red);
  system_os_post(MATRIX_DISPLAY_TASK_PRIO, 0, 0);
}

void ICACHE_FLASH_ATTR user_init()
{
  // init gpio subsytem
  gpio_init();

  // configure UART TXD/RXD to be GPIO1/GPIO3, set as output
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_GPIO1); 
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_GPIO3); 
  gpio_output_set(0, 0, (1 << DATA_PIN), 0);
  gpio_output_set(0, 0, (1 << SHIFT_PIN), 0);
  gpio_output_set(0, 0, (1 << UPDATE_PIN), 0);
  gpio_output_set(0, 0, (1 << INTERNAL_LED_PIN), 0);

  // Turn off internal LED (Output = HIGH)
  gpio_output_set((1 << INTERNAL_LED_PIN), 0, 0, 0);

  // setup timer for (500ms, repeating)
  os_timer_setfn(&some_timer, (os_timer_func_t *)user_loop, NULL);
  os_timer_arm(&some_timer, 500, 1);

  // setup Matrix display loop
  system_os_task(display_matrix, MATRIX_DISPLAY_TASK_PRIO, MATRIX_DISPLAY_LOOP_QUEUE, MATRIX_DISPLAY_LOOP_QUEUE_LEN);
  system_os_post(MATRIX_DISPLAY_TASK_PRIO, 0, 0);

  init_wifi_station();
  init_clock();
}


