#include <DHT.h>
#include <DS3231.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

#define DHTPIN 8
#define DHTTYPE DHT22

File file;
DHT dht(DHTPIN,DHTTYPE);
DS3231 Clock;
SoftwareSerial serial(2, 3);

bool Century = false;
bool h12;
bool PM;
bool Control = 0 ;
bool ADy, A12h, Apm;
bool Led;

byte ADay, AHour, AMinute, ASecond, ABits;
const int CS_PIN = 10;
int Temp;
int Umid;
int HI;
int Data[19] ;    
int Luz;
int LuzMap;
    
char Buffer[50];
char Buffer2[50];

String Conteudo;
String ConteudoText;
String DataString;

unsigned long Tempo;
unsigned long Tempo2;
unsigned long Tempo3;

void setup() {
Wire.begin();
Serial.begin(115200);
serial.begin(115200);
dht.begin();
initializeSD();
createFile("data.txt");

pinMode(A1, INPUT);
pinMode(4, OUTPUT);       //LIGADO
pinMode(5, OUTPUT);       //
pinMode(6, OUTPUT);       //ARDUINO RX
pinMode(7, OUTPUT);       //ESP RX
pinMode(9, OUTPUT);

digitalWrite(4, HIGH);

}

void loop() {
  
ApagaLed();
SaveData();
Sensor();
LeConteudo();
Relogio();


if(Control == 1){
  Envia();
  Control = 0;
}
}

void Sensor(){
  if(((millis() - Tempo) > 500) && (Control == 0)){
  Data[7] = dht.readTemperature() ;
  Data[8] = dht.readHumidity()  ;
  Data[9] = dht.computeHeatIndex(Data[7] , Data[8] , false) ;  
  Data[10] = dewPoint(Data[7]  , Data[8] ) ;
  LuzMap = analogRead(A1);
  Data[11] = ( map(LuzMap,0,1024,0,100) );
  Tempo = millis();
  Control = 1;
  }
}


String LeConteudo() {  //LE SERIAL DO ESP
char caracter;
while(serial.available()){
  caracter = serial.read();
  Serial.print(caracter);
}}

void Envia(){

sprintf(Buffer2,"%02d/%02d/20%d %02d:%02d:%02d %d%2d%2d%2d%2d%2d",Data[2],Data[1],Data[0],Data[4],Data[5],Data[6],Data[3],Data[7],Data[8],Data[9],Data[10],Data[11] );
Serial.print("Buffer : ");
Serial.println(Buffer2);
Serial.println();
serial.println(Buffer2);
digitalWrite(5, HIGH);
Tempo3 = millis();
}




double dewPoint(double celsius, double humidity){
  // (1) Saturation Vapor Pressure = ESGG(T)
  double RATIO = 373.15 / (273.15 + celsius);
  double RHS = -7.90298 * (RATIO - 1);
  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / RATIO ))) - 1) ;
  RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
  RHS += log10(1013.246);
  // factor -3 is to adjust units - Vapor Pressure SVP * humidity
  double VP = pow(10, RHS - 3) * humidity;
  // (2) DEWPOINT = F(Vapor Pressure)
  double T = log(VP / 0.61078); // temp var
  return (241.88 * T) / (17.558 - T);
}

void Relogio(){


  Data[0] = Clock.getYear() ;                // ANO
  Data[1] = Clock.getMonth(Century) ;    // MêS
  Data[2] = Clock.getDate() ;            // DIA
  Data[3] = Clock.getDoW();                  // DIA SEMANA
  Data[4] = Clock.getHour(h12, PM) ;     // HORA
  Data[5] = Clock.getMinute() ;          // MINUTO
  Data[6] = Clock.getSecond() ;          // SEGUNDO
 
}




void initializeSD(){
  Serial.println("Initializing SD card...");
  pinMode(CS_PIN, OUTPUT);
  if (SD.begin())  {
    Serial.println("SD card is ready to use."); } else {
    Serial.println("SD card initialization failed");
    return;  }}



int createFile(char filename[]){
  file = SD.open(filename, FILE_WRITE);
  if (file)  {
    Serial.println("File created successfully.");
    digitalWrite(7, HIGH);
    digitalWrite(9, LOW);
    return 1; } else{
    Serial.println("Error while creating file.");
    digitalWrite(9, HIGH);
    digitalWrite(7, LOW);
    return 0; }}



int openFile(char filename[]){
  file = SD.open(filename);
  if (file)  {
    Serial.println("File opened with success!");
    return 1; } else  {
    Serial.println("Error opening file...");
    return 0; }}



void readFile() {  //LE ARQUIVO
char caracter;
while(file.available()){
  caracter = file.read();
Serial.print(caracter);}}

void SaveData(){ 
    if( (millis() - Tempo2) > 600000){
Serial.println();
createFile("data.txt");
sprintf(Buffer,"%2d/%2d/20%2d %02d,%02d%02d ,%2d,%2d,%2d,%2d,%2d",Data[2] ,Data[1] ,Data[0],Data[4] ,Data[5] ,Data[6] ,Data[7] ,Data[8] ,Data[9] ,Data[10] ,Data[11] );
Serial.print("SD CARD : ");
Serial.println(Buffer);
Serial.println();
file.println(Buffer);
file.close(); 
Tempo2 = millis();
   }
    }

void ApagaLed(){
  if((millis() - Tempo3) > 8){
    digitalWrite(5, LOW);
    digitalWrite(7, LOW);
    Tempo3 = millis();
  }
}


