#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <FS.h>

String indexPage = String("") +
"<!DOCTYPE html>"+
"<html lang=\"zh\">"+
"<head>"+
"    <meta charset=\"UTF-8\">"+
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=no\">"+
"    <title>文件管理系统</title>"+
"</head>"+
"<body onload=\"ajax()\">"+
"    <h1>文件管理系统</h1>"+
"    <p id=\"demo\"></p>"+
"    <a href=\"/upload\">上传文件</a><br>"+
"    <a href=\"/format\">格式化磁盘</a><br>"+
"    <a href=\"/delete\">删除文件</a><br>"+
"    <a href=\"/reset\">重启服务器</a>"+
"</body>"+
"<script>"+
"    function ajax(){"+
"        var xhttp = new XMLHttpRequest();"+
"        xhttp.onreadystatechange = function() {"+
"            if (this.readyState == 4 && this.status == 200) {"+
"                var i;"+
"                var json;"+
"                var st = \"\";"+
"                json =  JSON.parse(this.responseText);"+
"                for (i in json.name){"+
"                    st += \"<a>\" + json.name[i] + \"<\/\/a>\" +\"<br>\" ;"+
"                }"+
"                document.getElementById(\"demo\").innerHTML = st;"+
"            }"+
"        };"+
"        xhttp.open(\"GET\", \"/list\", true);"+
"        xhttp.send();"+
"    };"+
"</script>"+
"</html>";



String uploadPage = String("") +
"<!DOCTYPE html>"+
"<html lang=\"zh\">"+
"<head>"+
"    <meta charset=\"UTF-8\">"+
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=no\">"+
"    <title></title>"+
"</head>"+
"<body>"+
"    <form method=\"POST\" enctype=\"multipart/form-data\">"+
"        <input type=\"file\" name = \"data\">"+
"        <input type=\"submit\" value=\"提交\">"+
"    </form>"+
"</body>"+
"</html>";

String deletePage = String("") +
"<!DOCTYPE html>"+
"<html lang=\"zh\">"+
"<head>"+
"    <meta charset=\"UTF-8\">"+
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=no\">"+
"    <title></title>"+
"</head>"+
"<body onload=\"ajax1()\">"+
"    <h1>删除文件</h1>"+
"    <a id=\"test\"></a>"+
"    <form id=\"demo\" method=\"POST\">"+
"        "+
"    </form>"+
"</body>"+
"<script>"+
"    function ajax1(){"+
"        var xhttp = new XMLHttpRequest();"+
"        xhttp.onreadystatechange = function() {"+
"            if (this.readyState == 4 && this.status == 200) {"+
"                var i;"+
"                var json;"+
"                var st = \"\";"+
"                var demo = document.getElementById(\"demo\");"+
"                json =  JSON.parse(this.responseText);"+
"                for (i in json.name){"+
"                    var newInput = document.createElement(\"input\");"+
"                    var newText = document.createElement(\"a\");"+
"                    var newLine = document.createElement(\"br\");"+
"                    newInput.type = \"radio\";"+
"                    newInput.name = \"filename\";"+
"                    newInput.value = json.name[i];"+
"                    newText.innerText = json.name[i];"+
"                    demo.appendChild(newInput);"+
"                    demo.appendChild(newText);"+
"                    demo.appendChild(newLine);"+
"                }"+
"                var newSubmit = document.createElement(\"input\");"+
"                newSubmit.type = \"submit\";"+
"                newSubmit.value = \"提交\";"+
"                demo.appendChild(newSubmit);"+
"            }"+
"        };"+
"        xhttp.open(\"GET\", \"/list\", true);"+
"        xhttp.send();"+
"    };"+
"</script>"+
"</html>";

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
ESP8266WebServer webServer(80);

const size_t capacity = 2 * JSON_ARRAY_SIZE(50) + JSON_OBJECT_SIZE(2);
DynamicJsonDocument doc(capacity);


void handleRoot();
void handleList();
void handleUploadPage();
void handleFileUpload();
void handleDeletePage();
void handleDelete();
void handleNotFound();
void respondOK();
void format();
void reset();
File fsuploadfile;

void setup()
{
	Serial.begin(115200);
	SPIFFS.begin();
	Serial.println("串口开始");
	Serial.println("文件系统开始");
	WiFi.mode(WIFI_AP);
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP("文件管理系统","12345678");
	webServer.on("/", HTTP_GET, handleRoot);
	webServer.on("/list",HTTP_GET, handleList);
	webServer.on("/format", format);
	webServer.on("/upload", HTTP_GET,handleUploadPage);
	webServer.on("/upload", HTTP_POST, respondOK ,handleFileUpload);
	webServer.on("/delete", HTTP_GET,handleDeletePage);
	webServer.on("/delete",HTTP_POST,handleDelete);
	webServer.on("/reset", reset);
	webServer.onNotFound(handleNotFound);

	MDNS.begin("esp8266", apIP);

	Serial.println("MDNS开始");

	webServer.begin();
	Serial.println("HTTP服务器开始！");

	MDNS.addService("http", "tcp", 80);
}

void loop()
{
	MDNS.update();
	webServer.handleClient();
}
void handleRoot()
{
	webServer.send(200,"text/html", indexPage);
}
void handleList()
{
	String myjson;
	Dir myDir = SPIFFS.openDir("/");
	JsonArray name = doc.createNestedArray("name");
	JsonArray size = doc.createNestedArray("size");

	while (myDir.next())
	{
		name.add(myDir.fileName());
		size.add(myDir.fileSize());
	}
	serializeJson(doc, myjson);
	serializeJson(doc, Serial);
	Serial.println("");
	webServer.send(200, "text/plain", myjson);
}
void respondOK()
{
	webServer.send(200);
}
void handleNotFound()
{	
	webServer.send(404,"text/plain","404 not found");
}
void handleUploadPage()
{	
	Serial.println("进入文件上传页面！");
	webServer.send(200, "text/html", uploadPage);
}
void handleFileUpload()
{	
	HTTPUpload& upload = webServer.upload();
	if(upload.status == UPLOAD_FILE_START)
	{
		String fileName = upload.filename;
		if(!fileName.startsWith("/")) fileName = "/" +fileName;
		Serial.println("File Name: " + fileName);
		fsuploadfile = SPIFFS.open(fileName, "w");
		Serial.println("打开文件成功！");
	} else if (upload.status == UPLOAD_FILE_WRITE)
	{
		if(fsuploadfile) fsuploadfile.write(upload.buf,upload.currentSize);
	} else if (upload.status == UPLOAD_FILE_END)
	{
		if (fsuploadfile) 
		{
			fsuploadfile.close();
			Serial.println(" Size: "+ upload.totalSize);
			Serial.println("File upload success"); 
			webServer.send(200, "text/plain", "File upload success");
		} else 
		{
			Serial.println("File upload failed"); 
			webServer.send(200, "text/plain", "File upload failed");
		}
	}
}
void handleDeletePage(){
	Serial.println("进入删除文件页面！");
	webServer.send(200, "text/html", deletePage);
}
void handleDelete(){
	String filename = webServer.arg("filename");
	SPIFFS.remove(filename);
	Serial.print("已删除文件：");Serial.println(filename);
	webServer.send(200, "text/plain", "Delete file success");
}
void format(){
	SPIFFS.format();
	Serial.println("成功格式化");
	webServer.send(200,"text/plain","Format success");
}
void reset()
{
	Serial.println("重启服务器");
	ESP.reset();
}