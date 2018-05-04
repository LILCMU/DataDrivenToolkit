
#define DEF_LENGTH            64

#define MEM_WIFI_SSID         0
#define MEM_WIFI_PASS         64
#define MEM_APIKEY            128
#define MEM_TIME              192

void EEPROMWritelong(int address, long value){
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four   = (value & 0xFF);
  byte three  = ((value >> 8) & 0xFF);
  byte two    = ((value >> 16) & 0xFF);
  byte one    = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
  EEPROM.commit();
}

long EEPROMReadlong(long address){
  //Read the 4 bytes from the eeprom memory.
  long four   = EEPROM.read(address);
  long three  = EEPROM.read(address + 1);
  long two    = EEPROM.read(address + 2);
  long one    = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void EEPROMClearSpace(unsigned int startAddr, unsigned int endAddr){
  debugPrint("EEPROM\t clearing");
  for (int i = startAddr; i < endAddr; i++) { EEPROM.write(i, 0); }
  EEPROM.commit();
}

String EEPROMReadString(unsigned int startAddr, unsigned int endAddr){
  String result = "";
  for (int i = startAddr; i < endAddr; i++)
  {
//    Serial.println(i);
    result += char(EEPROM.read(i));
  }
  result = (String) result.c_str();
  result.trim();
  return result;
}

void EEPROMWriteString(unsigned int startAddr, unsigned int packetLength, String packetText){
  
  ledOn();

  if (packetLength > DEF_LENGTH) { packetLength = DEF_LENGTH; }

  EEPROMClearSpace(startAddr, startAddr+DEF_LENGTH);
  
  debugPrint("EEPROM\t write ");  
  debugPrintStart("\t ");
  debugPrint(packetText);
  
  for (int i = 0; i < packetLength; i++)
  {
    EEPROM.write(startAddr+i, packetText[i]);
  }
  
  EEPROM.commit();
  
  ledOff();
  
}

void saveWifi(String qsid, String qpass){
  ledOn();
  
  EEPROMWriteString(MEM_WIFI_SSID, MEM_WIFI_SSID+qsid.length(), qsid);
  EEPROMWriteString(MEM_WIFI_PASS, MEM_WIFI_PASS+qpass.length(), qpass);
  
  ledOff();
}

void readWifi(){
  ledOn();
  
  debugPrint("EEPROM\t read\t SSID");
  String eessid = EEPROMReadString(MEM_WIFI_SSID, MEM_WIFI_SSID+DEF_LENGTH);
  debugPrintStart("\t SSID\t ");
  debugPrint(eessid);
 
  debugPrint("EEPROM\t read\t PASS");
  String eepass = EEPROMReadString(MEM_WIFI_PASS, MEM_WIFI_PASS+DEF_LENGTH);
  debugPrintStart("\t PASS\t ");
  debugPrint(eepass);  
  
  if ( eessid.length() > 0 ) {
    wifi_ssid = eessid;
    wifi_password = eepass;
  }
  
  ledOff();
}

void saveThingSpeakAPI(String apiKey){  
  ledOn();

  EEPROMWriteString(MEM_APIKEY, MEM_APIKEY+apiKey.length(), apiKey);
  
  ledOff();
}

void readThingSpeakAPI(){
  
  debugPrint("EEPROM\t read\t APIkey ");
  String eeAPIKey = EEPROMReadString(MEM_APIKEY, MEM_APIKEY+DEF_LENGTH);
  debugPrintStart("\t APIkey\t ");
  debugPrint(eeAPIKey);  
  if ( eeAPIKey.length() > 1 ) {
    writeAPIKey = eeAPIKey;
  }

}

void saveTime(unsigned long timestamp) {
  EEPROMWritelong(MEM_TIME, timestamp);
  timeSaved = timestamp;
}

unsigned long readTime() {
  return EEPROMReadlong(MEM_TIME);
}

// ------------ SDcard ------------

void initSDCard(){
  debugPrint("SDCard\t Initializing...");

  if (!SD.begin(4)) {
    debugPrint("SDCard\t initialization failed!");
  } else {
    hasSDCard = true;
    debugPrint("SDCard\t initialization done.");
  }
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
    debugPrint("SDCard\t error opening " + fileName);
  }
  
}

