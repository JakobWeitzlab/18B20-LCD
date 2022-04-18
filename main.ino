#include <LiquidCrystal_I2C.h>
#include <OneWire.h>; 
#include <DallasTemperature.h>; 


// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 4;

unsigned long previousTime1 = millis();
long timeInterval1 = 60000;

int meanTemp = 0;
int minTemp = 0;
int maxTemp = 0;
int raumTemp = 0;
int meanCounter = 0;
int helpMean = 0;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

#define ONE_WIRE_BUS 0
#define Anzahl_Sensoren_DS18B20 4 // Anzahl der angeschlossenen Sensoren - Mögliche Werte: '1','2',... 

#define DS18B20_Aufloesung 12
DeviceAddress DS18B20_Adressen;
 
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
float Temperatur[Anzahl_Sensoren_DS18B20]; 



void setup(){
  // Für beelogger-SMD: Aktivierung der Stromversorgung für Sensoren, Module und DS3231 
  #define Power_Pin 4

  if ((Anzahl_Sensoren_DS18B20 > 0)) {
  sensors.begin();
  for(byte i=0 ;i < sensors.getDeviceCount(); i++) {
    if(sensors.getAddress(DS18B20_Adressen, i)) {
      sensors.setResolution(DS18B20_Adressen, DS18B20_Aufloesung);
      }
    }
  }

  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  
}

void loop(){
  unsigned long currentTime = millis();
  
  if ((Anzahl_Sensoren_DS18B20 > 0)) {
      sensors.requestTemperatures();
  
      for(byte i=0 ;i < Anzahl_Sensoren_DS18B20; i++) {
        if (i < sensors.getDeviceCount()) {
          Temperatur[i] = sensors.getTempCByIndex(i);
        }
      }
      //Serial.println("DeviceCount" + String(sensors.getDeviceCount()));
      raumTemp = Temperatur[0];
      
      for (int y=1; y<sizeof(Temperatur)/sizeof(float);y++)
      {
        helpMean += Temperatur[y];
        meanCounter++;
        
        //Check for max Temp
        if(Temperatur[y] > maxTemp)
        {
          maxTemp = Temperatur[y];
        }

        //Check for min Temp
        if(Temperatur[y] < minTemp)
        {
          minTemp = Temperatur[y];
        }
        
        //Debbuging
        //Serial.println("Sensor:" + String(y) + " " + String(Temperatur[y])); 
      }
      meanTemp = helpMean/meanCounter;
    }
    
    //Serial.println("---------------------");

   
  //lcd.setCursor(colum, row)
  lcd.setCursor(0, 0);
  lcd.print("Durchschnitt:" + String(meanTemp));
  lcd.setCursor(0, 1);
  lcd.print("Min:" + String(minTemp));
  lcd.setCursor(8, 1);
  lcd.print("Max:" + String(maxTemp));
  lcd.setCursor(0, 2);
  lcd.print("Raumtemp.:" + String(raumTemp));
  delay(100);

  //Reset temps for new inputs
  minTemp = Temperatur[1];
  maxTemp = Temperatur[0];


  //Reset Measurements after 60 seconds
  if(currentTime - previousTime1 > timeInterval1)
  {
    previousTime1 = currentTime;
    Serial.println("reset");
    meanCounter = 0;
    helpMean = 0;
    meanTemp = 0;
    maxTemp = 0;
    lcd.clear();
  }
}

  
  
