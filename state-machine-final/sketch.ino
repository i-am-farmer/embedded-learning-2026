
//IDLE  没有任何动作 且ButtonState是HIGH
//DEBOUNCING   reading是LOW  但是不确定
//PRESSED 按钮确定被按下了
//IDLE --> 等待按下 --> 按下后记录当前的lastDebounceTime --> DEBOUNCING --> 检查是否是稳定按下了
//--> 如果reading是HIGH,就是抖动 不用管  如果是LOW且超过50ms就是确定被按下 LED状态产生变化 --> PRESSED
// --> 等待被松开 当检测到如果reading是HIGH 就恢复IDLE
enum ButtonState{
  IDLE,
  DEBOUNCING,
  PRESSED
};

ButtonState btnState = IDLE;
ButtonState lastPrintedState = IDLE;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;

const int LED_PIN = 2;
const int BTN_PIN = 4;

bool ledState = false;
int pressCount = 0;



void setup(){
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  Serial.begin(115200);

}
 void loop(){

  int reading = digitalRead(BTN_PIN);
  
  switch(btnState){
    case IDLE:
    if(reading == LOW){
      lastDebounceTime = millis();
      btnState = DEBOUNCING;
    }
    
    break;
    case DEBOUNCING:
    if(reading == HIGH){
      btnState = IDLE;
    }else if(millis() - lastDebounceTime > DEBOUNCE_DELAY){
      ledState =!ledState;
      digitalWrite(LED_PIN,ledState ? HIGH:LOW);
      btnState = PRESSED;
     pressCount ++;
    Serial.print("pressCount: "); 
    Serial.println(pressCount);
    }
    // if(millis() - lastDebounceTime > DEBOUNCE_DELAY){
    //   ledState =!ledState;
    //   digitalWrite(LED_PIN,ledState);
    //   btnState = PRESSED;
    // }else if(reading == HIGH){
    //   btnState = IDLE;
    // }
   
    break;
    case PRESSED:
    if(reading == HIGH){
      btnState = IDLE;
    }
    
    break;
  }

 }