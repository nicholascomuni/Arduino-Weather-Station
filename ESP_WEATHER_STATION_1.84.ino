#include <ESP8266WiFi.h>

const char* ssid = "TP-LINK_43B6EA" ;
const char* password = "71514932"   ;

WiFiServer server(80);

String Conteudo ;
String DataHora ;
int Luz         ;
int DoW         ;
int Temp        ;
int Umid        ;
int HI          ;
int DP          ;
int INT         ;

bool Control;

unsigned long Tempo;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
  digitalWrite(2, HIGH);
}



// prepare a web page to be send to a client (web browser)
String prepareHtmlPage() {
  String htmlPage =
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n"                               +
            "<!DOCTYPE html>"                    +
            "<html>"                             +
            "<head>"                             +
            "<meta charset='UTF-8'>"
            "</head>"                            +
            "<h1>Weather Station"                +
            "</h1><h2>Data e Hora"               +
            "</h2><p> "+ DataHora                + 
            "</p><p>"+DiaSemana(DoW)             +
            "</p><h2>Condições Atuais"           +
            " </h2><p>Temperatura : " + Temp     +
            " C</p1><p>Umidade : " + Umid        +
            "%</p><p>Indice de Calor : "  + HI   +
            " C</p><p>Ponto de Orvalho : " + DP  +
            " C</p><p>Fotoperíodo : " + Luz      +
            "</p><p>Pressao Atmosferica : "      +
            "</p><p>Velocidade do Vento : "      +
            "</p><p>Detecçao de Relampagos : "   +
            "</p><p>Campo EM : "                 + 
            "</p></html>"                        +
            "\r\n";
  return htmlPage ;
}


void loop() {
LeConteudo();
if((Conteudo != "") && ((millis() - Tempo)> 2)){      //INICIA
  Serial.println(Conteudo);
  Tradutor(Conteudo);  
  
  Conteudo = "";
  Tempo = millis();
}                                                     //FINALIZA
  
  WiFiClient client = server.available();
  // wait for a client (web browser) to connect
  if (client)
  {
    Serial.println("\n[Client connected]");
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        digitalWrite(0, HIGH);
        String line = client.readStringUntil('\r');
        Serial.print(line);
        // wait for end of client's request, that is marked with an empty line
        if (line.length() == 1 && line[0] == '\n')
        {
          client.println(prepareHtmlPage());
          break;
        }
      }
    }
    delay(1); // give the web browser time to receive the data

    // close the connection:
    client.stop();
    digitalWrite(0, LOW);
    Serial.println("[Client disonnected]");
  }
}

String LeConteudo() {  //LE SERIAL DO PC FUNÇÃO

char caracter;
while(Serial.available()){
  caracter = Serial.read()  ;
  Conteudo.concat(caracter) ;
  Tempo = millis()          ;  }
  return Conteudo           ;
}

void Tradutor(String Data){
  DataHora = Data.substring(0,19)               ;
  DoW  = (  (Data.substring(20,21) ).toInt()  ) ;
  Temp = (  (Data.substring(21,23) ).toInt()  ) ;
  Umid = (  (Data.substring(23,25) ).toInt()  ) ;
  HI   = (  (Data.substring(25,27) ).toInt()  ) ;
  DP   = (  (Data.substring(27,29) ).toInt()  ) ;
  Luz  = (  (Data.substring(29,31) ).toInt()  ) ;
}

String DiaSemana(int D){
  switch(D){
    case 1 :
    return "Domingo"       ;
    break                  ;
        case 2:
    return "Segunda-feira" ;
    break                  ;
        case 3:
    return "Terca-feira"   ;
    break                  ;
        case 4:
    return "Quarta-feira"  ;
    break                  ;
        case 5:
    return "Quinta-feira"  ;
    break                  ;
        case 6:
    return "Sexta-feira"   ;
    break                  ;
        case 7:
    return "Sábado"        ;
    break                  ;
  }
}
