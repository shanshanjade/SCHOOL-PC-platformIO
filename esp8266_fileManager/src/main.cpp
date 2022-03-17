#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
ESP8266WebServer webServer(80);



void handleRoot();
void handleList();
void handleUploadPage();
void handleFileUpload();
void handleDeletePage();
void handleDelete();
void handleNotFound();
bool handleFileRead(String path);
String getContentType(String filename);
void handleUserRequest();
void respondOK();
void format();
void reset();
File fsuploadfile;

void setup() {
    Serial.begin(115200);
    SPIFFS.begin();
    Serial.println("串口开始");
    Serial.println("文件系统开始");
    // WiFi.mode(WIFI_AP);
    // WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    // WiFi.softAP("文件管理系统", "12345678");
    WiFi.mode(WIFI_STA);
    WiFi.begin("yishulou","123456abcd");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println(WiFi.localIP().toString());
    webServer.on("/", HTTP_GET, handleRoot);
    webServer.on("/list", HTTP_GET, handleList);
    webServer.on("/format", format);
    webServer.on("/upload.html", HTTP_GET, handleUploadPage);
    webServer.on("/upload.html", HTTP_POST, respondOK, handleFileUpload);
    webServer.on("/delete.html", HTTP_GET, handleDeletePage);
    webServer.on("/delete.html", HTTP_POST, handleDelete);
    webServer.on("/reset", reset);
    webServer.onNotFound(handleUserRequest);
    MDNS.begin("esp8266", apIP);
    Serial.println("MDNS开始");
    webServer.begin();
    Serial.println("HTTP服务器开始！");
    MDNS.addService("http", "tcp", 80);
}

void loop() {
    MDNS.update();
    webServer.handleClient();
}
void handleRoot() {
    // webServer.send(200, "text/html", indexPage);
    handleUserRequest();
}
void handleList() {
    const size_t capacity = 2 * JSON_ARRAY_SIZE(50) + JSON_OBJECT_SIZE(2);
    DynamicJsonDocument doc(capacity);
    String myjson;
    Dir myDir = SPIFFS.openDir("/images");
    JsonArray name = doc.createNestedArray("name");
    JsonArray size = doc.createNestedArray("size");
    while (myDir.next()) {
        name.add(myDir.fileName());
        size.add(myDir.fileSize());
    }
    serializeJson(doc, myjson);
    serializeJson(doc, Serial);
    Serial.println("");
    webServer.send(200, "text/plain", myjson);
}
void respondOK() {
    webServer.send(200, "text/plain", "200 OK!");
}
void handleNotFound() {
    webServer.send(404, "text/plain", "404 not found");
}
void handleUploadPage() {
    Serial.println("进入文件上传页面！");
    // webServer.send(200, "text/html", uploadPage);
    handleUserRequest();
}
void handleFileUpload() {
    HTTPUpload& upload = webServer.upload();
    if (upload.status == UPLOAD_FILE_START) {
        String fileName = upload.filename;
        if (!fileName.startsWith("/"))
            fileName = "/images/" + fileName;
        Serial.println("File Name: " + fileName);
        fsuploadfile = SPIFFS.open(fileName, "w");
        Serial.println("打开文件成功！");
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (fsuploadfile)
            fsuploadfile.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
        if (fsuploadfile) {
            fsuploadfile.close();
            Serial.println(" Size: " + upload.totalSize);
            Serial.println("File upload success");
            webServer.send(200, "text/plain", "File upload success");
        } else {
            Serial.println("File upload failed");
            webServer.send(200, "text/plain", "File upload failed");
        }
    }
}
void handleDeletePage() {
    Serial.println("进入删除文件页面！");
    // webServer.send(200, "text/html", deletePage);
    handleUserRequest();
}
void handleDelete() {
    String filename = webServer.arg("filename");
    SPIFFS.remove(filename);
    Serial.print("已删除文件：");
    Serial.println(filename);
    webServer.send(200, "text/plain", "Delete file success");
}
void format() {
    SPIFFS.format();
    Serial.println("成功格式化");
    webServer.send(200, "text/plain", "Format success");
}
void reset() {
    Serial.println("重启服务器");
    ESP.reset();
}

bool handleFileRead(String path) {  //处理浏览器HTTP访问
    if (path.endsWith("/")) {       // 如果访问地址以"/"为结尾
        path = "/index.html";       // 则将访问地址修改为/index.html便于SPIFFS访问
    }
    String contentType = getContentType(path);  // 获取文件类型

    if (SPIFFS.exists(path)) {                    // 如果访问的文件可以在SPIFFS中找到
        File file = SPIFFS.open(path, "r");       // 则尝试打开该文件
        webServer.streamFile(file, contentType);  // 并且将该文件返回给浏览器
        file.close();                             // 并且关闭文件
        return true;                              // 返回true
    }
    return false;  // 如果文件未找到，则返回false
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