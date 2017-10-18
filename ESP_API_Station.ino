extern "C"{
  #include<ets_sys.h>
  #include<osapi.h>
  #include<os_type.h>
  #include<gpio.h>
  #include<user_interface.h>
  #include<espconn.h>
  #include<mem.h>
}

#define remote_ipaddress "192.168.4.1"

struct espconn conn1;
esp_tcp tcp1;
struct ip_info ipinfo;

#define AP_NETIF 0x01

void connectCB(void *arg) {
        struct espconn *pNewEspConn = (struct espconn *)arg;
        Serial.println("Connection Established");
}

void inittcp() {
          conn1.type = ESPCONN_TCP;
          conn1.state = ESPCONN_NONE;
          conn1.proto.tcp = &tcp1;
                   
          wifi_get_ip_info(AP_NETIF,&ipinfo);
          Serial.println(ipinfo.ip.addr);
          
          *((uint32 *)conn1.proto.tcp->remote_ip) = ipaddr_addr(remote_ipaddress);
          conn1.proto.tcp->remote_port = 8008;
          conn1.proto.tcp->local_port = espconn_port();
           
          espconn_connect(&conn1);
          Serial.println(espconn_connect(&conn1));
          espconn_regist_connectcb(&conn1, connectCB);
          Serial.println(espconn_regist_connectcb(&conn1, connectCB));
          espconn_regist_recvcb(&conn1, recvCB);
          Serial.println(espconn_regist_recvcb(&conn1, recvCB));
          Serial.print("Listening for data");
}

void recvCB(void *arg, char *pData, unsigned short len) {
          //Serial.println("recieve");
          Serial.println("recvcb");
          struct espconn *pEspConn = (struct espconn *)arg;
          printf("Received data!! - length = %d\n", len);
          int i=0;
          for (i=0; i<len; i++) {
          printf("%c", pData[i]);
          }
          Serial.println("\n");
} 

void initDone() {
          wifi_set_opmode_current(STATION_MODE);
          struct station_config stationConfig;
          strncpy(reinterpret_cast<char*>(stationConfig.ssid), "ESP8266", 32);
          strncpy(reinterpret_cast<char*>(stationConfig.password),"password", 64);
          wifi_station_set_config(&stationConfig);
          wifi_set_event_handler_cb(wifi_handle_event_cb);
}

void wifi_handle_event_cb(System_Event_t *evt)
{               
   switch (evt->event) {
        case EVENT_STAMODE_CONNECTED:{
           printf("connect to ssid %s, channel %d\n", evt->event_info.connected.ssid, evt->event_info.connected.channel);
           }
           break;
        case EVENT_STAMODE_DISCONNECTED:
           printf("disconnect from ssid %s, reason %d\n", evt->event_info.disconnected.ssid, evt->event_info.disconnected.reason);
           break;
        case EVENT_STAMODE_AUTHMODE_CHANGE:
           printf("mode: %d -> %d\n", evt->event_info.auth_change.old_mode, evt->event_info.auth_change.new_mode);
           break;
        case EVENT_STAMODE_GOT_IP:
           {
              printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR, IP2STR(&evt->event_info.got_ip.ip), IP2STR(&evt->event_info.got_ip.mask),IP2STR(&evt->event_info.got_ip.gw));
              Serial.println("\n");
              inittcp();
           }
           break;
 }
}

 
void user_rf_pre_init(void){
  
}

void setup(){
  Serial.begin(115200);         
  initDone();           
}



void loop(){

}



