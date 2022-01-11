# DUT0BUG-RM-CV-2021

## 项目简介
本仓库为2021年大连理工大学凌BUG战队manifold电控组代码
### 开发环境
```
DJI Manifold 2-G, Ubuntu 16.04
迈德威视工业相机SDK, OpenCV+OpenCV_contrib 4.5.2, Qt 5
```
### 开发者
王老师和他的工具人们
## 项目进度
功能|描述|进度
:--|:--|:--
全局日志|使用Qt内置日志功能实现|100%
统一配置文件|使用Qt.Json和OpenCV实现配置文件读写|100%
多线程|使用Qt.Thread管理多线程|0%
装甲板检测|灯条检测+数字识别|0%
能量机关|区域框选+SVM识别|10%
串口通信|使用Uart/Can与下位机通信|10%
角度解算|PnP/激光测距|0%
运动预测|预测+防抖|0%
## 文件树


算法总体流程解释请移步至本组织的doc仓库下
[传送门](https://gitee.com/DLUT-0BUG/docs)