#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Artnetnode.h>
#include <Ticker.h>

#define DMX_MAX 512 // max. number of DMX data packages.
uint8_t DMXBuffer[DMX_MAX];

// Change ip and mac address for your setup
byte mac[] = {0x00, 0x1A, 0xB6, 0x02, 0xD0, 0x35};

char* ssid = "LUMOS";
char* password = "2784B508";

Ticker ticker;

char udpBeatPacket[70];

WiFiUDP UdpSend;

Artnetnode artnetnode;

int pinR = 15;
int pinG = 12;
int pinB = 13;

void setup()
{
  Serial.begin(9600);
  artnetnode.setName("ESP8266 - Artnet");
  artnetnode.setStartingUniverse(1);
  while(artnetnode.begin(ssid, password, 1) == false){
    Serial.print("X");
  }
  Serial.println();
  Serial.println("Connected");

  analogWriteRange(255);
  
  artnetnode.setDMXOutput(0,1,0);

  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  analogWrite(pinR, 0);
  analogWrite(pinG, 0);
  analogWrite(pinB, 0);

  UdpSend.begin(4000);
  IPAddress localIP = WiFi.localIP();
  
  uint8_t mac[6];
  WiFi.macAddress(mac);
    
  sprintf(udpBeatPacket, "{\"mac\":\"%x:%x:%x:%x:%x:%x\",\"ip\":\"%d.%d.%d.%d\",\"voltage\":1}", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], localIP[0],  localIP[1],  localIP[2],  localIP[3]);
  //Serial.println();
  //Serial.println(udpBeatPacket);
  ticker.attach(5,beat);
  beat();
}

void loop()
{ 
  uint16_t code = artnetnode.read();
  if(code){
    if (code == OpDmx)
    {
      //Serial.print("D");
      analogWrite(pinR, artnetnode.returnDMXValue(0, 1));
      analogWrite(pinG, artnetnode.returnDMXValue(0, 2));
      analogWrite(pinB, artnetnode.returnDMXValue(0, 3));
    }
    else if (code == OpPoll) {
      Serial.println("Art Poll Packet");
    }
  }
  if (WiFi.status() == 6){
    ESP.reset();
  }
}

void beat(){

  UdpSend.beginPacket({192,168,0,100},33333);
  UdpSend.write(udpBeatPacket, sizeof(udpBeatPacket)-1);
  //Udp.write("{\"mac\":\"f6:8b:d9:c2:9a:69\",\"ip\":\"192.168.1.75\",\"voltage\":579}");
  UdpSend.endPacket();
}
