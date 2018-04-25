
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
#include <Ticker.h>
#include <HashMap.h>
#include <TimeLib.h> 

#define firmware_v    1.0
#define time_zone     0
#define led_pin       16

// ------------ Time variables
Ticker          second_tick;
Ticker          second_tick2;
unsigned int    localPort             = 8888;      // local port to listen for UDP packets
const char*     ntpServerName         = "time.nist.gov";
IPAddress       timeServerIP(132, 163, 96, 1);    // time.nist.gov NTP server address

const int       NTP_PACKET_SIZE     = 48; // NTP time stamp is in the first 48 bytes of the message
byte            packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
WiFiUDP         udp;
unsigned int    countSynctime         = 0;

// ------------ ThingSpeak Settings
//char          thingSpeakAddress[]   = "api.thingspeak.com";
char            thingSpeakAddress[]   = "data.learninginventions.org";
int             thingSpeakPort        = 80;
String          writeAPIKey           = "";
const int       sendingInterval       = 1 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)
String          fieldAndValues        = "";

// ------------ Variable Setup
long            lastConnectionTime    = 0; 
boolean         lastConnected         = false;
int             failedCounter         = 0;
int             value                 = 0;
boolean         countBlink            = 0;

// ------------ SDcard
boolean         hasSDCard             = false;

// ------------ GoGo constant
const int       gogo_command          = 5;
const int       gogo_wifi             = 218;
const int       gogo_record           = 223;


// ------------ Wifi
WiFiClient client;

// ------------ Hash and Storage
//define the max size of the hashtable
const byte            HASH_SIZE        = 10; 
HashType<char*,int>   hashRawArray[HASH_SIZE]; 
//handles the storage [search,retrieve,insert]
HashMap<char*,int>    hashMap = HashMap<char*,int>( hashRawArray , HASH_SIZE ); 
bool                  isDataHandled          = false;
byte                  hashMapSize            = 0;

String                inputString = "";         // a string to hold incoming data
boolean               stringComplete = false;  // whether the string is complete

void debugPrint(String msg=""){
  Serial.println(msg);
}

void setup()
{
  
  inputString.reserve(200);
  
  delay(100);
  // Start Serial for debugging on the Serial Monitor
  Serial.begin(115200);

  EEPROM.begin(512);
  pinMode(led_pin,OUTPUT);
  //saveWifi("gogo", "");
  //Read wifi config from EEPROM
  debugPrint("Firmware Version " + String(firmware_v));
  debugPrint();
  readThingSpeakAPI();

  //Connecting to a WiFi network
  connectWifi();
  delay(100);

  ledOn();

  if (WiFi.status() == WL_CONNECTED){
    startSyncTime();
  }
  
   //Initialize SD Card
  initSDCard(); 
  
  ledOff();
  delay(100);
  debugPrint("Ready");
  second_tick.attach(0.5, tick);
  //second_tick2.attach(5, tick2);

  inputString = "";

}

void splitString(String longString){
  int index = -1;

  do {
    index = longString.indexOf('\n');
    //debugPrint(String(index));
    String cutString = longString.substring(0,index);
    //debugPrint(cutString);
    extractValue(cutString);
    longString = longString.substring(index+1);
  } while (index != -1);
  
  extractValue(longString);
  
}

void loop()
{

  /*
  if (Serial.available()) {
    String inString = Serial.readString();
    extractValue(inString);
  }
  */
  serialEvent(); //call the function
  
  //handle the string when a newline arrives:
  if (stringComplete) {

    //check has more than 1 packet

    splitString(inputString);

    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  

  /*
  // Print Update Response to Serial Monitor
  if (client.available())
  {
    while ( client.available() > 0 ) {
      char c = client.read();
      Serial.print(c);
    }
  }
  */
  
  if (!client.connected() && lastConnected)
  {
    debugPrint("...disconnected");
    debugPrint();
    client.stop();
  }

  // Read value from Analog Input Pin 0
  //String strValue0 = (String) value ;
  //String analogValue0 = String(analogRead(A0), DEC);

  // Send data
      if(!client.connected() && (millis() - lastConnectionTime > sendingInterval))
  {
    lastConnectionTime = millis();
    // debugPrint("------------------------------");
    // debugPrint(fieldAndValues);
    
    //writeDataToSDCard(field, String(value));
    if (WiFi.status() == WL_CONNECTED && fieldAndValues != ""){
      if (updateThingSpeakGet(fieldAndValues)){
        //clearHashmap();
        debugPrint("ok");
      } else {
        debugPrint("not ok");
      }
      clearHashmap();
    } else if (WiFi.status() != WL_CONNECTED){
      //ledToggle();
    }

  }
  delay(1);
  
  //lastConnected = client.connected();
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void ledOn(){
  digitalWrite(led_pin,LOW);
}

void ledOff(){
  digitalWrite(led_pin,HIGH);
}

void ledToggle(){
  digitalWrite(led_pin, !digitalRead(led_pin));
}


void initSDCard(){
  debugPrint("Initializing SD card...");

  if (!SD.begin(4)) {
    debugPrint("initialization failed!");
  } else {
    hasSDCard = true;
    debugPrint("initialization done.");
  }
}


int extractValue(String data){
  ledOn();
  data.trim();

  //Check the command Packet
  if ( data.charAt(0)== gogo_command ){
    int firstComma = data.indexOf(',');

    //Check type Data Logging
    if (data.charAt(1)== gogo_record ){
      
      String  field = data.substring(2,firstComma);
      String  value = data.substring(firstComma+1);
      handleRecordData(field, value);

    //Check type Wifi Config
    } else if ( data.charAt(1)== gogo_wifi ){
      
      String  inssid = data.substring(2,firstComma);
      String  inpass = data.substring(firstComma+1);

      //API setting
      if (inssid.equals("thingSpeakAPIKey")){
        
        saveThingSpeakAPI(inpass);
        readThingSpeakAPI();

      } else {
        
        debugPrint("SSID : " + inssid);
        debugPrint("Pass : " + inpass);
        saveWifi(inssid, inpass);
        debugPrint("------------------------------");
        connectWifi();
        
      }
    }
  }
  ledOff();
}

void handleRecordData(String field, String value){
  //debugPrint(field);
  //field.trim();
  //value.trim();
  
  char* fieldChar;
  int indexOfField;
  for (int i=0;i<sizeof(field);i++){
    fieldChar += field.charAt(i);
  }
  //field.toCharArray(fieldChar,sizeof(field));
  //debugPrint("-----------------------------------");
  indexOfField = hashMap.getIndexOf(fieldChar);
  //debugPrint(String(indexOfField));
  //hashMap.debug();
  //hashMap[hashMapSize](fieldChar,true);
  writeDataToSDCard(field, value);
  if (indexOfField == 0 && hashMapSize < HASH_SIZE){

     hashMap[++hashMapSize](fieldChar,value.toInt());
     //hashMapSize++;
     if (indexOfField < HASH_SIZE)
        hashMap[hashMapSize+1](fieldChar,-1);
     //debugPrint("\tnew");
     
  } else if (hashMap.getValueOf(fieldChar) > -1 ) {
    //debugPrint("\tin array");
    return;
  }
/*
  for (int i=0;i<hashMapSize;i++){
    debugPrint(arr[i]);
  }
  */
  //debugPrint("\tok");
  
  String  fieldAndValue = "&" + field + "=" + value;
  
//  if (hashMapSize > 0){
//    fieldAndValues += "&";
//  }
  fieldAndValues += fieldAndValue;
  //writeDataToSDCard(field, value);
  return;
  //debugPrint("------------------------------");
  debugPrint(fieldAndValue);
  //writeDataToSDCard(field, value);
  if (WiFi.status() == WL_CONNECTED){
    if (updateThingSpeakGet(fieldAndValues)){
      debugPrint("ok");
    } else {
      debugPrint("not ok");
    }
  }
      
}
boolean updateThingSpeakGet(String tsData)
{
  tsData.trim();
  //debugPrint("Connecting to sp.");
  //debugPrint(tsData);
  if (client.connect(thingSpeakAddress, thingSpeakPort))
  {
    String url = "/update?key=";
    url += writeAPIKey;
    //url += "&";
    url += tsData;

    // debugPrint(url);
    
    client.print(String("GET ") + url + " HTTP/1.1\r\n");
    client.print("Host: "+ String(thingSpeakAddress) +"\r\n");
    //client.print("Host: api.thingspeak.com\r\n");
    client.print("Connection: close\r\n");
    client.print("\r\n\r\n");
    String line = "";
    lastConnectionTime = millis();

    if (client.connected())
    {
      debugPrint("Sending : "+tsData);
      //debugPrint("Connecting to ThingSpeak...");
      failedCounter = 0;
      
      while(!client.available()){ //Wait until response
        delay(20);
        Serial.print("+");
      }

      while(client.available()){ // Store the response msg to line
        line += (char)client.read();
        delay(20);
        Serial.print("-");
      }
      
      //debugPrint(line);
      int response = convertDataToInt(line);
      debugPrint(String (response) );
      return (response > 0 );
    }
    else
    {
      failedCounter++;

      debugPrint("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      debugPrint();
      return false;
    }

  }
  else
  {
    failedCounter++;

    debugPrint("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    debugPrint();

    lastConnectionTime = millis(); 
  }
  return false;
}

boolean updateThingSpeak(String tsData)
{
  if (client.connect(thingSpeakAddress, thingSpeakPort))
  {         
    client.print("POST /update HTTP/1.1\r\n");
    //client.print("Host: api.thingspeak.com\r\n");
    client.print("Host: "+ String(thingSpeakAddress) +"\r\n");
    client.print("Connection: close\r\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\r\n");
    client.print("Content-Type: application/x-www-form-urlencoded\r\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\r\n\r\n");

    client.print(tsData);

    lastConnectionTime = millis();

    if (client.connected())
    {
      debugPrint("Connecting to ThingSpeak...");
      debugPrint();

      failedCounter = 0;
      return true;
    }
    else
    {
      failedCounter++;

      debugPrint("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      debugPrint();
      return false;
    }

  }
  else
  {
    failedCounter++;

    debugPrint("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    debugPrint();

    lastConnectionTime = millis(); 
  }
  return false;
}



void writeDataToSDCard(String data_name,String data_value){
  if (!hasSDCard)
    return;
    
  File myFile;
  data_name.trim();
  String fileName = data_name + ".csv";
  //debugPrint((String) SD.exists(fileName));
  /*if (!SD.exists(fileName)) {
    debugPrint("create a file "+ fileName);
    myFile = SD.open(fileName, FILE_WRITE);
    if (myFile) {
      myFile.println("DateTime," + data_name);
      myFile.close();
    }
  }
  */

  myFile = SD.open(fileName, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    //debugPrint("Writing to "+ fileName);
    myFile.println(dateTimeString() + "," + data_value);
    myFile.close();

  // if the file didn't open, print an error:
  } else {
    debugPrint("error opening " + fileName);
  }
}

int convertDataToInt(String rawData){
  int responseLength;
  int responseVal;

  // Serial.println(rawData);

  int indexContent = rawData.indexOf("close");
  // debugPrint("index = ");
  // debugPrint(String(indexContent));

  // Invalid data
  if (indexContent == -1) {
    return 0;
  }

  //Get response number
  String dataSub = rawData.substring(indexContent+47);
  dataSub.trim();
  responseVal = dataSub.toInt();
  return responseVal;
}

void clearHashmap(){
  fieldAndValues = "";
  hashMap = HashMap<char*,int>( hashRawArray , HASH_SIZE );
  hashMapSize =0;
}

void tick (void)
{
  countSynctime++;
  if (WiFi.status() == WL_CONNECTED){
    ledToggle();

    // Every (5 min *60)/0.5
    if (countSynctime >= 120) {
      countSynctime = 0;
    // startSyncTime();
      debugPrint(dateTimeString());
    }
    
  } else {
    
    if (countBlink==1){
      ledOn();
    } else if (countBlink==2){
      ledOff();
    } else if (countBlink>3){
      countBlink=0;
    }
    countBlink++;
  }
  return;
}

void tick2 (void)
{

  debugPrint("=========================");
  if (WiFi.status() == WL_CONNECTED){
    if (fieldAndValues != ""){
      
      debugPrint(fieldAndValues);
      if (updateThingSpeakGet(fieldAndValues)){
        debugPrint("ok");
      } else {
        debugPrint("not ok");
        ESP.reset();
      }
      
      fieldAndValues = "";
      hashMap = HashMap<char*,int>( hashRawArray , HASH_SIZE );
      hashMapSize =0;
    }
  }
  return;
}



