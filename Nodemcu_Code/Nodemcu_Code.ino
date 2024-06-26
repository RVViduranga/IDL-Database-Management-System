#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif

#include <ESP8266WebServer.h>
#include <EEPROM.h>

//Variables
int offlineCount = 0;
const int resetButtonPin = D2;
int sensorPin = D1; // Declare a varible to digitalpin 1
int i = 0;
int del = 10;
int statusCode;
const char* ssid = "text";
const char* passphrase = "text";
String st;
String content;

//Function Decalration
bool testWifi(void);
void launchWeb(void);
void setupAP(void);

//Establishing Local server at port 80 whenever required
ESP8266WebServer server(80);

// Server's IP address/domain name and URL path
const char* serverName = "http://192.168.8.196/sensordata/post-esp-data.php";

// API Key value to be compatible with the PHP code 
String apiKeyValue = "tPmAT5Ab3j7F9";

String sensorName = "0006";

void setup() {
  Serial.begin(115200); // For debugging
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(sensorPin, INPUT);
  Serial.println();
  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); //Initialasing EEPROM
  delay(10);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  // Read eeprom for ssid and pass
  /*Serial.println("Reading EEPROM ssid");
  if (digitalRead(sensorPin) == HIGH) {
    eraseEEPROM();
  }*/

  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");

  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);


  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi())
  {
    Serial.println("Succesfully Connected!!!");
    return;
  }
  else
  {
    Serial.println("Turning the HotSpot On");
    launchWeb();
    setupAP();// Setup HotSpot
  }

  Serial.println();
  Serial.println("Waiting.");
  
  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(".");
    delay(100);
    server.handleClient();
  }
}

void loop() {
  
  int datam = digitalRead(sensorPin); // Read the digital pin from D1
  if (WiFi.status() == WL_CONNECTED) {
    if(offlineCount==0){
    
    WiFiClient client; // Create a WiFiClient object
    HTTPClient http;

    http.begin(client, serverName); // Begin with the WiFiClient object

    // Prepare HTTP POST request data
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData; 
    // Condition to take the string according to the sensor vale  
    if (datam == HIGH) {
      Serial.println("Motion Detected");
      String b = "Motion Detected";
      httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName + "&value1=" + b + "";
      while(true){
        int datam = digitalRead(sensorPin);
        delay(del);
        if(datam == LOW){
          break;
        }
      }
     }
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);

    http.end();
    }
    else{
      for(int j=0;j<offlineCount;j++){
        WiFiClient client; // Create a WiFiClient object
        HTTPClient http;

        http.begin(client, serverName); // Begin with the WiFiClient object

        // Prepare HTTP POST request data
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        String httpRequestData; 
        // Condition to take the string according to the sensor vale  
        Serial.println("Motion Detected");
        String c = "Motion Detected";
        httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName + "&value1=" + c + "";
        Serial.print("httpRequestData: ");
        Serial.println(httpRequestData);
        int httpResponseCode = http.POST(httpRequestData);
        
        http.end();
        //delay(100);
    
      }
     offlineCount = 0;
    }
  }
  else {
    Serial.println("WiFi Disconnected");
    if (datam == HIGH) {
      Serial.println("Motion Detected");
      while(true){
        int datam = digitalRead(sensorPin);
        delay(del);
        if(datam == LOW){
          break;
        }
      }
     offlineCount++;
     }
  } 

}

bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}

void launchWeb()
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}

void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);

    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.softAP("Sensor 0006", "");
  Serial.println("softap");
  launchWeb();
  Serial.println("over");
}

void createWebServer()
{
 {
    server.on("/", []() {

      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content);
    });
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });

    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");

        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();

        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.reset();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);

    });
  } 
}
/*void eraseEEPROM() {
  Serial.println("Reset button pressed. Erasing EEPROM...");
  
  // Clear Wi-Fi credentials from EEPROM
  for (int i = 0; i < 96; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();

  Serial.println("EEPROM erased.");
  
}*/
