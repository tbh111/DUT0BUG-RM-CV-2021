# DUT0BUG-RM-CV-2021

## 项目简介

本仓库为2021年大连理工大学凌BUG战队视觉组代码（非战队官方开源），仅为纪念本人于2021.5.1-2021.8.6与王飞龙老师及各位队友在大福楼、北京分区赛以及深圳国赛期间度过的短暂RMer时光。

### 开发环境

```
DJI Manifold 2-G, Ubuntu 16.04
迈德威视工业相机SDK, OpenCV + OpenCV_contrib 4.5.2
Qt 5.15.2 + Qt creator 4.15.1(自主源码编译)
```

由于在北部分区赛中，使用Linux系统中的多线程框架容易导致程序在运行一段时间后崩溃（core dumped），且使用妙算2（DJI的智商税及劝退产品）上的串口与下位机通信的效率较低，因此在深圳国赛中，我们使用了自主源码编译（折腾了好久）的Qt 5.15.2 LTS + Qt creator 4.15.1 + OpenCV 4.5.2 进行项目构建，并使用Can总线以及 Qt中集成的QCanBus模块与下位机进行通信。所有算法均使用OpenCV传统方法构建，于是我在深圳，在赛场炫酷（花里胡哨）的场地灯光中，同很多战队中坚守传统的视觉人度过了破防的一周。

### 开发者

Tongbh, Myron & Fly Dragon

## 系统功能

| 功能     | 描述                    | 国赛应用情况 |
|:------ |:--------------------- |:------ |
| 全局日志   | 使用Qt内置日志功能实现          | ❎      |
| 统一配置文件 | 使用Qt.Json和xml实现配置文件读写 | ❎ / ✅  |
| 多线程    | 使用Qt.Thread管理多线程      | ✅      |
| 装甲板检测  | 灯条检测+数字识别             | ✅      |
| 能量机关   | 区域框选+SVM识别            | ❎      |
| 下位机通信  | 使用Uart/Can与下位机通信      | ❎ / ✅  |
| 角度解算   | PnP/激光测距              | ✅ / ❎   |
| 运动预测   | 预测+防抖                 | ❎      |

## 项目结构

```
├── DUT0BUG-RM-CV-2021.pro
├── LICENSE
├── README.md
├── build
├── can-bus-test          // 一个用于测试系统can总线的小工具
│   ├── build
│   ├── can-bus-test.pro
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   └── mainwindow.ui
├── config
│   ├── SVM_MODEL.xml    // 能量机关装甲板识别模型
│   ├── camera.xml       // 哨兵下云台/步兵镜头标定数据
│   ├── config.json      // 系统参数
│   └── long_camera.xml  // 哨兵上云台长焦镜头标定数据
├── imageshow.ui         // 摄像头图像GUI显示（可关闭）
├── include
│   ├── CameraApi.h
│   ├── CameraDefine.h
│   ├── CameraStatus.h
│   ├── anglesolver.h
│   ├── armordetector.h
│   ├── capturethread.h
│   ├── common.h
│   ├── commprotocol.h
│   ├── config.h
│   ├── energydetector.h
│   ├── imageshow.h
│   ├── processthread.h
│   └── taskmanager.h
├── lib
│   ├── arm
│   │   └── libMVSDK.s
│   ├── arm64
│   │   └── libMVSDK.so
│   ├── arm_softfp
│   │   └── libMVSDK.so
│   ├── x64
│   │   └── libMVSDK.so
│   └── x86
│   └── libMVSDK.so
├── log
│   └── log.txt
└── src
├── anglesolver.cpp      // pnp角度解算
├── armordetector.cpp    // 识别装甲板
├── capturethread.cpp    // 相机图像获取线程
├── commprotocol.cpp     // 通信协议
├── config.cpp           // 参数读取
├── energydetector.cpp   // 能量机关识别
├── imageshow.cpp        // GUI图窗
├── main.cpp             // 程序入口
├── processthread.cpp    // 图像处理线程
└── taskmanager.cpp      // 线程调度
```

## 系统实现思路

由于篇幅过长，请移步至本项目中的doc文件夹：[]()

## 写在后面

2021年，我经历了太多，3月独自完成大创结项与专利申请，4月底临危受命在凌BUG战队中负责视觉工作，5月在北京某地下仓库度过了不见天日的一周，8月在深圳某别墅地下室度过了同样艰辛的一周，再到9月保研失败，12月并不顺利的头铁考研。感谢一直支持鼓励我的朋友们，感谢2021年拼命的自己。
