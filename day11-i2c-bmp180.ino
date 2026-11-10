#include<Wire.h>
#include<Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10180);
constexpr uint8_t SDA_PIN = 21;
constexpr uint8_t SCL_PIN = 22;



void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN,SCL_PIN);

  if(!bmp.begin()){
    Serial.println("BMP180 not found, check wiring");
    while(1);
  }
  Serial.println("BMP180 ready");
}

void loop() {

  sensors_event_t event;
  bmp.getEvent(&event);


  if(event.pressure){
    Serial.print("Pressure: ");
    Serial.print(event.pressure);
    Serial.println(" hPa");
  }else{
    Serial.println("Sensor error");
  }

  //读取温度
  float temperature;
  bmp.getTemperature(&temperature);
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.println(" ℃");
  delay(1000);
}
