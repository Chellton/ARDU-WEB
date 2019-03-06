/*
Благодарю тех, кто оставил свои отзывы, просмотрев мой ролик на Ютубе. 
Проект постоянно развивается. Есть серверная часть, позволяющая во много раз 
сократить время загрузки страицы. Думаю, я тоже ее опубликую, но несколько позже.
Ебедительная просьба не удалять это приветствие и оставить ссылку на оригинал кода. 
Всем удачи.

I thank those who have left their comments, viewing my video on YouTube. 
The project is constantly evolving. There backend, allowing many times faster boot straitsy. 
I think I will post it too, but later.
Ebeditelnaya please do not delete this greeting and leave a link to the original code. 
Good luck to everyone.

http://www.youtube.com/watch?v=KdfUfBaEKCs


*/

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <EEPROM.h>
#include <EEPROM2.h>

#include <avr/pgmspace.h>

// MAC address from Ethernet shield sticker under board
  byte macDef[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// IP address, may need to change depending on network http://192.168.0.197/
  byte ipDef[] = { 192, 168, 0, 197 };   //Manual setup only
  byte subnetDef[] = { 255, 255, 255, 0 }; //Manual setup only
    char subnetChar[]={0};
  byte gatewayDef[] = { 192, 168, 0, 1 }; //Manual setup only
  char passwordDef[] = { 48, 48, 48, 48, 0, 0, 0, 0 };

  byte ip[4] = {0};
  byte gateway[4] = {0};
  byte subnet[4] = {0};
  byte mac[6] = {0};
  char password[] = {0};
  

// create a server at port 80
  EthernetServer server(80);  
  
// Не изменять! Здесь шаблон места хранения данных в EEPROM
const byte EEPROM_place[] = { 
  19, //0 - if 0 - set default net connection parametres
  20, //1 - IP (20,21,22,23)
  25, //2 - gateway (25,26,27,28)
  31, //3 - subnet (31,32,33,34)
  35, //4 - mac 35 (35,36,37,38,39,40)
  50, //5 - 50-57 Password 8 char
  60};//6 - 60-67 EEPROM2 CountInPin
  
File webFile;
File myFile;

#define REQ_BUF_SZ   50          // size of buffer used to capture HTTP requests
char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0;              // index into HTTP_req buffer
boolean File_type = true;
unsigned int timerWork = 0;

#define numberOut   16
#define numberIn    12

//RELE PIN ---
int RelePinOut[numberOut] = {34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49};
boolean ReleStatus[numberOut]={0};
unsigned int counter[4];
//INPUT PIN ---
int DataInPin[numberIn] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33};
boolean DataInPinStatus[numberIn]={0};
unsigned int CountInPin[4] = {0};

int SAFEmode;

// Функция программаного сброса
void(* resetFunc) (void) = 0; 

void setup()
{
  SAFEmode = EEPROM.read(30);
  for (int i = 0; i <numberOut; i++){
    pinMode(RelePinOut[i], OUTPUT);
    digitalWrite(RelePinOut[i], HIGH);
  }
  
  for (int i = 0; i <numberIn; i++){
    pinMode(DataInPin[i], INPUT);
  }

  // disable Ethernet chip
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  
  Serial.begin(57600);
  Serial2.begin(57600);

  // initialize SD card
  Serial.println("Initializing SD card...");
  Serial2.write("Initializing SD card...\n");
  if (!SD.begin(4)) {
    Serial.println("ERROR - SD card initialization failed!");
    Serial2.write("ERROR - SD card initialization faЬiled!\n");
    //return;    // init failed
  }
  Serial.println("SUCCESS - SD card initialized.");
  Serial2.write("SUCCESS - SD card initialized.\n");
  if (!SD.exists("index.htm")) {// check for index.htm file
    Serial.println("ERROR - Can't find index.htm file!");
    Serial2.write("ERROR - Can't find index.htm file!\n");
    //return;  // can't find index file
  }
  Serial.println("SUCCESS - Found index.htm file.");
  Serial2.write("SUCCESS - Found index.htm file.\n");
  Serial.println();
  Serial2.write("\n");
  
  
   //================================= N E T ==============================
  // Загрузка параметров по умолчанию если EEPROM.read(23) == 0
  if (EEPROM.read(23) == 0){
    Serial.print("Load default paramrtres... \t");
    Serial2.write("Load default paramrtres... \t");
    EEPROM.write(20, ipDef[0]);
    EEPROM.write(21, ipDef[1]);
    EEPROM.write(22, ipDef[2]); 
    EEPROM.write(23, ipDef[3]);
    
    EEPROM.write(25, gatewayDef[0]);
    EEPROM.write(26, gatewayDef[1]);
    EEPROM.write(27, gatewayDef[2]); 
    EEPROM.write(28, gatewayDef[3]);
    
    EEPROM.write(31, subnetDef[0]);
    EEPROM.write(32, subnetDef[1]);
    EEPROM.write(33, subnetDef[2]); 
    EEPROM.write(34, subnetDef[3]);

    EEPROM.write(35, macDef[0]);
    EEPROM.write(36, macDef[1]);
    EEPROM.write(37, macDef[2]);
    EEPROM.write(38, macDef[3]);
    EEPROM.write(39, macDef[4]);
    EEPROM.write(40, macDef[5]);
    
    EEPROM.write(50, passwordDef[0]);
    EEPROM.write(51, passwordDef[1]);
    EEPROM.write(52, passwordDef[2]);
    EEPROM.write(53, passwordDef[3]);
    EEPROM.write(54, passwordDef[4]);
    EEPROM.write(55, passwordDef[5]);
    EEPROM.write(56, passwordDef[6]);
    EEPROM.write(57, passwordDef[7]);
    
    Serial.println("Ok");
    Serial2.write("Ok\n");
  }
  
  Serial.println("Seting Net parameters");
  Serial2.write("Seting Net parameters\n");
  Serial.print("IP\t");
  Serial2.write("IP\t");
  for (int i = 0; i <= 3; i++){
    ip[i] = EEPROM.read(EEPROM_place[1] + i); 
    Serial.print(ip[i]);
    Serial2.write(ip[i]);
    if (i != 3){
      Serial.print(".");
      Serial2.write(".");
    }else{
      Serial.println();
      Serial2.write("\n");
    }
  }

  Serial.print("GW\t");
  for (int i = 0; i <= 3; i++){
    gateway[i] = EEPROM.read(EEPROM_place[3] + i); Serial.print(gateway[i]);
    if (i != 3){ Serial.print(".");}else{Serial.println();}
  }
  
  Serial.print("SUB\t");
  for (int i = 0; i <= 3; i++){
    subnet[i] = EEPROM.read(EEPROM_place[2] + i); Serial.print(subnet[i]);
    if (i != 3){ Serial.print(".");}else{Serial.println();}
  }

  Serial.print("MAC\t");
  for (int i = 0; i <= 5; i++){
    mac[i] = EEPROM.read(EEPROM_place[4] + i); Serial.print(mac[i]);
    if (i != 5){ Serial.print(".");}else{Serial.println();}
  }
  
  delay(500);          
  
  Ethernet.begin(mac, ip, gateway, subnet);  // initialize Ethernet device
  server.begin();           // start to listen for clients
  
  Serial.println("------------------------");
  Serial.print("My ADD\t"); Serial.println(Ethernet.localIP());
  delay(1000);//  < NET ============================================================
  
  if (SAFEmode == 1){// Если включен режим автосохранения состояния выходных линий
    Serial.println("Autosave ON");
    Serial.print("Output status\t");
    for (int i = 0; i<numberOut; i++){//Загрузка состояния реле из EEPROM
      boolean _s = EEPROM.read(i);
      if (_s < 2 ) ReleStatus[i] = _s;
      Serial.print(ReleStatus[i]);
    }
    Serial.println();
    Serial.print("Counter IN\t");
    for (int i = 0; i<4; i++){
      int _a = EEPROM_place[6] + i*2;
      EEPROM_read(_a, CountInPin[i]);
      Serial.print(ReleStatus[i]);Serial.print(", ");
    }
    Serial.println();
  }else{Serial.println("Autosave Off");}
}

void loop()//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ L O O P
{        
  timerWork = millis()/1000;
  EthernetConnect();
  ChangeRelePosition();
  counterInPin(); //
}

void ChangeRelePosition(){
  for (int i = 0; i < 16; i++){
    if (ReleStatus[i] == 0){
      digitalWrite(RelePinOut[i], HIGH); 
    }else{
      digitalWrite(RelePinOut[i], LOW);
    }
  }
}


void EthernetConnect(){
  EthernetClient client = server.available();  // try to get client

  if (client) {  // got client?
    boolean currentLineIsBlank = true;
    File_type = true;

    while (client.connected()) {
      if (client.available()) {   // client data available to read
        char c = client.read(); // read 1 byte (character) from client
        if (c == '\r') req_index = REQ_BUF_SZ;
        if (req_index < (REQ_BUF_SZ - 1)) {
          HTTP_req[req_index] = c;          // save HTTP request character
          req_index++;
        }
        if (c == '\n' && currentLineIsBlank) {
          Serial.println(HTTP_req);//debug
          if (StrContains(HTTP_req, "GET / ") != 0 
            ||StrContains(HTTP_req, "GET /index.htm") != 0){

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connnection: close");
            client.println();
            webFile = SD.open("index.htm"); // open web page file
          }
          else if (StrContains(HTTP_req, "GET /") != 0) {
            char *GetData;
            GetData = HTTP_req + 5; //Отсекаем префикс "GET /"
            (strstr(HTTP_req, " HTTP/1.1"))[0] = 0; //Отсекаем суфикс " HTTP/1.1"

            if (StrContains(HTTP_req, "server.cgi?data") != 0) {
              char *dataCgi = HTTP_req + 21;
              File_type = false;
              ReWork(dataCgi);
            }
            else if (StrContains(HTTP_req, "status.xml") != 0) {
              File_type = false;
              XML_response(client);
            }
            else{
              webFile = SD.open(GetData); // look after the "GET /" (5 chars)
            }
          }

          if (webFile) {
            while(webFile.available() && File_type == true) {
              client.write(webFile.read()); // send web page to client
            }
            webFile.close();
          }
          // reset buffer index and all buffer elements to 0
          req_index = 0;
          StrClear(HTTP_req, REQ_BUF_SZ);
          break;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // a text character was received from client
          currentLineIsBlank = false;
        }
      } // end if (client.available())
    } // end while (client.connected())
    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
    delay(2);
    client.flush();
    delay(2);
  } // end if (client)

}

// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
  for (int i = 0; i < length; i++) {
    str[i] = 0;
  }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
  char found = 0;
  char index = 0;
  char len;

  len = strlen(str);

  if (strlen(sfind) > len) {
    return 0;
  }
  while (index < len) {
    if (str[index] == sfind[found]) {
      found++;
      if (strlen(sfind) == found) {
        return 1;
      }
    }
    else {
      found = 0;
    }
    index++;
  }

  return 0;
}


void XML_response(EthernetClient cl){

  cl.println("HTTP/1.1 200 OK");
  cl.println();

  cl.println("<response>");
    cl.print("<rt0>"); //rele_table0
        for (int i = 0; i < 16; i++) {
          if (ReleStatus[i] == 1){
            cl.print("1");
          }else{
            cl.print("0");
          }
          //Serial.print(ReleStatus[i]); //debag
        }
     cl.println("</rt0>");
  //Serial.print("\t"); //debug
  
     cl.print("<int0>"); //in_table0
        for (int i = 0; i < numberIn; i++) {
          boolean c = digitalRead(DataInPin[i]);
          if (c == 1){
            cl.print("1");
          }else{
            cl.print("0");
          }
        //Serial.print(c); //debug
        }
  cl.println("</int0>");
  //Serial.println(); //debug
  
  cl.print("<out0>"); //out_table0
  cl.print("1001");
  cl.println("</out0>");
  
  cl.print("<sn0>");
  cl.print("12345567890");
  cl.println("</sn0>");
    
  cl.print("<fw0>");
  cl.print("1");
  cl.println("</fw0>");
  
  cl.print("<adc0>");
  cl.print("12.1");
  cl.println("</adc0>");
  
  cl.print("<adc1>");
  cl.print("14.4");
  cl.println("</adc1>");
  
  cl.print("<tmp0>"); //temper0
  cl.print("25.5");
  cl.println("</tmp0>");
  
//  cl.print("<count0>");
//  cl.print(1);
//  cl.println("</count0>");
//  
//  cl.print("<count1>");
//  cl.print(2);
//  cl.println("</count1>");
//
//  cl.print("<count2>");
//  cl.print(3);
//  cl.println("</count3>");
//  
//  cl.print("<count3>");
//  cl.print(4);
//  cl.println("</count3>");
////  
  cl.print("<c_ccl0>"); //count_cycle0
  cl.print(CountInPin[0]);
  cl.println("</c_ccl0>");
  
  cl.print("<c_ccl1>");
  cl.print(CountInPin[1]);
  cl.println("</c_ccl1>");
  
  cl.print("<c_ccl2>");
  cl.print(CountInPin[2]);
  cl.println("</c_ccl2>");
  
  cl.print("<c_ccl3>");
  cl.print(CountInPin[3]);
  cl.println("</c_ccl3>");

  cl.print("<syst0>");  //systime0
  cl.print(timerWork); 
  cl.println("</syst0>");

  cl.print("<s_ip0>");
  cl.print(Ethernet.localIP());
  cl.println("</s_ip0>");
  
  cl.print("<s_mac0>");//set_mac0
  for (int i = 0; i <=5; i++){
    cl.print(mac[i]);
    if (i!=5) cl.print(".");
  }
  cl.println("</s_mac0>");
  
  cl.print("<s_msk0>"); //set_mask0
  cl.print(StringADD(255,255,255,0));
  cl.println("</s_msk0>");
  
  cl.print("<set_gate0>");//set_gate0
  cl.print(StringADD(192,168,1,10));
  cl.println("</set_gate0>");

  cl.print("<bst0>");//bitset_table0
  cl.print(0);
  cl.println("</bst0>");
  cl.print("</response>");
}

void ReWork(char *datacgi){
  (strstr(HTTP_req, " HTTP/1.1"))[0] = 0; //Отсекаем суфикс " HTTP/1.1"

  //Выполнение запроса на управление дискретными выходами (управление реле)
  if (StrContains(datacgi, "REL") != 0){
    datacgi = datacgi + 4; // отсекаем "REL="
    int num = datacgi[0]-48; // извлекаем первую цифру номера...
    // ... и если есть вторая - формируем номер реле
    if (datacgi[1] != NULL) num = num*10 + (datacgi[1] - 48);
    num = num - 1;
    if (ReleStatus[num] == 1){
      ReleStatus[num] = 0;
    }
    else{
      ReleStatus[num] = 1;
    }
    //Если включен режим автосохранения параметров GET /server.cgi?data=SAV,1 HTTP/1.1
    if (SAFEmode == true){
      EEPROM.write(num, ReleStatus[num]);
      // Serial.print("Save \t"); Serial.print(num); //debug
      // Serial.print("\t"); Serial.println(ReleStatus[num]); //debug
    }
  }


  //выполнение запроса на измененеие адреса, например...
  //... GET /server.cgi?data=IPX,192.168.1.197$
  //... GET /server.cgi?data=MSK,255.255.255.0$
  //... GET /server.cgi?data=GTW,192.168.1.10$
  //... GET /server.cgi?data=PAS,12345678 HTTP/1
  //... GET /server.cgi?data=MAC,222.173.190.239.254.237$


  if (StrContains(datacgi, "IPX,") != 0){
    SetNetParametres(datacgi, 0);
  }else if (StrContains(datacgi, "MSK,") != 0){
    SetNetParametres(datacgi, 1);
  }else if (StrContains(datacgi, "GTW,") != 0){
    SetNetParametres(datacgi, 2);
  }else if (StrContains(datacgi, "MAC,") != 0){
    SetNetParametres(datacgi, 3);
  }else if (StrContains(datacgi, "PAS,") != 0){
    SetPassword(datacgi);
  }


  //Выполнение запроса на перезагрузку модуля GET /server.cgi?data=RST
  if (StrContains(datacgi, "RST") != 0){
    resetFunc();
  }

  //Выполнение запроса на сброс к заводским настройкам GET /server.cgi?data=DEF
  if (StrContains(datacgi, "DEF") != 0){
    // Serial.println("Reset and Default set"); //debug
    EEPROM.write(23,0); // пишем в 23 ячейку EEPROM ноль, вызывая ошибку - IP xxx.xxx.xxx.0
    delay(500);
    resetFunc();
  }

  //Выполнение запроса на автосохраннение параметров GET /server.cgi?data=SAV,1 HTTP/1.1
  if (StrContains(datacgi, "SAV,1") != 0){
    // Serial.println("GET Autosave ON ....."); //debug
    datacgi = datacgi + 4; // отсекаем "SAV,"
    // Serial.println(datacgi); //debug
    if (datacgi[0] == 49){ // 49 DEC == '1' ASCII
      SAFEmode = 1;
      // Serial.println("Autosave ON "); //debug
    }else{
      SAFEmode = 0;
      // Serial.println("Autosave OFF "); //debug
    }
    EEPROM.write(30,SAFEmode);
  }
}

void SetNetParametres(char* dataset, int datatype)
{
    int b = 0; // переменная для преобразования CHAR в INT
    int ste = 0; // переменная для контроля шагов между раделителями "." в новом адресе
    byte add[5]; // массив нового адреса... 192 168 1 197
    int _cchar = 20; // максимальное количество символов в строке (16 за вычетом заголовка)
    int _cblock = 3; // количество блоков в строке, разделенных "." с учетом нуля (0,1,2,3)
    
    byte _place; // переменная для определения ячейки памяти EEPROM
    char* _sprint; // переменная для хранения текста сообщения 
    
    if (datatype == 0){
      _place = EEPROM_place[1];
      _sprint = "New IP: ";  
    }else if (datatype == 1){
      _place = EEPROM_place[2];
      _sprint = "New subnet: ";
    }else if (datatype == 2){
      _place = EEPROM_place[3];
      _sprint = "New gateway: ";
    }else if (datatype == 3){
      _place = EEPROM_place[4];
      _sprint = "New MAC: ";
      _cchar = 32;
      _cblock = 5;
    }
    
    
    for (int i = 4; i <= _cchar; i++){ // Начинаем конвертацию CHAR в INT
      if (dataset[i] != '$'){ //если это не конец данных...
        if (dataset[i] != '.'){ // ... и не разделитель ".", то 
          b = b + dataset[i] - 48; // преобразуем CHAR в INT
          b = b*10; // сдвигаем регистр на шаг для приема следующего INT
        }
        else{
          ste ++; // если разделитель пришел,
          b = b/10; // делаем откат последнего сдвига регистра
          if (ste == 1){add[0] = b;} //... и присваеиваем IP первый аргумент, например 192
          if (ste == 2){add[1] = b;} // и так каждый раз при появлении разделителя...
          if (ste == 3){add[2] = b;      } // ...
          if (ste == 4){add[3] = b;      } // ...
          if (ste == 5){add[4] = b;      } // ...
          b = 0;
        }
      }else{ // если пришел признак окончания пакета "$"
        ste ++; // делаем все тоже самое, что при в предыдушие разы..., но....
        b = b/10;
        if (ste == _cblock + 1){
          add[_cblock] = b;
        }
        i = _cchar + 1; // ... задаем счетчику циклов запредельный аргумент, что бы его остановить.
      }
    }

//    Serial.print(_sprint); //debug
//    for (int i = 0; i <= _cblock; i++){
//        Serial.print(add[i]);
//        if (i!=_cblock) Serial.print(".");
//    }
  
    //Сохраняем новый адрес в EEPROM
    for (int i = 0; i <= _cblock; i++){
      EEPROM.write(_place + i, add[i]);
    }
//  Serial.println("\t saved Ok");//debug
}

void SetPassword(char *dataset){
//  Serial.print("New password:\t");//debug
  char *_psv = dataset + 4;
  int _place = EEPROM_place[5];
  boolean stopWrite = false;
  
  for (int i = 0; i <= 7; i++){
    if (_psv[i] == 0) stopWrite = true;
    
    if (stopWrite != true){
      EEPROM.write(EEPROM_place[_place], _psv[i]);
//      Serial.print(_psv[i]);//debug
    }else{
//      EEPROM.write(_place, 0);//debug
    }
  }
//  Serial.println();//debug
}

String StringADD(int add0, int add1, int add2, int add3){
  
  char buff0[6], buff1[6], buff2[6], buff3[6];
  itoa(add0, buff0, 10);
  itoa(add1, buff1, 10);
  itoa(add2, buff2, 10);
  itoa(add3, buff3, 10);

  String _add = String(String(buff0) + "." + buff1 + "." + buff2 + "." + buff3);
  //Serial.println(vbn);
  return _add;
}

void counterInPin(){ // Процедура контроля состояния InPin и работа счетчиков (9, 10, 11, 12)
  for (int i = 0; i < 4; i++){
    boolean in = digitalRead(DataInPin[i+8]);
    if (DataInPinStatus[i+8] != in){
      DataInPinStatus[i+8] = in;
      if (in == true){
        CountInPin[i]++;
        int _a = EEPROM_place[6] + i*2;
        EEPROM_write(_a,CountInPin[i]);
      }
    }
  }
}
