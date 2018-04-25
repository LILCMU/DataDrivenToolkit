
//Wifi Settings
String  wifi_ssid     = "";
String  wifi_password = "";
int     wifi_counter  = 0;

String  wifi_default_ssid     = "gogo_ddw";
String  wifi_default_password = "ddw_gogo";

void connectWifi(){
  readWifi();
  checkEmptyWifiInfo();
  ledOn();
  debugPrint();
  debugPrint("Connecting to ");
  debugPrint("\t"+wifi_ssid);
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
    debugPrint("\tWiFi connected");  
    debugPrint("\tIP address: ");
    Serial.print(WiFi.localIP());
    debugPrint();
    startSyncTime();
    
  }
  
  ledOff();
  
}

void startSyncTime() {

  // Show time
//  if (timeStatus() != timeNotSet) {
//    debugPrint(dateTimeString());
//    return;
//  }


//  if (udp.available() ==0) {
    udp.begin(localPort);
    delay(300);
//  }

  debugPrint("Start sync time");
  setSyncProvider(getNtpTime);
  delay(200);
  debugPrint(dateTimeString());

}

void checkEmptyWifiInfo(){
  
  if ( (wifi_ssid == "") || ( wifi_ssid.length() > 0 && wifi_ssid.charAt(0) == 255 ) ) {
    wifi_ssid       = wifi_default_ssid;
    wifi_password   = wifi_default_password;
  }
  
}

void saveWifi(String qsid, String qpass){
  ledOn();
  
  debugPrint("clearing eeprom");
  for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
  
  debugPrint("writing eeprom ssid:");
  for (int i = 0; i < qsid.length(); ++i)
  {
    EEPROM.write(i, qsid[i]);
  }
  
  debugPrint("writing eeprom pass:"); 
  for (int i = 0; i < qpass.length(); ++i)
  {
    EEPROM.write(32+i, qpass[i]);
  }    
  EEPROM.commit();
  ledOff();
}

void readWifi(){
  ledOn();
// read eeprom for ssid and pass
  debugPrint("Reading EEPROM ssid");
  String eessid;
  for (int i = 0; i < 32; ++i)
  {
    eessid += char(EEPROM.read(i));
  }
  Serial.print("\tSSID: ");
  debugPrint(eessid);
 
  debugPrint("Reading EEPROM pass");
  String eepass = "";
  for (int i = 32; i < 96; ++i)
  {
    eepass += char(EEPROM.read(i));
  }
  Serial.print("\tPASS: ");
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

  debugPrint("clearing eeprom");
  for (int i = 96; i < 160; ++i) { EEPROM.write(i, 0); }
  
  debugPrint("writing eeprom API KEY");
  for (int i = 0; i < apiKey.length(); ++i)
  {
    EEPROM.write(96+i, apiKey[i]);
  }
  
  EEPROM.commit();
  ledOff();
}

void readThingSpeakAPI(){
  // read eeprom for ssid and pass
  ledOn();
  
  debugPrint("Reading EEPROM API");
  String eeAPIKey;
  for (int i = 96; i < 160; ++i)
  {
    eeAPIKey += char(EEPROM.read(i));
  }
  
  Serial.print("\tEEPROM API KEY: ");
  debugPrint(eeAPIKey);  
  if ( eeAPIKey.length() > 1 ) {
    writeAPIKey = eeAPIKey.c_str();
    writeAPIKey.trim();
  }
  ledOff();
}

time_t getNtpTime()
{
  while (udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  
  // WiFi.hostByName(ntpServerName, timeServerIP); 
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + time_zone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
