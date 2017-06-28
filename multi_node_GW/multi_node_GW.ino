#include <SPI.h>
#include <RH_RF95.h>
#include <Console.h>
#include "YunClient.h"
YunClient client;

RH_RF95 rf95;
float frequency = 920.6;

int cnt = 0;
void setup() {
  Bridge.begin(115200);
  Console.begin();
  while(!Console);
  Console.println("LoRa Receiver");

  if (!rf95.init())
    Console.println("init failed")
  ;
  // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(13);
  // キャリアセンス5ms
  rf95.setCADTimeout((unsigned long)5);
  RH_RF95::ModemConfig config;
  config.reg_1d = 0b01110000 | 0b0100; // Bw125 | Cr46
  config.reg_1e = 0b10010000 | 0b0100; // Sf512 | CRCon
  config.reg_26 = 0b00000000; // default.
  rf95.setModemRegisters(&config);

}

void loop() {
  if (rf95.available())// Listen Data from LoRa Node
  {
    uint8_t message[RH_RF95_MAX_MESSAGE_LEN];//receive data buffer
    uint8_t len = sizeof(message);//data buffer length
    memset( message , '\0' , len );
    if (rf95.recv(message, &len))//Check if there is incoming data
    {
      if(message[0] == 'x'){ //ヘッダの確認
        char *tp, head;
        int cnt=0;
        /* ,を区切りに文字列を抽出 */   
        tp = strtok( message, "," );
        String msg;
        int packetCount,id;
        
        while ( tp != NULL ) {
          cnt++;
          
          if(cnt==1){
            head=tp[0];
            Console.print("Id:");
            id=tp[1];
            packetCount=tp[2];
            Console.println(id);
          }
          
          if(cnt==2){
            Console.print("msg:");
            msg=String(tp);
            Console.println(msg);
       
          }
          tp = strtok( NULL,"," );
        }
        
        Console.print("packetCount:");
        Console.print((uint8_t)packetCount);Console.print(" ");
        // print RSSI of packet
        Console.print("' with RSSI ");
        int a= rf95.lastRssi();
        Console.println(a);
        uint8_t data[] = "   Server ACK";//Reply 
        data[0] = (uint8_t)id;
  
        //Console.print(data[0]);Console.print(data[1]);
        Console.println((char *)data);

        Console.println("send reply");
        rf95.send(data, sizeof(data));// Send Reply to LoRa Node
        rf95.waitPacketSent();
  
        Console.println();
      }
    }
  }
}
