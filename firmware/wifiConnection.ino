
//Wifi Settings
String  wifi_ssid             = "";
String  wifi_password         = "";
int     wifi_counter          = 0;

String  wifi_default_ssid     = "gogo_ddw";
String  wifi_default_password = "ddw_gogo";

void connectWifi(){
  
  readWifi();
  checkEmptyWifiInfo();
  ledOn();
  debugPrint();
  debugPrintStart("WIFI\t Connecting to ");
  debugPrint(wifi_ssid);
  //debugPrint(wifi_password);
  
  if (wifi_password.length() > 0){
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  } else {
    WiFi.begin(wifi_ssid.c_str());
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    if (wifi_counter++ >= 30){
      wifi_counter = 0;
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED){
    
    debugPrint();
    debugPrint("WIFI\t connected");  
    debugPrintStart("WIFI\t IP address: ");
    Serial.println(WiFi.localIP());
    debugPrint();
    
    // Start sync time
    timeClient.begin();
    
  }
  
  ledOff();
  
}

void checkEmptyWifiInfo(){
  
  if ( (wifi_ssid == "") || ( wifi_ssid.length() > 0 && wifi_ssid.charAt(0) == 255 ) ) {
    wifi_ssid       = wifi_default_ssid;
    wifi_password   = wifi_default_password;
  }
  
}

void saveWifi(String qsid, String qpass){
  
  ledOn();
  
  debugPrint("EEPROM\t clearing");
  for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
  
  debugPrint("EEPROM\t write\t ssid:");
  for (int i = 0; i < qsid.length(); ++i)
  {
    EEPROM.write(i, qsid[i]);
  }
  
  debugPrint("EEPROM\t write\t pass:"); 
  
  for (int i = 0; i < qpass.length(); ++i)
  {
    EEPROM.write(32+i, qpass[i]);
  }
  
  EEPROM.commit();
  ledOff();
}

void readWifi(){
  ledOn();
  debugPrint("EEPROM\t read\t SSID");
  String eessid;
  for (int i = 0; i < 32; ++i)
  {
    eessid += char(EEPROM.read(i));
  }
  debugPrintStart("\t SSID\t ");
  debugPrint(eessid);
 
  debugPrint("EEPROM\t read\t PASS");
  String eepass = "";
  for (int i = 32; i < 96; ++i)
  {
    eepass += char(EEPROM.read(i));
  }
  debugPrintStart("\t PASS\t ");
  debugPrint(eepass);  
  
  if ( eessid.length() > 1 ) {
    wifi_ssid = (String) eessid.c_str();
    wifi_password = (String) eepass.c_str();

    wifi_ssid.trim();
    wifi_password.trim();
  }
  ledOff();
}

void saveThingSpeakAPI(String apiKey){
  
  ledOn();
  debugPrint("EEPROM\t clearing");
  for (int i = 96; i < 160; ++i) { EEPROM.write(i, 0); }
  
  debugPrint("EEPROM\t write\t API key");
  for (int i = 0; i < apiKey.length(); ++i)
  {
    EEPROM.write(96+i, apiKey[i]);
  }
  
  EEPROM.commit();
  ledOff();
  
}

void readThingSpeakAPI(){
  ledOn();
  debugPrint("EEPROM\t read\t API key");
  String eeAPIKey;
  for (int i = 96; i < 160; ++i)
  {
    eeAPIKey += char(EEPROM.read(i));
  }
  
  debugPrintStart("\t API key\t ");
  debugPrint(eeAPIKey);  
  if ( eeAPIKey.length() > 1 ) {
    writeAPIKey = eeAPIKey.c_str();
    writeAPIKey.trim();
  }
  ledOff();
}
