//Programa: Relogio com NodeMCU ESP8266 e display Oled - NTP
//Referencia: A Beginner's Guide to the ESP8266 - Pieter P.
//Adaptacoes e alteracoes: Arduino e Cia

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <SSD1306Wire.h>
#include <time.h>

//Cria uma instancia da classe ESP8266WiFiMulti, chamada 'wifiMulti'
ESP8266WiFiMulti wifiMulti;

//Cria uma instancia da classe WifiUDP para enviar e receber dados
WiFiUDP UDP;

IPAddress timeServerIP;
//Define o servidor NTP utilizado
const char* NTPServerName = "b.ntp.br";

//Time stamp do NTP se encontra nos primeiros 48 bytes da mensagem
const int NTP_PACKET_SIZE = 48;

//Buffer para armazenar os pacotes transmitidos e recebidos
byte NTPBuffer[NTP_PACKET_SIZE];

//Inicializa o display Oled
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);

int screenWidth = 64;
int screenHeight = 48;
int centerX = screenWidth / 2;
int centerY = screenHeight / 2;

String months[12] = {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez"};

void startWiFi();
void startUDP();
uint32_t getTime();
void sendNTPpacket(IPAddress& address);
inline int getSeconds(uint32_t UNIXTime);
inline int getMinutes(uint32_t UNIXTime);
inline int getHours(uint32_t UNIXTime);

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.println("rn");

  //Inicia a comunicacao com os hothospts configurados
  startWiFi();
  startUDP();

  if (!WiFi.hostByName(NTPServerName, timeServerIP)) {
    //Obtem o endereco IP do servidor NTP
    Serial.println("DNS lookup failed. Rebooting.");
    Serial.flush();
    ESP.reset();
  }
  Serial.print("IP do servidor NTP:t");
  Serial.println(timeServerIP);

  Serial.println("rnEnviando requisicao NTP...");
  sendNTPpacket(timeServerIP);

  //Inicializacao do display
  display.init();
}

//Requisita horario do servidor NTP a cada minuto
unsigned long intervalNTP = 60000;
unsigned long prevNTP = 0;
unsigned long lastNTPResponse = millis();
uint32_t timeUNIX = 0;

unsigned long prevActualTime = 0;

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - prevNTP > intervalNTP) {
    //Verificar se passou um minuto da ultima requisicao
    prevNTP = currentMillis;
    Serial.println("rnEnviando requisicao NTP ...");
    sendNTPpacket(timeServerIP);
  }

  uint32_t time = getTime();
  if (time) {
    timeUNIX = time - 10800;
    Serial.print("Resposta NTP:t");
    Serial.println(timeUNIX);
    lastNTPResponse = currentMillis;
  } else if ((currentMillis - lastNTPResponse) > 3600000) {
    Serial.println("Mais de 1 hora desde a ultima resposta NTP. Reiniciando.");
    Serial.flush();
    ESP.reset();
  }

  uint32_t actualTime = timeUNIX + (currentMillis - lastNTPResponse) / 1000;
  if (actualTime != prevActualTime && timeUNIX != 0) {
    //Verifica se passou um segundo desde a ultima impressao de valores no serial monitor
    prevActualTime = actualTime;
    Serial.printf("\rUTC time:\t%02d:%02d:%02d", getHours(actualTime), getMinutes(actualTime), getSeconds(actualTime));
  }

  struct tm *currentTime;
  time_t seconds = time_t(timeUNIX);
  currentTime = localtime(&seconds);

  //Mostrando a hora no display
  //Apaga o display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);

  char string[10];
  sprintf(string, "%02d:%02d", getHours(actualTime), getMinutes(actualTime));

  //Mostra o horario atualizado
  display.setFont(ArialMT_Plain_24);
  display.drawString(centerX, 0, string);

  sprintf(string, "%02d %s", currentTime->tm_mday, months[currentTime->tm_mon].c_str());
  display.setFont(ArialMT_Plain_10);
  display.drawString(centerX, centerY, string);

  display.display();
}

void startWiFi() {
  //Coloque aqui as redes wifi necessarias
  wifiMulti.addAP("Murilo", "94658243");

  Serial.println("Conectando");
  while (wifiMulti.run() != WL_CONNECTED) {
    //Aguarda a conexao da rede wifi
    delay(250);
    Serial.print('.');
  }
  Serial.println("rn");
  Serial.print("Conectado a rede ");
  Serial.println(WiFi.SSID());
  Serial.print("Endereco IP:t");
  Serial.print(WiFi.localIP());
  Serial.println("rn");
}

void startUDP() {
  Serial.println("Iniciando UDP");
  //Inicializa UDP na porta 23
  UDP.begin(123);
  Serial.print("Porta local:t");
  Serial.println(UDP.localPort());
  Serial.println();
}

uint32_t getTime() {
  if (UDP.parsePacket() == 0) {
    return 0;
  }
  UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  //Combina os 4 bytes do timestamp em um numero de 32 bits
  uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
  //Converte o horario NTP para UNIX timestamp
  //Unix time comeca em 1 de Jan de 1970. Sao 2208988800 segundos no horario NTP:
  const uint32_t seventyYears = 2208988800UL;
  //Subtrai setenta anos do tempo
  uint32_t UNIXTime = NTPTime - seventyYears;
  return UNIXTime;
}

void sendNTPpacket(IPAddress& address) {
  //Seta todos os bytes do buffer como 0
  memset(NTPBuffer, 0, NTP_PACKET_SIZE);
  //Inicializa os valores necessarios para formar a requisicao NTP
  NTPBuffer[0] = 0b11100011;   // LI, Version, Mode
  //Envia um pacote requisitando o timestamp
  UDP.beginPacket(address, 123);
  UDP.write(NTPBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}

inline int getSeconds(uint32_t UNIXTime) {
  return UNIXTime % 60;
}

inline int getMinutes(uint32_t UNIXTime) {
  return UNIXTime / 60 % 60;
}

inline int getHours(uint32_t UNIXTime) {
  return UNIXTime / 3600 % 24;
}