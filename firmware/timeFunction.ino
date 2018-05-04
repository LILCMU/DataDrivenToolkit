
String dateTimeString(){

  unsigned long epoch =  timeClient.getEpochTime();

//  if (now() < timeSaved){
//    Serial.print("get");
//    updateTimeFromMem();
//  } else 

  if (epoch > now()){
    updateTimeFromNtp();
  } else {
    saveTime(now());
  }
  
  String dateTime = "";
  
  dateTime += year();
  dateTime += "-";
  dateTime += correctDigits(month());
  dateTime += "-";
  dateTime += correctDigits(day());
  dateTime += " ";
  dateTime += correctDigits(hour());
  dateTime += ":";
  dateTime += correctDigits(minute());
  dateTime += ":";
  dateTime += correctDigits(second());

  return dateTime;
}

unsigned int getYear(){
  unsigned long epoch =  timeClient.getEpochTime();
  return year(epoch);
}

String correctDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if(digits < 10)
    return "0"+String(digits);
  return String(digits);
}

void showDateTime(){
  debugPrint("Time\t "+dateTimeString());
}


void updateTimeFromNtp(){
  setTime(timeClient.getEpochTime());
}

void updateTimeFromMem(){
  timeSaved = readTime()+30;
  debugPrintStart("Time\t restore\t ");
  Serial.println(timeSaved);
  setTime(timeSaved);
}

