#include <Wire.h>
#include <Adafruit_BMP085_U.h>
#include <string.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10180);

constexpr uint8_t LED_PIN = 2;
constexpr uint8_t SDA_PIN = 21;
constexpr uint8_t SCL_PIN = 22;

// 注意：跨任务共享的变量必须加 volatile
volatile float threshold = 30.0;
volatile bool alarmEnabled = true;


QueueHandle_t tempQueue; 
SemaphoreHandle_t serialMutex;


// ========== 任务1：传感器读取 ==========
void Task_Sensor(void *pvParameters){
  
    while(1){
      if(alarmEnabled){
        
          float temp;
          bmp.getTemperature(&temp);
          if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE){
          Serial.print("temperature: ");
          Serial.println(temp);
           xSemaphoreGive(serialMutex); // 用完必须释放锁！
          }

          xQueueSend(tempQueue, &temp, 10 / portTICK_PERIOD_MS);
        }
       
      
      
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

  
  
}

// ========== 任务2：UART命令解析 ==========
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
          if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE){
            if(strcmp(buffer,"off") == 0){
            alarmEnabled = false;
            Serial.println("Alarm disabled");
          }else if(strncmp(buffer,"set ",4) == 0){
            threshold = atof(buffer + 4);
            alarmEnabled = true;
            Serial.print("threshold: ");
            Serial.println(threshold);
          }
            xSemaphoreGive(serialMutex); // 用完必须释放锁！
          }
          i = 0;
        }
      }
 
      
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
  

}

// ========== 任务3：报警输出 ==========
void Task_Alarm(void *pvParameters){
  pinMode(LED_PIN, OUTPUT);
  float receivedTemp;
      while(1){
        if (xQueueReceive(tempQueue, &receivedTemp, portMAX_DELAY) == pdTRUE) {


          if (alarmEnabled && (receivedTemp > threshold)){
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

}


void setup(){
  Serial.begin(115200);
  Wire.begin(SDA_PIN,SCL_PIN);

  tempQueue = xQueueCreate(5, sizeof(float));
  if (tempQueue == NULL) {
  Serial.println("Queue creation failed!");
  }
  serialMutex = xSemaphoreCreateMutex();


  if(!bmp.begin()){
    Serial.println("BMP180 not found");
    while(1);
  }


  // 创建三个任务
  xTaskCreate(Task_Sensor, "Sensor", 2048, NULL, 1, NULL);
  xTaskCreate(Task_UART, "UART", 2048, NULL, 2, NULL);   // UART优先级高一点，保证响应快
  xTaskCreate(Task_Alarm, "Alarm", 2048, NULL, 1, NULL);
  
  
}


void loop() {
  // 什么都不做，RTOS接管
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}