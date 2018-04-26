

String dateTimeString(){

  unsigned long epoch =  timeClient.getEpochTime();
  String dateTime = "";
  
  dateTime += year(epoch);
  dateTime += "-";
  dateTime += correctDigits(month(epoch));
  dateTime += "-";
  dateTime += correctDigits(day(epoch));
  dateTime += " ";
  dateTime += timeClient.getFormattedTime();
  
  return dateTime;
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

