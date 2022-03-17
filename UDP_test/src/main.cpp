#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

// IPAddress local_IP(192,168,8,88);
// IPAddress gateway(192,168,8,88);
// IPAddress subnet(255,255,255,0);
WiFiUDP udp;
uint8_t incomingPacket[15000];
char espchar[] = "hello from mina!郭闪闪";
char replyPacket[] = "Hi there! Got the message :-)你好呀，我是esp8266，我获取到了小程序发送的信息";
void setup()
{
	Serial.begin(115200);
	// WiFi.mode(WIFI_AP);
	// WiFi.softAPConfig(local_IP,gateway,subnet);
	// WiFi.softAP("UDPTEST","12345678");
	// WiFi.begin();
	WiFi.beginSmartConfig();
	WiFi.mode(WIFI_AP_STA);
	WiFi.begin("HUAWEIMATE30", "12345678");
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print('.');
		delay(500);
	}
	Serial.println("");
	Serial.println(WiFi.localIP().toString());
	udp.begin(8266);
}

void loop()
{
	int packetSize = udp.parsePacket();
	if (packetSize)
	{
		Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
		int len = udp.read(incomingPacket, packetSize);
		if (len > 0)
		{
			Serial.printf("文件类型为 %d\n", incomingPacket[0]);
			Serial.printf("宽度为 %d\n", incomingPacket[1]);
			Serial.printf("高度为 %d\n", incomingPacket[2]);
		}
		udp.beginPacket(udp.remoteIP(), udp.remotePort());
		udp.write(replyPacket);
		udp.endPacket();
	}
}