1. 用 millis() 定时读取传感器，避免 I2C 读取阻塞主循环。

1. strncmp(buffer, "set ", 4) 判断命令前缀，atof(buffer + 4) 提取参数。

1. 报警逻辑只在 alarmEnabled 为真时运行，防止 off 之后误触发。
