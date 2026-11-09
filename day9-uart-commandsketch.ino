constexpr uint8_t LED_PIN = 2;
int i = 0;
char buffer[32];

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("ready");
}

void loop() {
  if(Serial.available() >0){
    
    char c = Serial.read();

    if(c != '\n' && c != '\r'){
      buffer[i] = c;
      i++;
    }else{
      buffer[i] = '\0';
      if(strcmp(buffer,"on") == 0){
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ON");
      }else if(strcmp(buffer,"ON") == 0){
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ON");
      }else if(strcmp(buffer,"off") == 0){
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED OFF");
      }else if(strcmp(buffer,"OFF") == 0){
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED OFF");
      }else {
        Serial.print("unknown: ");
        Serial.println(buffer);
      }
        i = 0;
    }
    
  }

}
