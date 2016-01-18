

String dateTimeString(){

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

String correctDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if(digits < 10)
    return "0"+String(digits);
  return String(digits);
}


