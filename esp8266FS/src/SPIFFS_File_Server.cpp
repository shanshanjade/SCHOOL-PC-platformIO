// 当用户访问NodeMCU地址时，NodeMCU将会检查访问地址是否指向SPIFFS系统中的文件，并且
// 将该文件显示于用户的浏览器中。如果访问地址所指向的文件无法在SPIFFS中找到，NodeMCU将会
// 向用户发送404信息。
#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#define ssid "xiaomi123"
#define pswd "a.13513710972"

ESP8266WiFiMulti wifimulti;
ESP8266WebServer server(80);
String handelFileRead();
String getContentType();
void handleUserRequest()
{
  String webAdress = server.uri();
  bool fileOK = handleFileRead(webAdress);
}

bool handleFileRead(String path)
{
  if (path.endsWith("/")) path = "/index.html";
  String ContentType = getContentType(path);
  if (SPIFFS.exists(path))
    {
      File file = SPIFFS.open(path, "r");
      server.streamFile(file, ContentType);
      file.close();
      return true;
    }
  return false;
}

String getContentType(String fileName)
{
  if (fileName.endsWith(".htm")) return "text/html";
  else if(fileName.endsWith(".html")) return "text/html";
  else if(fileName.endsWith(".css")) return "text/css";
  else if(fileName.endsWith(".js")) return "application/javascript";
  else if(fileName.endsWith(".png")) return "image/png";
  else if(fileName.endsWith(".jpg")) return "image/jpg";
  else if(fileName.endsWith(".bmp")) return "image/bmp";
  else if(fileName.endsWith(".gif")) return "image/gif";
  else if(fileName.endsWith(".ico")) return "image/ico";
  else if(fileName.endsWith(".xml")) return "text/xml";
  else if(fileName.endsWith(".pdf")) return "application/x-pdf";
  else if(fileName.endsWith(".zip")) return "application/x-zip";
  else if(fileName.endsWith(".gz")) return "application/x-gz";
}

void setup()
{ 
  // 启动串口
  Serial.begin(115200);

  // 连接WiFi
  wifimulti.addAP(ssid, pswd);
  Serial.print("Connecting....");
  while (wifimulti.run() != WL_CONNECTED)
  {
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to : ");Serial.println(WiFi.SSID());
  Serial.print("IP address : ");Serial.println(WiFi.psk());

  // 启动文件系统
  if (SPIFFS.begin())
  {
    Serial.println("SPIFFS started!");
  }else
  {
    Serial.println("SPIFFS failed to start!");
  }
  
  // 启动网站
  server.onNotFound(handleUserRequest);
  server.begin();
}

void loop()
{
  server.handleClient();
}