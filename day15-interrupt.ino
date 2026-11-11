constexpr uint8_t LED_PIN = 2;
constexpr uint8_t BTN_PIN = 4;

volatile bool buttonPressed = false;
volatile unsigned long lastInterruptTime = 0;



void IRAM_ATTR onButtonPress() {
  unsigned long currentTime = millis();
  if(currentTime - lastInterruptTime >200){
     buttonPressed = true;  // 只有一句代码：打个标记
     lastInterruptTime = currentTime;

  }
}
void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(BTN_PIN),onButtonPress,FALLING);
  Serial.println("Interrupt Ready");
}

void loop() {
  if(buttonPressed){
    buttonPressed = false;
    static bool ledState =false;
    ledState = !ledState;
    digitalWrite(LED_PIN,ledState ? HIGH : LOW);
    Serial.println("BUtton pressed via Interrupt!");

  }

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 2000) {
    lastPrint = millis();
    Serial.println("Main loop is running...");
  }

}
