#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include "hzk16s.c"

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 88, 8);
ESP8266WebServer webServer(80);

void handleRoot_GET();
void handleRoot_POST();
void handleNotFound();
bool handleFileRead(String path);
String getContentType(String filename);
void handleUserRequest();
void respondOK();
File fsuploadfile;

void setup() {
    Serial.begin(9600);
    SPIFFS.begin();
    // WiFi.mode(WIFI_AP);
    // WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    // WiFi.softAP("文件管理系统", "12345678");
    WiFi.mode(WIFI_STA);
    WiFi.begin("HUAWEIMATE30", "12345678");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.println("WIFI connected!");
    Serial.println(WiFi.localIP().toString());

    webServer.on("/", HTTP_GET, handleRoot_GET);
    webServer.on("/", HTTP_POST, handleRoot_POST);
    webServer.onNotFound(handleUserRequest);
    webServer.begin();
}

void loop() {
    webServer.handleClient();
}

void respondOK() {
    webServer.send(200, "text/plain", "200 OK!");
};
bool handleFileRead(String path) {  //处理浏览器HTTP访问
    if (path.endsWith("/")) {       // 如果访问地址以"/"为结尾
        path = "/index.html";       // 则将访问地址修改为/index.html便于SPIFFS访问
    }
    Serial.println(path);
    String contentType = getContentType(path);    // 获取文件类型
    if (SPIFFS.exists(path)) {                    // 如果访问的文件可以在SPIFFS中找到
        File file = SPIFFS.open(path, "r");       // 则尝试打开该文件
        webServer.streamFile(file, contentType);  // 并且将该文件返回给浏览器
        file.close();                             // 并且关闭文件
        return true;                              // 返回true
    }
    return false;  // 如果文件未找到，则返回false
}
void handleRoot_GET() {
    handleUserRequest();
}
void handleRoot_POST() {
    String s1 = webServer.arg("gb2312");
    uint8_t s1_length = s1.length();
    s1.replace("%", " ");  //将所有的 "%" 替换为空格
    s1.trim();             //删除首尾空格
    const char* s1_cstr = s1.c_str();
    char* stopString = (char*)s1_cstr;
    uint8_t* s1_cstr_arr = (uint8_t*)malloc(s1_length / 3);
    for (int i = 0; i < s1_length / 3; i++) {
        s1_cstr_arr[i] = strtol(stopString, &stopString, 16);
    }
    char buffer[32];
    int txt_index;
    unsigned char key[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
    for (txt_index = 0; txt_index < s1_length / 6; txt_index++) {
        unsigned long offset = (94 * (unsigned int)(s1_cstr_arr[txt_index * 2] - 0xa0 - 1) + (s1_cstr_arr[txt_index * 2 + 1] - 0xa0 - 1)) * 32;
        memcpy_P(&buffer, &achzk16s[offset], 32);
        for (int k = 0; k < 16; k++) {
            for (int j = 0; j < 2; j++) {
                for (int i = 0; i < 8; i++) {
                    int flag = buffer[k * 2 + j] & key[i];
                    Serial.printf("%s", flag ? "■" : "□");
                }
            }
            Serial.printf("\n");
        }
        delay(10);
    }
    webServer.send(200, "text/plain", "get string!");
    free(s1_cstr_arr);
}

void handleUserRequest() {
    // 获取用户请求网址信息
    String webAddress = webServer.uri();
    Serial.println(webAddress);
    // 通过handleFileRead函数处处理用户访问
    bool fileReadOK = handleFileRead(webAddress);
    // 如果在SPIFFS无法找到用户访问的资源，则回复404 (Not Found)
    if (!fileReadOK) {
        webServer.send(404, "text/plain", "404 Not Found");
    }
}
String getContentType(String filename) {
    if (filename.endsWith(".htm"))
        return "text/html";
    else if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".png"))
        return "image/png";
    else if (filename.endsWith(".gif"))
        return "image/gif";
    else if (filename.endsWith(".jpg"))
        return "image/jpeg";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    else if (filename.endsWith(".xml"))
        return "text/xml";
    else if (filename.endsWith(".pdf"))
        return "application/x-pdf";
    else if (filename.endsWith(".zip"))
        return "application/x-zip";
    else if (filename.endsWith(".gz"))
        return "application/x-gzip";
    return "text/plain";
}