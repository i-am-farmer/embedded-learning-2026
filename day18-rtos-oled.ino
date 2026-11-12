#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_BMP085_U.h>
#include <string.h>

// 定义屏幕宽高
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// 初始化 OLED，传入 I2C 句柄（在 setup 里 Wire.begin 之后传）
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,&Wire,-1);

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10180);

//i2c锁
SemaphoreHandle_t i2cMutex;
//记录温度
float  lastTemperature = 25.0;
//初始话阈值
volatile float threshold = 30.0;
//标记是否报警
volatile bool alarmEnabled = true;
//记录当前温度
volatile float currentTemp = 25.0; 


constexpr uint8_t LED_PIN = 2;


// ========== 任务1：传感器读取 ==========
void Task_Sensor(void *pvParameters){
   while(1){
    if(alarmEnabled){
      float temp;
      if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
      bmp.getTemperature(&temp);
      xSemaphoreGive(i2cMutex); // 释放 I2C 锁
      }
      currentTemp = temp;
    }   

    vTaskDelay(1000 / portTICK_PERIOD_MS);
   }
}


// ========== 任务2：传感器读取显示 ==========
void Task_Display(void *pvParameters){
  while(1){
    
    if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
      
      
      display.clearDisplay();
      display.setCursor(0, 0);

       // 单色屏不能设红色，我们用反色或者反转来模拟报警
      if(alarmEnabled && currentTemp > threshold){
        display.invertDisplay(true); // 反色报警特效
      } else {
        display.invertDisplay(false);
      }

      display.print("Temp: ");
      display.println(currentTemp);
      display.print("threshold: ");
      display.println(threshold);
      display.print("Status: ");
      display.println(alarmEnabled ? "ACTIVE" : "OFF");


      display.display(); // 刷新屏幕
      xSemaphoreGive(i2cMutex); // 释放 I2C 锁
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}


// ========== 任务3：UART命令解析 ==========
void Task_UART(void *pvParameters){
    char buffer[32];
  int i = 0;

  
    while(1){

      
        if(Serial.available() > 0){
        char c = Serial.read();
        if(c != '\n' && c != '\r'){
          if(i<sizeof(buffer) - 1){
            buffer[i] = c;
            i++;
          }
        }else{  
          buffer[i] = '\0';
          
            if(strcmp(buffer,"off") == 0){
            alarmEnabled = false;
            Serial.println("Alarm disabled");
          }else if(strncmp(buffer,"set ",4) == 0){
            threshold = atof(buffer + 4);
            alarmEnabled = true;
            Serial.print("threshold: ");
            Serial.println(threshold);
          }
          i = 0;
        }
      }
 
      
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
  
}

// ========== 任务3：LED报警灯 ==========
void Task_Alarm(void *pvParameters){
  pinMode(LED_PIN, OUTPUT);
      while(1){
        if (alarmEnabled && (currentTemp > threshold)){
            digitalWrite(LED_PIN, HIGH);
            vTaskDelay(200/ portTICK_PERIOD_MS);
            digitalWrite(LED_PIN, LOW);
            vTaskDelay(200/ portTICK_PERIOD_MS);
          }else{
            digitalWrite(LED_PIN, LOW);
            vTaskDelay(100/ portTICK_PERIOD_MS);
          }
          
              
      } 

}


void setup() {
  Wire.begin(21, 22);
  Serial.begin(115200);

  if(!bmp.begin()){
    Serial.println("BMP180 not found");
    while(1);
  }

  i2cMutex = xSemaphoreCreateMutex();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // 卡死
  }
  
  display.clearDisplay(); // 清屏
  display.setTextSize(1); // 字体大小
  display.setTextColor(SSD1306_WHITE); // 字体颜色
  display.setCursor(0, 0); // 坐标 (x, y)
  display.println("System Ready!");
  display.display(); // 必须调用这句才会刷新到屏幕上


    // 创建三个任务
  xTaskCreate(Task_Sensor, "Sensor", 2048, NULL, 1, NULL);
  xTaskCreate(Task_UART, "UART", 2048, NULL, 2, NULL);   // UART优先级高一点，保证响应快
  xTaskCreate(Task_Display, "Display", 2048, NULL, 1, NULL);
  xTaskCreate(Task_Alarm, "Alarm", 2048, NULL, 1, NULL);
}

void loop() {
  
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
