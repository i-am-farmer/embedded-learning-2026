1. 串口是字节流，Serial.read() 一次读一个字符，所以要攒到换行符再当整行处理。

1. 字符串比较必须用 strcmp，不能用 ==，因为数组名比较的是地址。

1. buffer[i] = '\0' 不能省，strcmp 和 Serial.println 都靠它知道字符串在哪结束。
1. i < sizeof(buffer) - 1 判断，超出就丢弃，保证不越界，最后强制 buffer[i] = '\0'。这句话就是面试素材，写进 notes/day9-uart-command.md
