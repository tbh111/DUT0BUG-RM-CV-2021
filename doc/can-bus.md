## 设置manifold的can总线

#### 1.设置can

manifold共有两个can接口，分别为can0和can1。使用下面的命令将can0比特率设置为250kbps，注意官方文档中给出的maniflod的can总线最大传输速率为2Mbps，且不能向外设供电。

```
sudo ip link set up can0 type can bitrate 250000
```

为了在出现"bus error"错误之后的100毫秒内恢复原先状态，使用下面的命令设置。

```
sudo ip link set up can0 type can bitrate 250000 restart-ms 100
```

#### 2.建立can总线设备

首先检查QCanBus类是否存在，在qmake中加入

```
QT += serialbus
```

在can通信类的头文件中加入

```
#include <QCanBus>
```

