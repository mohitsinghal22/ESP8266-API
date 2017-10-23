extern "C"{
  #include<ets_sys.h>
  #include<osapi.h>
  #include<os_type.h>
  #include<gpio.h>
  #include<user_interface.h>
  #include<espconn.h>
  #include<mem.h>
}

#define remote_ipaddress "192.168.4.1"      //Ip address of the Ap to which tcp connection is made
char *psent = "Yeah its working second connection"; //Data to be send 

//Tcp Stucture for connection 1
struct espconn conn1;
esp_tcp tcp1;
struct ip_info ipinfo;
struct softap_config config;

//Tcp Structure for connection 2
struct espconn conn2;
esp_tcp tcp2;

#define AP_NETIF 0x01

//Connection 2 callback
void connect2CB(void *arg) {
struct espconn *pNewEspConn = (struct espconn *)arg;
printf("Connection Established");
Serial.println(espconn_send(&conn2,reinterpret_cast <unsigned char*>(psent),40));
Serial.println(espconn_regist_sentcb(&conn2, sent2CB));
}

//Connection 1 callback
void connectCB(void *arg) {
        struct espconn *pNewEspConn = (struct espconn *)arg;
        Serial.println("Connection Established");
}

void inittcp() {
  // Initializing connection 1 parameters as client
          conn1.type = ESPCONN_TCP;  //Type of the connection TCP or UDP
          conn1.state = ESPCONN_NONE; // Current State of TCP
          conn1.proto.tcp = &tcp1;
                   
          wifi_get_ip_info(AP_NETIF,&ipinfo);
          Serial.println(ipinfo.ip.addr);
          
          *((uint32 *)conn1.proto.tcp->remote_ip) = ipaddr_addr(remote_ipaddress);
          conn1.proto.tcp->remote_port = 8008;  //Port of the server 
          conn1.proto.tcp->local_port = espconn_port(); //Port of the client
           
          espconn_connect(&conn1);
          Serial.println(espconn_connect(&conn1));  //call to establish connection with Server
          espconn_regist_connectcb(&conn1, connectCB);
          Serial.println(espconn_regist_connectcb(&conn1, connectCB)); // Conenction callback
          espconn_regist_recvcb(&conn1, recvCB);
          Serial.println(espconn_regist_recvcb(&conn1, recvCB));  //Recieve callback
          Serial.print("Listening for data");

          //Initializing Connection 2 Parameter as server
          conn2.type = ESPCONN_TCP; //Type of connection TCP or UDP
          conn2.state = ESPCONN_NONE; //State of the TCP connection
          conn2.proto.tcp = &tcp2;
          conn2.proto.tcp->local_port = 8014; // Local port on which server is running
          espconn_accept(&conn2); //Accepting Client calls

          Serial.println(reinterpret_cast<char*>(psent));
          uint16_t len_gth = sizeof(*psent);
          Serial.println(len_gth);
          
          //Serial.println(espconn_accept(&conn2)); 
          //espconn_regist_connectcb(&conn2, connectCB);
          Serial.println(espconn_regist_connectcb(&conn2, connect2CB)); //Connection 2 Callback
          
}

//Recieve callback from Connection 1
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

//Sent callback for conenction 2
void sent2CB(void *arg) {
          Serial.println("send");
} 

//Initializing Station Parameters
void initDone() {
          wifi_set_opmode_current(STATION_MODE);
          struct station_config stationConfig;
          strncpy(reinterpret_cast<char*>(stationConfig.ssid), "ESP8266", 32);
          strncpy(reinterpret_cast<char*>(stationConfig.password),"password", 64);
          wifi_station_set_config(&stationConfig);
          wifi_set_event_handler_cb(wifi_handle_event_cb);
}

//Handling Station events
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
  initDone();           //Calling function to initialize ESP8266 as Station
}



void loop(){

}



