# RTT-control-board

#### 介绍
使用rt-thread做的控制板，对接控制通信模块和传感器模块，进行传感器采集和数据上传。
作为历史封存，工程编译可能需要添加头文件地址，之前是用的公共代码，上传时才将文件全部整合了。
将电脑历史工程全部扔github，然后去修修

#### 硬件
MCU :STM32F103ZET6  8M外部晶振
串口1作为调试口
串口2作为AT通信口

#### 使用组件
AT client
FreeModBus
传感器管理器
ulog
若干传感器驱动




