#include <Ethernet.h>
#include <SPI.h>
//network NB: Pins 10, 11, 12 and 13 are reserved for Ethernet module. 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 0, 177 };
byte gateway[] = { 192, 168, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };

String inString = String(35);

String Led;

int led[] = {00, 2, 3, 4 ,5 ,6 ,7 ,8,9  }; //Led pins num 0 in arry is not used
int numofleds = 8; //numofleds
String value[] = {"on","on","on","on","on","on","on","on","on"}; //startup all led are off

EthernetServer server(80);
String data;
void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip,gateway,subnet); 
  server.begin();
 //set pin mode
  for (int j = 1; j < (numofleds + 1); j++){
    pinMode(led[j], OUTPUT);
  }
  Serial.println("Serial READY");
  Serial.println("Ethernet READY");
  Serial.println("Server READY");
}

void loop()
{
  EthernetClient client = server.available();
  
  if(client){
    // an http request ends with a blank line
    boolean current_line_is_blank = true;
    while (client.connected()) {
     
      if(client.available()) {
      
        char c = client.read();
        // if we've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so we can send a reply
        if (inString.length() < 35) {
            inString.concat(c);
         } 
        if (c == '\n' && current_line_is_blank) {
                    
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<html><body><form method=get>");
          client.println("<p>Led controller</p>");
         
         for(int i=1;i < (numofleds + 1) ;i++){ 
           Led = String("led") + i;
           
          if(inString.indexOf(Led+"=on")>0 || inString.indexOf("all=on")>0){
            Serial.println(Led+"on");
            digitalWrite(led[i], HIGH);
            value[i] = "off"; 
          }else if(inString.indexOf(Led+"=off")>0 || inString.indexOf("all=off")>0 ){          
            Serial.println(Led+"on");
            digitalWrite(led[i], LOW);
            value[i] = "on";
          }
           client.println("<br>"+Led+"  <input type=submit name="+Led+" value="+value[i]+">");
         }
         client.println("<br>All <input type=submit name=all value=on><input type=submit name=all value=off>");
         client.println("</from></html></body>");
          break;
        }
        if (c == '\n') {
          // we're starting a new line
          current_line_is_blank = true;
        } else if (c != '\r') {
          // we've gotten a character on the current line
          current_line_is_blank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    inString = "";
    client.stop();
  }
}
