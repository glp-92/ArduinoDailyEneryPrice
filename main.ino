#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
  
#define ssid "XXX"
#define password "XXX"
/*
 * On telegram web, join chat en copy after '-'
*/
#define chat_id "XXX"
#define BOT_token "XXX"

StaticJsonDocument<4096> doc;
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_token, secured_client);

String payload;
char* hours[] = {"00-01", "01-02", "02-03", "03-04", "04-05", "05-06", "06-07", 
"07-08", "08-09", "09-10", "10-11", "11-12", "12-13", "13-14", "14-15", "15-16", "16-17", 
"17-18", "18-19", "19-20", "20-21", "21-22", "22-23", "23-24"};
byte hours_size = sizeof(hours) / sizeof(hours[0]);


void setup() {
  
  Serial.begin(115200);
  delay(4000);
  WiFi.begin(ssid, password);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println("Connected to the WiFi network");
  
}
  
void loop() {
  
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    HTTPClient http;
    http.begin("https://api.preciodelaluz.org/v1/prices/all?zone=PCB"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
    if (httpCode > 0) { //Check for the returning code
        payload = http.getString();
      }
    else {
      Serial.println("Error on HTTP request");
    }
    http.end(); //Free the resources
    DeserializationError error = deserializeJson(doc, payload);

    payload = "";
    
    for (int i = 0; i < hours_size; i++) {
      String key = String(hours[i]);
      float price = doc[key]["price"];
      String price_kw = String(price / 10.00);
      payload += key + " => " + price_kw + "€/kWh\n";
    }
    

    Serial.println("Sending data with bot...");
    bool success = bot.sendMessage(chat_id, payload, "");
  }
  
  delay(60000);
  
}