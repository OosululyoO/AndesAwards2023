/*CT1*/
//#define SSID "D6A0"
//#define PASS "river"
//#define IP "172.20.10.2"
//#define SSID "123"
//#define PASS "12345678"
//#define IP "172.20.10.2" // ThingSpeak IP Address: 184.106.153.149
// 使用 GET 傳送資料的格式
//String wifi="AT+CWJAP=\"csie-614\",\"csiewifi@614\"\r\n";
//String TCP = "AT+CIPSTART=\"TCP\",\"192.168.50.73\",8080\r\n";
String wifi = "AT+CWJAP=\"D6A0\",\"river0316\"\r\n";
String TCP = "AT+CIPSTART=\"TCP\",\"192.168.0.24\",8080\r\n";
String GET = "GET http://localhost:8080/專題/0824_dht.php?";

//String response = "AT+SEND=10014,51,~A20231219B153442C1D26.50E53.61F1012.86G33H1000I1H0001I2H0001I3J~";
String response="";
String allinfo1 = "";
String A, B, C, D, E, F, H, G, I;
String combinedValue = "";
int countH = 0;
const int ledPin = 7;
String Getallinfo(String inputString) { //提取所有的資料
  int start = inputString.indexOf('~'); // 找到第一个波浪符號的位置
  int end = inputString.indexOf('~', start + 1); // 从第一个波浪符號後面找到第二個波浪符號的位置

  if (start != -1 && end != -1 && end > start) {
    return inputString.substring(start + 1, end); // 提取两個波浪符號之間的資料並返回
  } else {
    return "未找到数据"; // 如果未找到两個波浪符號之間的資料，返回錯誤消息
  }
}

int countHInString(String inputString) { //抓H個數
  int count = 0;
  for (unsigned int i = 0; i < inputString.length(); i++) {
    if (inputString.charAt(i) == 'H') {
      count++;
    }
  }
  return count;
}

String Getsensorinfo(String inputString) { //提取感測器資料
  int startIndex = inputString.indexOf('A'); // 找到字母 'A' 的位置
  int endIndex = inputString.indexOf('G');   // 找到字母 'G' 的位置

  if (startIndex != -1 && endIndex != -1 && endIndex > startIndex) {
    return inputString.substring(startIndex + 1, endIndex); // 提取 'A' 到 'F' 之間的資料並返回
  } else {
    return "未找到数据"; // 如果未找到 'A' 到 'F' 之間的資料，返回錯誤消息
  }
}

String GetAreainfo(String inputString) { //提取相機編號
  int gPos = inputString.indexOf('G');
  int hPos = inputString.indexOf('H');
  String Areainfo = inputString.substring(gPos, hPos);
  combinedValue += Areainfo;
  return Areainfo;
}

String Getanimalinfo(String inputString) { //提取動物資料
  int hPos = inputString.indexOf('H');
  int iPos = inputString.indexOf('I', hPos);
  String animalinfo = inputString.substring(hPos, iPos + 2);
  combinedValue += animalinfo;
  allinfo1 = inputString.substring(iPos + 2);
  return animalinfo;
}

void parseResponse(String data) {
  int indexA = data.indexOf("A");
  int indexB = data.indexOf("B");
  int indexC = data.indexOf("C");
  int indexD = data.indexOf("D");
  int indexE = data.indexOf("E");
  int indexF = data.indexOf("F");
  int indexG = data.indexOf("G");
  int indexH = data.indexOf("H");
  int indexI = data.indexOf("I");
  int indexJ = data.indexOf("J");

  A = data.substring(indexA + 1, indexB);
  B = data.substring(indexB + 1, indexC);
  C = data.substring(indexC + 1, indexD);
  D = data.substring(indexD + 1, indexE);
  E = data.substring(indexE + 1, indexF);
  F = data.substring(indexF + 1, indexG);
  G = data.substring(indexG + 1, indexH);
  H = data.substring(indexH + 1, indexI);
  I = data.substring(indexI + 1, indexJ);
}

boolean connectWiFi() {
  Serial1.println("AT+CWMODE=1\r\n");
  delay(200);
  while (Serial1.available()) {
    String response = Serial1.readString();
    //Serial1.println(response);
  }
  Wifi_connect();
  return true;
}

void Wifi_connect() {
  sendDebug(wifi);
  Loding("Wifi_connect");
}

void Loding(String state) {
  for (int timeout = 0; timeout < 10; timeout++) {
    if (Serial1.find("OK")) {
      //Serial1.println("RECEIVED: OK");
      break;
    } else if (timeout == 9) {
      //Serial1.print(state);
      //Serial1.println(" fail...\nExit2");
    } else {
      //Serial1.print("Wifi Loading...");
      delay(500);
    }
  }
}

void sendDebug(String cmd) {
  //Serial.print("SEND: ");
  //Serial.println(cmd);
  Serial1.println(cmd);
  while (Serial1.available()) {
    String response1 = Serial1.readString();
    //Serial1.println(response1);
  }
  delay(200);
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
  pinMode(ledPin, OUTPUT);
  sendDebug("AT\r\n");
  //Loding("sent AT\r\n");
  connectWiFi();
}

void loop() {
  digitalWrite(ledPin, LOW);
  
  while(Serial.available()) {  
  
  response = Serial.readStringUntil('\n');
  digitalWrite(ledPin, HIGH);
  
  //Serial.println(response);
  String allinfo = Getallinfo(response); //提取所有的資料
  allinfo1 = allinfo;
  //Serial.println(allinfo1);
  countH = countHInString(allinfo); //抓H個數
  String sensorinfo = Getsensorinfo(allinfo); //提取感測器資料
  String Areainfo = GetAreainfo(allinfo);     //提取相機編號
  for (int i = 0; i < countH; i++) {
    sendDebug(TCP);delay(200);
    String animalinfo = Getanimalinfo(allinfo1); //提取動物資料
    String msg = "A" + sensorinfo + Areainfo + animalinfo + "J";
    parseResponse(msg);
    if (Serial1.find("Error")) {
      //Serial1.print("RECEIVED: Error\nExit1");
      return;
    }
    String cmd = GET + ("&dated=") + A +
                 ("&timed=") + B +
                 ("&area=") + C +
                 ("&animal=") + H +
                 ("&populationd=") + I +
                 ("&temperature=") + D +
                 ("&humidity=") + E +
                 ("&air_pressure=") + F +
                 ("&pic_id=") + G +
                 " HTTP/1.1\r\n" +
                 "Host: " + "8080" + "\r\n" +
                 "Connection: close\r\n\r\n";
    Serial1.print("AT+CIPSEND=");
    Serial1.println(cmd.length());
    if (Serial1.find(">")) {
      //Serial1.print(">");
      //Serial.print(cmd);
      Serial1.print(cmd);
      delay(200);
    } else {
      //Serial1.print(cmd);
      //Serial1.print("AT+CIPCLOSE\r\n");
    }
    if (Serial1.find("OK")) {
      //Serial1.println("RECEIVED: OK");
    } else {
      //Serial1.println("RECEIVED: Error\nExit2");
    }
    //Serial1.print(cmd);
    //Serial1.print("AT+CIPCLOSE\r\n");
    while (Serial1.available()) {
      String line = Serial1.readString();
      //Serial1.print(line);
    }

    //Serial1.println();
    //Serial1.println("closing connection");
    delay(200);
  }
  //Serial.print("");
  }
  //while (1);
}
