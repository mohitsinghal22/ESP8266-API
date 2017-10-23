extern "C"{
      #include<ets_sys.h>
      #include<osapi.h>
      #include<os_type.h>
      #include<gpio.h>
      #include<user_interface.h>
      #include<espconn.h>
      #include<mem.h>
}

#define remote_ipaddress "192.168.4.2"  //Ipaddress of station provided by AP for TCP connection 2

#define AP_NETIF 0x01

char *psent = "Yeah its working first connection";  // Sending data to Station 1 TCP CLIENT

void initDone();        //Function used to initialize ESP8266 Parameters
void initTCP();         //Function used to initialize TCP Parameters
void eventCB(System_Event_t *event);      //Events Callback
void connect_CB(void *arg);               //Connection callback after establishment of TCP connection

// Declaretion for TCP Connection 1
struct softap_config config;        
struct espconn conn1;
esp_tcp tcp1;

// Declaretion for TCP Connection 2
struct espconn conn2;
esp_tcp tcp2;
struct ip_info ipinfo;

//Connection Callback for first TCP connection
void connect1CB(void *arg) {
        struct espconn *pNewEspConn = (struct espconn *)arg;
        printf("Connection Established");
        Serial.println(espconn_send(&conn1,reinterpret_cast <unsigned char*>(psent),40));
        Serial.println(espconn_regist_sentcb(&conn1, sent1CB));
}

//Connection Callback for second TCP connection
void connect2CB(void *arg) {
        struct espconn *pNewEspConn = (struct espconn *)arg;
        Serial.println("Connection Established");
}

void initTCP() {
          // TCP1 Connection Parameter as server
          conn1.type = ESPCONN_TCP;
          conn1.state = ESPCONN_NONE;
          conn1.proto.tcp = &tcp1;
          conn1.proto.tcp->local_port = 8008; //Local port for AP
          espconn_accept(&conn1); //Accepting connection 1

          Serial.println(reinterpret_cast<char*>(psent));
          uint16_t len_gth = sizeof(*psent);
          Serial.println(len_gth);
          
          Serial.println(espconn_accept(&conn1)); 
          //espconn_regist_connectcb(&conn1, connect1CB);
          Serial.println(espconn_regist_connectcb(&conn1, connect1CB));

          //TCP2 Connection Parameter as Client
          conn2.type = ESPCONN_TCP;
          conn2.state = ESPCONN_NONE;
          conn2.proto.tcp = &tcp2;
                   
          wifi_get_ip_info(AP_NETIF,&ipinfo);
          Serial.println(ipinfo.ip.addr);
          
          *((uint32 *)conn2.proto.tcp->remote_ip) = ipaddr_addr(remote_ipaddress);
          conn2.proto.tcp->remote_port = 8014;
          conn2.proto.tcp->local_port = espconn_port();
           
          Serial.println(espconn_connect(&conn2));
          Serial.println(espconn_regist_connectcb(&conn2, connect2CB));
          Serial.println(espconn_regist_recvcb(&conn2, recv2CB));
          Serial.print("Listening for data");
          }

//Sending callback for connection1
void sent1CB(void *arg) {
          Serial.println("send");
} 

//Recieving Callback for connection 2
void recv2CB(void *arg, char *pData, unsigned short len) {
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


void initDonecb(void) {
          wifi_set_opmode_current(SOFTAP_MODE);     // Configuring in SOFTAP mode
          strcpy(reinterpret_cast<char*>(config.ssid),"ESP8266");  //Providing SSID to ESP8266
          strcpy(reinterpret_cast<char*>(config.password),"password"); //Provoding password to ESP8266
          config.ssid_len = 0;        // Length of SSID
          config.authmode = AUTH_OPEN;
          config.ssid_hidden = 0;
          config.max_connection = 4;    //Maximum number of station to be connected with ESP8266
          wifi_softap_set_config_current(&config);
          wifi_wps_enable(WPS_TYPE_PBC);      //Enabling WPS
          wifi_wps_start();
          wifi_softap_dhcps_start(); //Start DHCP For provoding IP
          wifi_set_event_handler_cb(wifi_handle_event_cb); //Events callback
}


void wifi_handle_event_cb(System_Event_t *evt)
{
          // Getitng connected Station info  
          uint8 stationCount = wifi_softap_get_station_num();     //Count the number of stations
          printf("stationCount = %d\n", stationCount);  
          struct station_info *stationInfo = wifi_softap_get_station_info();
          if (stationInfo != NULL) {
                  while (stationInfo != NULL) {
                                printf("Station IP: %d.%d.%d.%d\n", IP2STR(&(stationInfo->ip)));
                                stationInfo = STAILQ_NEXT(stationInfo, next);
                                }
                  wifi_softap_free_station_info();
                  }

          // Events for AP mode        
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
  initDonecb();           // Function to initialize esp8266 parameter
}

void loop(){
}



