// Esp libraries
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
// end
#include <string>
// ssid for esp8266
#ifndef STASSID
// #define STASSID "LED-ART"
// #define STAPSK "yusufMJWL"
#define STASSID "LEDART"
#define STAPSK "LEDART123"
#endif
// end

//Setting static ip
IPAddress staticIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
//end

// creating char buffer for ssid and password
const char* ssid = STASSID;
const char* password = STAPSK;
// end
// Esp server instantiating at 80
ESP8266WebServer server(80);
// end




// P10 libraries
#include <SPI.h>
#include <DMD2.h>
#include <fonts/Arial14.h>
#include <fonts/SystemFont5x7.h>
// end

// Creating classes for P10 and its textbox
SPIDMD dmd(1, 1);
DMD_TextBox box(dmd, 0, 0, 32, 7);
DMD_TextBox box2(dmd, 0, 8, 32, 8);  // end


// EEPROM library
#include <ESP_EEPROM.h>
  // end


// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;
// end

//
enum IncrementBy : uint8_t {
  SECONDS = 0,
  MINUTES,
  HOURS,
  DAYS,
  WEEKS,
};

struct EEPROMStruct {
  unsigned long timeStamp;
  uint32_t counter;
  IncrementBy incrementBy;
  uint16_t messageSize;
  uint32_t increment;
  char message[100];
} eeprom_data;

void SerialPrintEEPROMStruct() {
  Serial.println("-----------------------------------------");
  Serial.println("timeStamp");
  Serial.println(eeprom_data.timeStamp);
  Serial.println("counter");
  Serial.println(eeprom_data.counter);
  Serial.println("incrementBy");
  Serial.println(eeprom_data.incrementBy);
  Serial.println("messageSize");
  Serial.println(eeprom_data.messageSize);
  Serial.println("increment");
  Serial.println(eeprom_data.increment);
  Serial.println("message");
  Serial.println(eeprom_data.message);
  Serial.println("-----------------------------------------");
}

// global variables
String textMessage;
unsigned long counterTimeStamp;
int counter = 0;
uint32_t secs;
unsigned long initialMS = 0;
unsigned long display_count = 0;
unsigned long previousAnalogReadTime = 0;



void handleRoot() {
  String s = "<html lang='en'><head><meta charset='UTF-8' /><meta name='viewport' content='width=device-width, initial-scale=1.0' /><title>LEDART</title><style>label{width: 150px;display: inline-block;}input[type='submit']{width: 250px;margin: auto;margin-top: 20px;font-size: 25px;height: 40px;border-radius: 10px;}</style></head>";
  s += "<body style='height: 100vh;width: 100%;background-color: aliceblue;margin: 0;padding: 0;display: flex;flex-direction: column;justify-content: center;align-items: center;'>";
  s += "  <h1 style='letter-spacing: 2px; font-size: 60px'>LEDART</h1>";
  s += "<form action='/handleForm' method='post' style='display: flex;flex-direction: column;gap: 10px;justify-content: center;' >";
  s += "<div> <label style='font-size: 20px' for='text'>Message:</label><input style='border-radius: 10px;height: 40px;font-size: 20px;padding: 0 10px;' type='text' name='txt' ";
  s += "maxlength='99'";
  s += " value='";
  s += textMessage ? textMessage : "MSG";
  s += "'/></div>";
  s += "<div><label for='count' style='font-size: 20px'>Start Count:</label><input type='number' style='border-radius: 10px;height: 40px;font-size: 20px;padding: 0 10px;' name='count'";
  s += " value='";
  s += eeprom_data.counter;
  s += "'/></div>";
  s += "<div><label for='increment' style='font-size: 20px'>Timer Counter:</label><input type='number' style='border-radius: 10px;height: 40px;font-size: 20px;padding: 0 10px;' name='increment' ";
  s += " value='";
  s += eeprom_data.increment;
  s += "'/></div>";
  s += "<div style='display: flex'><div><div style='display: flex'><input type='radio' name='incrementBy' value='SECONDS' id='option1' style='height: 20px; width: 20px'";
  if (eeprom_data.incrementBy == 0) {
    s += " checked";
  };
  s += "/>";
  s += "<label for='option1' style='font-size: 20px'>Seconds</label></div>";
  s += "<div style='display: flex'><input type='radio' name='incrementBy' value='MINUTES' id='option2' style='height: 20px; width: 20px'";
  if (eeprom_data.incrementBy == 1) {
    s += " checked";
  };
  s += "/><label style='font-size: 20px' for='option2'>Minutes</label></div>";
  s += "<div style='display: flex'><input type='radio' style='height: 20px; width: 20px' name='incrementBy' value='HOURS'id='option3'";
  if (eeprom_data.incrementBy == 2) {
    s += " checked";
  };
  s += "/><label style='font-size: 20px' for='option3'>Hours</label></div>";
  s += "<div style='display: flex'><input type='radio' name='incrementBy' value='DAYS' id='option4' style='height: 20px; width: 20px'";
  if (eeprom_data.incrementBy == 3) {
    s += " checked";
  };
  s += "/><label for='option4' style='font-size: 20px'>Days</label></div>";
  s += "<div style='display: flex'><input type='radio' name='incrementBy' value='WEEKS' id='option5' style='height: 20px; width: 20px'";
  if (eeprom_data.incrementBy == 4) {
    s += " checked";
  };
  s += "/><label for='option5' style='font-size: 20px'>Weeks</label></div></div></div>";

  s += "<input type='submit' /></form></body></html>";

  server.send(200, "text/html", s);
}


void handlePlain() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    server.send(200, "text/plain", "POST body was:\n" + server.arg("plain"));
    Serial.println(server.arg("plain"));
    box.print(server.arg("plain"));
  }
}

IncrementBy incrementByStrToEnum(std::string incrementByStr) {
  IncrementBy incrementByEnum;
  if (incrementByStr.compare("SECONDS") == 0) {
    incrementByEnum = IncrementBy::SECONDS;
  } else if (incrementByStr.compare("MINUTES") == 0) {
    incrementByEnum = IncrementBy::MINUTES;
  } else if (incrementByStr.compare("HOURS") == 0) {
    incrementByEnum = IncrementBy::HOURS;
  } else if (incrementByStr.compare("DAYS") == 0) {
    incrementByEnum = IncrementBy::DAYS;
  } else if (incrementByStr.compare("WEEKS") == 0) {
    incrementByEnum = IncrementBy::WEEKS;
  } else {
    incrementByEnum = IncrementBy::SECONDS;
  }
  return incrementByEnum;
}
void handleForm() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    std::string message = server.arg("txt").c_str();
    bool isMessageChanged = false;
    bool isIncrementByChanged = false;
    bool isCounterChanged = false;
    bool isIncrementChanged = false;
    uint32_t countStart = server.arg("count").toInt();
    uint16_t increment = server.arg("increment").toInt();


    if (message.compare(eeprom_data.message) != 0) {
      isMessageChanged = true;
      Serial.println("in message changed");
      strncpy(eeprom_data.message, message.c_str(), sizeof(eeprom_data.message));
      eeprom_data.message[sizeof(eeprom_data.message) - 1] = '\0';
    }


    IncrementBy incrementByForm = incrementByStrToEnum(server.arg("incrementBy").c_str());
    if (eeprom_data.incrementBy != incrementByForm) {
      isIncrementByChanged = true;
      Serial.println("in incrementBy changed");
      now = rtc.now();
      eeprom_data.incrementBy = incrementByForm;
      eeprom_data.timeStamp = now.unixtime();
      countStart = 0;
      eeprom_data.counter = 0;
    }
    if (server.arg("count").toInt() != eeprom_data.counter) {
      isCounterChanged = true;
      Serial.println("in counter changed");
      Serial.println("form counter");
      Serial.println(server.arg("count").toInt());
      Serial.println("eeprom counter");
      Serial.println(eeprom_data.counter);
      countStart = server.arg("count").toInt();
      eeprom_data.counter = countStart;
    }
    if (server.arg("increment").toInt() != eeprom_data.increment) {
      isIncrementChanged = true;
      Serial.println("in increment changed");
      eeprom_data.increment = server.arg("increment").toInt();
    }

    if (isMessageChanged == true || isIncrementByChanged == true || isCounterChanged == true || isIncrementChanged == true) {
      EEPROM.put(0, eeprom_data);
      bool isCommitOk = EEPROM.commit();
      Serial.println((isCommitOk) ? "Form Commit OK" : "Commit failed");
      SerialPrintEEPROMStruct();
    }
    textMessage = convertCharToStr(eeprom_data.message);
    box.clear();
    box.print(textMessage);
    server.send(200, "text/plain", "Data sent Successfully please go back");
  }
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }
  server.send(404, "text/plain", message);
}
unsigned long millisAtSetup = 0;
void setup(void) {
  Serial.begin(115200);
  Serial.println(" ");
 if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  // uint16_t retryCount = 0;
  // Serial.println("Trying to connect to RTC");
  // while (1) {

  //   if(!rtc.begin())
  //   {
  //     Serial.println("RTC connection successfull!!!");
  //     break;
  //   }
  //   else
  //   {
  //     Serial.print(".");
  //     // Serial.flush();
  //     delay(100);
  //   }

  //   retryCount += 1;
  // }

  //EEPROM SECTION
  EEPROM.begin(sizeof(EEPROMStruct));
  EEPROM.get(0, eeprom_data);


  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(staticIP, gateway, subnet);
  WiFi.softAP(ssid, password, 6, 0);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //  WiFi.begin(ssid, password);
  // Serial.println("");
  // // Wait for connection
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  server.on("/", handleRoot);


  server.on("/handleForm", handleForm);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  // Serial.println(String(rtc.begin()));
  // if (!rtc.begin()) {
  //   Serial.println("Couldn't find RTC");
  //   // Serial.flush();
  //   while (!rtc.begin()) {
  //     Serial.print(".");
  //     delay(500);

  //   };
  // } else {
  //   Serial.println("Connected Successfully");
  // }
  now = rtc.now();
  unsigned long delta = now.unixtime() - eeprom_data.timeStamp;

  display_count = calcTimeDiff();
  dmd.setBrightness(255);
  dmd.selectFont(SystemFont5x7);
  dmd.begin();
  String printMessage = convertCharToStr(eeprom_data.message);
  textMessage = printMessage;
  box.print(printMessage);
  box2.print(display_count);
  SerialPrintEEPROMStruct();
  Serial.println("printMessage");
  Serial.println(printMessage);
  previousAnalogReadTime = millis();
  millisAtSetup = millis();
}

uint8_t scrollByCountRem = 0;
void loop(void) {
  server.handleClient();

  uint64_t timeDiff = calcTimeDiff();
  if (display_count != timeDiff) {
    display_count = timeDiff;
    box2.clear();
    box2.print(display_count);
  }
  unsigned long millisNow = millis();
  if ((millisNow - millisAtSetup) > 500) {
    readAnalogPinP0();
    millisAtSetup = millis();
  }
}

void readAnalogPinP0() {
  int analogValue = analogRead(A0);
  if (analogValue > 1000 && millis() - previousAnalogReadTime > 1000) {
    eeprom_data.timeStamp = now.unixtime();
    SerialPrintEEPROMStruct();
    EEPROM.put(0, eeprom_data);
    bool isCommitOk = EEPROM.commit();
    Serial.println((isCommitOk) ? "push button pressed  Commit OK" : "Commit failed");
    previousAnalogReadTime = millis();
  }
}

uint64_t calcTimeDiff() {
  now = rtc.now();
  unsigned long delta = now.unixtime() - eeprom_data.timeStamp;
  uint32_t remainder = 0;

  switch (eeprom_data.incrementBy) {
    case IncrementBy::SECONDS:
      break;

    case IncrementBy::MINUTES:
      remainder = delta % 60;
      delta -= remainder;
      delta = delta / 60;
      break;

    case IncrementBy::HOURS:
      remainder = delta % 3600;
      delta -= remainder;
      delta = delta / 3600;
      break;

    case IncrementBy::DAYS:
      remainder = delta % (3600 * 24);
      delta -= remainder;
      delta = delta / (3600 * 24);
      break;

    case IncrementBy::WEEKS:
      remainder = delta % (3600 * 24 * 7);
      delta -= remainder;
      delta = delta / (3600 * 24 * 7);
      break;
  }
  return (eeprom_data.counter + (delta / eeprom_data.increment));
}

String convertCharToStr(char buf[100]) {
  String temp = "";
  for (int i = 0; buf[i] != '\0' && i < 100; i++) {
    if (buf[i] != '\0') {
      temp += buf[i];
    }
  }
  return temp;
}
