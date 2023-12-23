//CT1
#include <dhtnew.h>
#include <DS3231.h>
#include <Wire.h>
DS3231 myRTC;
DHTNEW mySensor(3);
bool century = false;
bool h12Flag;
bool pmFlag;

int cameranumber = 01;
int address = 10014;

int sensorPin = 2;
unsigned long lastChangeTime = 0;
int HBcount = 0;
bool objectDetected = true;

String LD="";

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Wire.begin();
  pinMode(sensorPin, INPUT);//微波
  Serial.print("AT\r\n");
  while(Serial.available()) {
    String AT = Serial.readStringUntil('\n');
    Serial.println(AT);
  }
}


void loop() {
  String cmd="";
  String pi_rec="";
  String pi_sen="";

  int microwaveValue = readMicrowaveSensor();

  if (microwaveValue == 1&& objectDetected == true) {//==>
    pi_sen="ON";
    Serial1.println(pi_sen);
    LD = generateLD();
    //Serial.println(LD);
    objectDetected = false;
  }
  
  while(Serial1.available()) {
    pi_rec = Serial1.readStringUntil('\n');
    if(pi_rec=="0")
    {
      return;
      }
    if(pi_rec!="")
    {
      LD = LD + pi_rec + "J~";
      cmd = "AT+SEND=" + String(address) + "," + String(LD.length()) + "," + LD + "\r\n";
      }
    
  }
  
  //cmd = "AT+SEND=" + String(address) + "," + String(LD.length()) + "," + LD + "\r\n";
  if(pi_rec!="")//&&objectDetected == true
  {
    Serial.print(cmd); 
  }
}

int readMicrowaveSensor() {
  int sensorValue = digitalRead(sensorPin);
  if (sensorValue == LOW ) {
    HBcount = 1;
    lastChangeTime = millis(); 
    //Serial.println(HBcount);
  } else if (sensorValue == HIGH ) {
    if(millis() - lastChangeTime > 1000)
    {
      HBcount = 0;
      objectDetected = true;
      }
    //Serial.println(HBcount);
  }  
  return HBcount;
}

String generateLD() {
  mySensor.read();  
  String ny=FixTime(String(myRTC.getYear(), DEC));
  String nmo=FixTime(String(myRTC.getMonth(century), DEC));
  String nd=FixTime(String(myRTC.getDate(), DEC));
  String nh=FixTime(String(myRTC.getHour(h12Flag, pmFlag), DEC));
  String nmi=FixTime(String(myRTC.getMinute(), DEC));
  String ns=FixTime(String(myRTC.getSecond(), DEC));
  float temperature = mySensor.getTemperature();
  float pressure = 1012.86;
  float humidity = mySensor.getHumidity();
  String LDD = "~A20" + ny + 
                     nmo + 
                     nd + 
              "B" + nh + 
                    nmi + 
                    ns +
              "C" + String(cameranumber)+      
              "D" + String(temperature, 1) +
              "E" + String(humidity, 1) +
              "F" + String(pressure, 2) ;

  return LDD;
}
String FixTime(String inputDate) {
  // 處理日期，如果是個位數，則在前面添加0
  String Date="";
  if (inputDate.length() == 1) {
    Date = "0" + inputDate;
  }
  else{
    Date=inputDate;
  }
  return Date;
}
