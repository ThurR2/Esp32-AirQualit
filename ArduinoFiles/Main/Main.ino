// Iniciando importação das bibliotecas - start import a libs
//Wifi
#include <WiFi.h>

//Lib de MQTT
#include <PubSubClient.h>

//DHT 11
#include "DHT.h"
// Css811 - Chines PCB Lib SparkFunCs811
#include <Wire.h>
#include "SparkFunCCS811.h"

//Configurando - iniciando bibliotes -- Config - start libs
//DHT 11
#define DHTPIN 5
#define DHTTYPE    DHT11
DHT dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

// Css811 - Chines PCB lib SparkFunCs811
#define CCS811_ADDR 0x5A // Endereço alternativo.
CCS811 myCCS811(CCS811_ADDR);



void setup() {
  Serial.begin(115200);
  Serial.println("Inciando Sistema de Qualidade de ar");
  dht.begin(); // Inicia interface com o sensor DHT11 - Temp/umid.

  Wire.begin(); // Inciar a biblioteca I2C - CSS811.
  CCS811Core::CCS811_Status_e returnCode = myCCS811.beginWithStatus();
  Serial.print("CCS811 begin exited with: ");
  Serial.println(myCCS811.statusString(returnCode));

}

void loop() {
  // put your main code here, to run repeatedly:

  // Codigo DHT11
  delay(2000); // Sensor demora 2 segundo para leitura.
  float h = dht.readHumidity(); // Le a umidade do ar.
  float t = dht.readTemperature(); // Le a temperatura do ar.
  Serial.print(h);
  Serial.print(t);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) { // Faz o teste se existe algum dado nulo, se existir da saida.
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hic = dht.computeHeatIndex(t, h, false); // Calcula o index de calor.
  Serial.print("linha 54 ");
  //Codigo Css811 - Pcb Chines
 // Calibraa medição de Co2 atraves do algoritmo do sensor + temp + umidade.
  //myCCS811.readAlgorithmResults();
  Serial.print("linha 58 ");
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
    Serial.print(myCCS811.getTVOC());
    Serial.print("] millis[");
    Serial.print(millis());
    Serial.print("]");
    Serial.println();
  }
  else if (myCCS811.checkForStatusError())
  {
    //If the CCS811 found an internal error, print it.
    printSensorError();
        Serial.print("Deu RUim 1");
  }
  else {
            Serial.print("\n Sensor Nao carregado.");
  }
  delay(1000); //Wait for next reading
  //



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
