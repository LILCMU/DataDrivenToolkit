
//Wifi Settings
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
