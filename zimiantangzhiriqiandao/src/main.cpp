/*
时间：2020年10月28日
自勉堂值日签到表

1.准备的网页为两个：1.index.html, 2.responok.html
2.wifi名称为“自勉堂值日签到表”，无密码
3.
当提交的姓名为“郭闪” && 电话为“13513710972”时，会显示签到表
*/
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <FS.h>

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);
void handleOk();
void handleList();


 
const char responseHTML[] PROGMEM=  ""
"<!DOCTYPE html>"
"<html lang=\"zh\">"
"<head>"
"    <meta charset=\"UTF-8\">"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=no\">"
"    <title>自勉堂值日签到处</title>"
"    <style>"
"        input {"
"            height: 7.5vw;"
"            width: 91vw;"
"            font-size: 4.4vw;"
"            border-radius: 5px;"
"            padding-left: 0.5em;"
"        }"
"        select {"
"            height: 9vw;"
"            font-size: 4.4vw;"
"            border-radius: 5px;"
"        }"
"        #lou {"
"            width: 79.5vw;"
"            float: left;"
"        }"
"        #jiaoshihao {"
"            margin-left: 4px;"
"        }"
"        #tijiao {"
"            width: 95vw;"
"            height: 11vw;"
"            background: green;"
"            border-radius: 5px;"
"        }"
"        "
"    </style>"
"</head>"
"<body>"
"    <h1>自勉堂值日签到表</h1>"
"    <hr>"
"    <form action=\"/ok\" onsubmit=\"tijiao()\">"
"        <label for=\"\" id=\"time1\">签到时间：</label><br>"
"        <input type=\"text\" name=\"time\" id=\"time\" value=\"\" readonly>"
"        <br>"
"        <br>"
"        <label for=\"\">请选择 班级：</label><br>"
"        <select name=\"nianji\">"
"            <option value=\"2016级\">2016级</option>"
"            <option value=\"2017级\">2017级</option>"
"            <option value=\"2018级\">2018级</option>"
"            <option value=\"2019级\">2019级</option>"
"            <option value=\"2020级\">2020级</option>"
"        </select>"
"        <select name=\"zhuanye\">"
"            <option value=\"视觉传达设计\">视觉传达设计</option>"
"            <option value=\"环境设计\">环境设计</option>"
"            <option value=\"服装与服饰设计\">服装与服饰设计</option>"
"        </select>"
"        <select name=\"benzhuan\">"
"            <option value=\"本科\">本科</option>"
"            <option value=\"专科\">专科</option>"
"            <option value=\"专升本\">专升本</option>"
"        </select>"
"        <select name=\"banji\">"
"            <option value=\"1班\">1班</option>"
"            <option value=\"2班\">2班</option>"
"            <option value=\"3班\">3班</option>"
"            <option value=\"4班\">4班</option>"
"        </select><br>"
"        <br>"
"        <label for=\"\">请选择 打扫教室：</label><br>"
"        <select name=\"lou\" id=\"lou\" >"
"            <option value=\"自勉堂\">自勉堂</option>"
"        </select>"
"        <select name=\"jiaoshihao\" id=\"jiaoshihao\" >"
"                <option value=\"102\">102</option>"
"                <option value=\"112\">112</option>"
"                <option value=\"113\">113</option>"
"                <option value=\"116\">116</option>"
"                <option value=\"402\">402</option>"
"                <option value=\"405\">405</option>"
"                <option value=\"406\">406</option>"
"                <option value=\"407\">407</option>"
"                <option value=\"408\">408</option>"
"                <option value=\"409\">409</option>"
"                <option value=\"410\">410</option>"
"                <option value=\"411\">411</option>"
"                <option value=\"412\">412</option>"
"                <option value=\"413\">413</option>"
"                <option value=\"414\">414</option>"
"                <option value=\"415\">415</option>"
"                <option value=\"417\">417</option>"
"                <option value=\"418\">418</option>"
"                <option value=\"501\">501</option>"
"                <option value=\"502\">502</option>"
"                <option value=\"503\">503</option>"
"                <option value=\"504\">504</option>"
"                <option value=\"505\">505</option>"
"                <option value=\"506\">506</option>"
"                <option value=\"507\">507</option>"
"                <option value=\"508\">508</option>"
"                <option value=\"509\">509</option>"
"                <option value=\"510\">510</option>"
"                <option value=\"511\">511</option>"
"            </select>"
"        <br>"
"        <br>"
"        <label for=\"\">请输入 姓名：</label><br>"
"            <input id=\"xingming\" type=\"text\" name=\"xingming\" minlength=\"2\" maxlength=\"3\" required ><br><br>"
"        <label for=\"\">请输入 电话：</label><br>"
"            <input id=\"dianhua\" type=\"text\" name=\"dianhua\" minlength=\"11\" maxlength=\"11\" required ><br><br>"
"        <input id=\"tijiao\" type=\"submit\" value=\"签到\">"
"    </form>"
"    <br>"
"    <br>"
"</body>"
"<script>"
"    var myDate = new Date();"
"    document.getElementById(\"time\").value =myDate.toLocaleString();"
"</script>"
"</html>";

String responseHTMLOk = ""
                        "<!DOCTYPE html>"
                        "<html lang=\"zh\">"
                        "<head>"
                        "    <meta charset=\"UTF-8\">"
                        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=no\">"
                        "    <title>JavaScript</title>"
                        "</head>"
                        "<body>"
                        "    <h1>签到成功！</h1>"
                        "</body>"
                        "</html>";

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("自勉堂值日签到表");

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);
  
  // replay to all requests with same HTML
  webServer.on("/ok", handleOk);
  webServer.on("/list",handleList);

  webServer.onNotFound([](){
    webServer.send(200, "text/html", responseHTML);
  });
  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}

void handleOk(){
  String time,nianji,zhuanye,benzhuan,banji,lou,jiaoshihao,xingming,dianhua;
  time = webServer.arg("time");
  nianji = webServer.arg("nianji");
  zhuanye = webServer.arg("zhuanye");
  benzhuan = webServer.arg("benzhuan");
  banji = webServer.arg("banji");
  lou = webServer.arg("lou");
  jiaoshihao = webServer.arg("jiaoshihao");
  xingming = webServer.arg("xingming");
  dianhua = webServer.arg("dianhua");

  if (xingming == "郭闪" && dianhua == "13513710972") {
    handleList();
  } else {
    File list = SPIFFS.open("/list.txt","a+");
    list.print(time);list.print("--");
    list.print(nianji);list.print("--");
    list.print(zhuanye);list.print("--");
    list.print(benzhuan);list.print("--");
    list.print(banji);list.print("--");
    list.print(lou);list.print("--");
    list.print(jiaoshihao);list.print("--");
    list.print(xingming);list.print("--");
    list.println(dianhua);
    list.close();
    webServer.send(200,"text/html",responseHTMLOk + time + "<br>" + nianji + zhuanye + benzhuan + banji + "<br>" + lou + jiaoshihao + "<br>" + xingming + "<br>" + dianhua);
    webServer.client().stop();
  }
}

void handleList(){
  File list = SPIFFS.open("/list.txt","r");
  webServer.streamFile(list,"text/plain");
  list.close();
}