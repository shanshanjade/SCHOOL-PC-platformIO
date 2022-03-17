#include <Arduino.h>
#include <EEPROM.h>

#define BOOTMODE_ADDR 0
#define SSID_ADDR 1
#define PSWD_ADDR 31
#define OFFLINE_CMD "offl"
#define CONFIGWIFI_CMD "cw"

uint8_t boot_mode = 0; //0=正常启动 1=配网模式 2=离线模式
char ssid[30];
char pswd[30];
void (*resetFunc)(void) = 0; //软重启函数

void setup()
{
  Serial.begin(9600);
  EEPROM.begin();
  EEPROM.get(BOOTMODE_ADDR, boot_mode);
  if (boot_mode == 1)
  {
    recw: Serial.println("进入配网模式...");
    boot_mode = 0;
    EEPROM.put(BOOTMODE_ADDR, boot_mode);
    bool done = false;
    while (!done)
    {
      bool done_ssid = false;
      Serial.println("请输入wifi名：");
      while (!done_ssid)
      {
        if (Serial.available() > 0)
        {
          strcpy(ssid, Serial.readString().c_str());
          Serial.println(ssid);
          EEPROM.put(SSID_ADDR, ssid);
          done_ssid = true;
        }
      }

      bool done_pswd = false;
      Serial.println("请输入wifi密码：");
      while (!done_pswd)
      {
        if (Serial.available() > 0)
        {
          strcpy(pswd, Serial.readString().c_str());
          Serial.println(pswd);
          EEPROM.put(PSWD_ADDR, pswd);
          done_pswd = true;
        }
      }
      Serial.println("配网完成！设备将在5S后重启...");
      Serial.println("输入\"n\"以重新配置");
      uint16_t waitms = millis();
      while ( (millis() - waitms) <= 5000 )
      {
        if (Serial.available() > 0)
        {
          if (Serial.readString() == "n")
          {
            goto recw;
          }
        }
      }
      delay(5000);
      done = true;
      resetFunc();
    }
  }
  else if (boot_mode == 2)
  {
    Serial.println("进入离线模式...");
    boot_mode = 0;
    EEPROM.put(BOOTMODE_ADDR, boot_mode);
    bool done = false;
    while (!done)
    {
      if (Serial.available() > 0)
      {
        if (Serial.readString() == "d")
        {
          done = true;
          Serial.println("退出离线模式！");
          delay(500);
          resetFunc();
        }
      }
    }
  }
  Serial.println("正常启动！");
  Serial.print("SSID: ");
  Serial.println(EEPROM.get(SSID_ADDR, ssid));
  Serial.print("PSWD: ");
  Serial.println(EEPROM.get(PSWD_ADDR, pswd));
}

void loop()
{
  if (Serial.available() > 0)
  {
    String a = Serial.readString();
    if (a == CONFIGWIFI_CMD)
    {
      boot_mode = 1;
      EEPROM.put(BOOTMODE_ADDR, boot_mode);
      resetFunc();
    }
    else if (a == OFFLINE_CMD)
    {
      boot_mode = 2;
      EEPROM.put(BOOTMODE_ADDR, boot_mode);
      resetFunc();
    }
    Serial.flush();
  }
  delay(500);
}
