#include <SPI.h>
#include <RH_RF95.h>
#include <EEPROM.h>

//ノードIDを登録しておくEEPROMのアドレス
#define NODE_ID_ADDRESS 0x00


int node_id = 0;
char msg[100] = "";
int packetCount = 0;

RH_RF95 rf95;
float frequency = 920.6;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Starting");
  
  Serial.println("LoRa Sender");
  if (!rf95.init())
    Serial.println("init failed");
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

  //ノードIDの取得
  node_id = EEPROM.read(NODE_ID_ADDRESS);//Get Client id.     
}



void loop() {

  msg[0] = 'x';//ヘッダ
  msg[1] = node_id;
  msg[2] = ++packetCount;
  strcat(msg, ",");
  strcat(msg, "Test Message");
  
  Serial.println(packetCount);
  Serial.println(msg);
  
  Serial.println("send start");
  rf95.send(msg, strlen((char*)msg));

  Serial.println("response wait start");
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];//Reply data array
  uint8_t len = sizeof(buf);//reply data length
  for(int i = 0; i < 10; i++){//繰り返しは10回まで
    if (rf95.waitAvailableTimeout(3000))// Check If there is reply in 3 seconds.
    {
      // Should be a reply message for us now   
      if (rf95.recv(buf, &len))//check if reply message is correct
      {
        if(buf[0] == node_id ) // Check if reply message has the our node ID
        {
          Serial.print("got reply: ");//print reply
          Serial.println((char*)buf);
          i=10;//ループ終了
        }
      }
      else
      {
         Serial.println("recv failed");//別の信号の可能性
      }
    }
    else
    {
        Serial.println("No reply, is rf95_server running?");//No signal reply
        rf95.send(msg, strlen((char*)msg));//resend data
    }
  }
  int msgLength = strlen((char*)msg);
  memset( msg , '\0' , msgLength );
  Serial.println("send end");
  delay(7000);//次の送信までのウェイト
}
