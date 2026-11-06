# ESP32 嵌入式入门实验（Wokwi）

## Day 5: 状态机消抖（核心实验）
- 目标：用 `IDLE / DEBOUNCING / PRESSED` 三状态实现非阻塞消抖，按一次切换一次 LED。
- 关键点：
  - `lastDebounceTime` 在读数变化时重置；
  - 先检查 `reading == HIGH` 再检查超时，避免已松开的短按被误判；
  - `PRESSED` 等 `reading == HIGH` 才回 `IDLE`，防止按住连切。
- 状态转换图：
  //IDLE  没有任何动作 且ButtonState是HIGH
//DEBOUNCING   reading是LOW  但是不确定
//PRESSED 按钮确定被按下了
//IDLE --> 等待按下 --> 按下后记录当前的lastDebounceTime --> DEBOUNCING --> 检查是否是稳定按下了
//--> 如果reading是HIGH,就是抖动 不用管  如果是LOW且超过50ms就是确定被按下 LED状态产生变化 --> PRESSED
// --> 等待被松开 当检测到如果reading是HIGH 就恢复IDLE
- 文件：`day5-state-machine-final/sketch.ino`

## 踩过的坑
- 只看 `reading == LOW` 判断按下，导致按住时 LED 连续切换，改成比较上一次和这一次的读数（边沿检测）。
- 状态打印放在每个 `case` 入口，串口刷屏，改成只在状态变化时打印。
- 枚举名拼写 `DEBIUNCING`，全局改成 `DEBOUNCING`。
