extern "C"{
      #include<ets_sys.h>
      #include<osapi.h>
      #include<os_type.h>
      #include<gpio.h>
      #include<user_interface.h>
      #include<espconn.h>
      #include<mem.h>
}

char *psent = "Yeah its working";

void initDone();
void initTCP();
void eventCB(System_Event_t *event);
void connect_CB(void *arg);

struct softap_config config;
struct espconn conn1;
esp_tcp tcp1;

void connectCB(void *arg) {
struct espconn *pNewEspConn = (struct espconn *)arg;
printf("Connection Established");
Serial.println(espconn_send(&conn1,reinterpret_cast <unsigned char*>(psent),20));
Serial.println(espconn_regist_sentcb(&conn1, sentCB));
}

void initTCP() {
          conn1.type = ESPCONN_TCP;
          conn1.state = ESPCONN_NONE;
          conn1.proto.tcp = &tcp1;
          conn1.proto.tcp->local_port = 8008; 
          espconn_accept(&conn1);

          Serial.println(reinterpret_cast<char*>(psent));
          uint16_t len_gth = sizeof(*psent);
          Serial.println(len_gth);
          
          Serial.println(espconn_accept(&conn1)); 
          //espconn_regist_connectcb(&conn1, connectCB);
          Serial.println(espconn_regist_connectcb(&conn1, connectCB));
          }

void sentCB(void *arg) {
          Serial.println("send");
} 

void initDonecb(void) {
          wifi_set_opmode_current(SOFTAP_MODE);
          strcpy(reinterpret_cast<char*>(config.ssid),"ESP8266");
          strcpy(reinterpret_cast<char*>(config.password),"password");
          config.ssid_len = 0;
          config.authmode = AUTH_OPEN;
          config.ssid_hidden = 0;
          config.max_connection = 4;
          wifi_softap_set_config_current(&config);
          wifi_wps_enable(WPS_TYPE_PBC);
          wifi_wps_start();
          wifi_softap_dhcps_start();
          wifi_set_event_handler_cb(wifi_handle_event_cb);
}


void wifi_handle_event_cb(System_Event_t *evt)
{
          uint8 stationCount = wifi_softap_get_station_num();
          printf("stationCount = %d\n", stationCount);
          struct station_info *stationInfo = wifi_softap_get_station_info();
          if (stationInfo != NULL) {
                  while (stationInfo != NULL) {
                                printf("Station IP: %d.%d.%d.%d\n", IP2STR(&(stationInfo->ip)));
                                stationInfo = STAILQ_NEXT(stationInfo, next);
                                }
                  wifi_softap_free_station_info();
                  }
                  
          switch (evt->event) {
                  case EVENT_SOFTAPMODE_STACONNECTED:{
                       printf("station: " MACSTR "join, AID = %d\n", MAC2STR(evt->event_info.sta_connected.mac),evt->event_info.sta_connected.aid);
                       initTCP();}
                       break;
                  case EVENT_SOFTAPMODE_STADISCONNECTED:
                       printf("station: " MACSTR "leave, AID = %d\n", MAC2STR(evt->event_info.sta_disconnected.mac),evt->event_info.sta_disconnected.aid);
                       break;
                  default:
                       //printf("Unknown event");
                       break;
               }
}

 
void user_rf_pre_init(void){
}

void setup(){
  Serial.begin(115200);
  initDonecb();           
}

void loop(){
}



