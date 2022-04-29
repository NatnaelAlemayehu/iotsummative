#include <DHT.h>  // Including library for dht
 
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <ESP_Mail_Client.h>

String apiKey = "YRCJ5QBAQMOVZGY9";     //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "Natis";     // replace with your wifi ssid and wpa2 key
const char *pass =  "Mywifi@123";
const char* server = "api.thingspeak.com";

#define DHTPIN 4         //pin where the dht11 is connected

#define SMTP_server "smtp.gmail.com"
#define SMTP_Port 465

#define sender_email "espmailclient@gmail.com"
#define sender_password "Summative2022"

#define Recipient_email "natnaelzewdu10@gmail.com"

SMTPSession smtp;

static const uint8_t D8 = 15;
int moisture = A0;


DHT dht(DHTPIN, DHT11);
int relaypin = D8;
int fan_autoswitch = 0;

 
WiFiClient client;
 
void setup() 
{
       Serial.begin(115200);
       delay(10);
       dht.begin();
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 
}
 
void loop() 
{
         smtp.debug(1);
 ESP_Mail_Session session;
 session.server.host_name = SMTP_server ;
 session.server.port = SMTP_Port;
 session.login.email = sender_email;
 session.login.password = sender_password;
// session.login.user_domain = "";

    /* Declare the message class */
  SMTP_Message message;
  
  
      float moist = analogRead(moisture);
      float temp = dht.readTemperature();
      
              if (isnan(moist) || isnan(temp)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
 
                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(temp);
                             postStr +="&field2=";
                             postStr += String(fan_autoswitch);
                             postStr +="&field3=";
                             postStr += String(moist);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             
                        }
          client.stop();
 
          Serial.println("Waiting...");
          
    if(temp > 25){
             digitalWrite(relaypin, HIGH);
             fan_autoswitch = 1;
          }else{
             digitalWrite(relaypin, LOW);
             fan_autoswitch = 0;
             delay(2000);
          }

    if(moist < 15){
    message.sender.name = "AGROTECH_ESP8266_SUMMATIVE";
    message.sender.email = sender_email;
    message.subject = "Soil Moisture: Status";
    message.addRecipient("Farmer",Recipient_email);

    //Send simple text message
    String textMsg = "The moisture level is low, you should turn on the irrigation system";
    message.text.content = textMsg.c_str();
    message.text.charSet = "us-ascii";
    message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
    if (!smtp.connect(&session))
      return;
  
    if (!MailClient.sendMail(&smtp, &message))
      Serial.println("Error sending Email, " + smtp.errorReason());
  }