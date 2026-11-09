constexpr uint8_t LED_PIN = 2;
int i = 0;
char buffer[32];
void setup() {
  
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  
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
      int value = atoi(buffer);
      value = constrain(value,0,255);
      analogWrite(LED_PIN,value);
      Serial.print("brightness: ");
      Serial.println(value);
      i = 0; 
    }
  }
 
}
