
#include<Wire.h>
#include<Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10180);
constexpr uint8_t SDA_PIN = 21;
constexpr uint8_t SCL_PIN = 22;

constexpr uint8_t LED_PIN = 2;
int i = 0;
char buffer[32];
float threshold = 30.0;       // 报警阈值
bool alarmEnabled = true;     // 报警系统是否开启
unsigned long lastReadTime = 0; // 上次读传感器的时间
constexpr unsigned long READ_INTERVAL = 1000; // 1秒读一次，不要读太快  

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Wire.begin(SDA_PIN,SCL_PIN);

  if(!bmp.begin()){
    Serial.println("BMP180 not found, check wiring");
    while(1);
  }
  Serial.println("BMP180 ready");
}

void loop() {


    if(Serial.available() > 0){
    char c = Serial.read();
    if(c != '\n' && c != '\r'){
      if(i < sizeof(buffer) - 1){
        buffer[i] = c;
        i++;
      } 
    }else{
      buffer[i] = '\0';
      if(strcmp(buffer,"off") == 0){
        alarmEnabled = false;
        digitalWrite(LED_PIN, LOW);
        Serial.println("Alarm disabled");
      }else if(strncmp(buffer,"set ",4) == 0){
        threshold = atof(buffer + 4);
        Serial.print("Threshold set to: ");
        Serial.println(threshold);
        alarmEnabled = true;
      }else{
        Serial.println("unknown command");
      }
    }
  }

  if(alarmEnabled && (millis() - lastReadTime >= READ_INTERVAL)){
    lastReadTime = millis();
     

      float temperature;
      bmp.getTemperature(&temperature);

      Serial.print("Current Temp: ");
      Serial.print(temperature);
      Serial.println(" C");
      
    // 报警判断
  if (temperature > threshold) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("ALARM! Temp exceeded threshold!");
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  }

}
