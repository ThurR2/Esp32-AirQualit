// Iniciando importação das bibliotecas - start import a libs
#include <ArduinoJson.h> 
//Wifi
#include <WiFi.h>
const char* ssid = "REDE";           /* Nome */
const char* password = "SENHA";      /* Senha */
WiFiClient wifiClient;

//Lib de MQTT
#include <PubSubClient.h>
#include "Esp32MQTTClient.h"

//DHT 11
#include "DHT.h"
// Css811 - Chines PCB Lib SparkFunCs811
#include <Wire.h>
#include "SparkFunCCS811.h"
float h;
float t;

//Configurando - iniciando bibliotes -- Config - start libs
//DHT 11
#define DHTPIN 5
#define DHTTYPE    DHT11
DHT dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

// Css811 - Chines PCB lib SparkFunCs811
#define CCS811_ADDR 0x5A // Endereço alternativo.
CCS811 myCCS811(CCS811_ADDR);
float co2;
float tvoc;

/* -------- Conexão IBM - ESP32 ----------- */
#define ORG "ORG" /* ID de organização */
#define DEVICE_TYPE "Componente" /* Insira o nome do componente */
#define DEVICE_ID "ID do componente" /* Insira o ID */
#define TOKEN "Token de autenticação"/* Insira o Token */
/*-------- Comunicação IOT – Não altere essa parte da programação -------- */
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
const char eventTopic[] = "iot-2/evt/status/fmt/json";
/*-------- Variável Lógica -------- */
int cont = 0;                         /* Realiza uma contagem contínua */

PubSubClient client(server, 1883, wifiClient);

void setup() {
  Serial.begin(115200);
  Serial.println("Inciando Sistema de Qualidade de ar");
  dht.begin(); // Inicia interface com o sensor DHT11 - Temp/umid.

  Wire.begin(); // Inciar a biblioteca I2C - CSS811.
  CCS811Core::CCS811_Status_e returnCode = myCCS811.beginWithStatus();
  Serial.print("CCS811 begin exited with: ");
  Serial.println(myCCS811.statusString(returnCode));
  wifiConnect();                     /* Cria o void wifiConnect */
  mqttConnect();                     /* Cria o void mqttConnect */

}

void loop() {


}
//printSensorError gets, clears, then prints the errors
//saved within the error register.
void printSensorError()
{
  uint8_t error = myCCS811.getErrorRegister();

  if (error == 0xFF) //comm error
  {
    Serial.println("Failed to get ERROR_ID register.");
  }
  else
  {
    Serial.print("Error: ");
    if (error & 1 << 5)
      Serial.print("HeaterSupply");
    if (error & 1 << 4)
      Serial.print("HeaterFault");
    if (error & 1 << 3)
      Serial.print("MaxResistance");
    if (error & 1 << 2)
      Serial.print("MeasModeInvalid");
    if (error & 1 << 1)
      Serial.print("ReadRegInvalid");
    if (error & 1 << 0)
      Serial.print("MsgInvalid");
    Serial.println();
  }
}
void wifiConnect()
{ /* Função void wifiConnect */
  Serial.print("Conectando a Rede ");
  Serial.print(ssid);                /* Indica a Rede que o ESP32 irá se conectar */
  WiFi.begin(ssid, password);        /* Conecta ao ssid e o password configurado */
  while (WiFi.status() != WL_CONNECTED)
  { /* Enquanto estiver desconectado */
    delay(500);                      /* Aguarda 500 milissegundos */
    Serial.print(".");
  }
  Serial.print("Wi-Fi conectado, Endereço de IP: ");
  Serial.println(WiFi.localIP());     /* Indica o endereço de IP */
}

void mqttConnect()
{ /* Função void mqttConnect */
  if (!!!client.connected())
  {
    Serial.print("Reconectando MQTT do cliente ");
    Serial.println(server);                       /* Indica o endereço do servidor */
    while (!!!client.connect(clientId, authMethod, token) )
    {
      Serial.print(".");
      delay(500); 
    }
  }
}

void sensores()
{
  // Codigo DHT11
  delay(2000); // Sensor demora 2 segundo para leitura.
  h = dht.readHumidity(); // Le a umidade do ar.
  t = dht.readTemperature(); // Le a temperatura do ar.
  Serial.print(h);
  Serial.print(t);
  
  // Check if any reads failed and exit early (to try again).
  
  if (isnan(h) || isnan(t)) { // Faz o teste se existe algum dado nulo, se existir da saida.
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hic = dht.computeHeatIndex(t, h, false); // Calcula o index de calor.
  Serial.print("linha 134 ");
  //Codigo Css811 - Pcb Chines
 // Calibra medição de Co2 atraves do algoritmo do sensor + temp + umidade.
  //myCCS811.readAlgorithmResults();
  Serial.print("linha 138 ");
  //
  if (myCCS811.dataAvailable())
  {
    Serial.print("Entrei no If 62");
    myCCS811.setEnvironmentalData(h, t);
    //Calling readAlgorithmResults() function updates the global tVOC and CO2 variables
    myCCS811.readAlgorithmResults();

    Serial.print("CO2[");
    Serial.print(myCCS811.getCO2());
    Serial.print("] tVOC[");
    co2 = myCCS811.getCO2();
    Serial.print(myCCS811.getTVOC());
    Serial.print("] millis[");
    tvoc = myCCS811.getTVOC();
    Serial.print(millis());
    Serial.print("]");
    Serial.println();
  }
  else if (myCCS811.checkForStatusError())
  {
    //If the CCS811 found an internal error, print it.
    printSensorError();
        Serial.print("FATAL ERROR - CODE 01");
  }
  else {
            Serial.print("\n Sensor Nao carregado.");
  }
  delay(1000); //Wait for next reading
  //
}
String createJsonString() {
  String data = "{";
    data+= "\"d\": {";
      data+="\"temperature\":";
      data+=String(t);
      data+=",";
      data+="\"humidity\":";
      data+=String(h);
      data+=",";
      data+="\"Co2\":";
      data+=String(co2);
    data+="}";
  data+="}";
  return data;
}
