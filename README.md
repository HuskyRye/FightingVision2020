# FightingVision2020
An open source vision software for RoboMaster2020

[^_^]: # (哈哈我是注释，不会在浏览器中显示)
[^_^]: # (战队还没有确定要开源，所以先注释掉)
[^_^]: # (本代码为中南大学FYT战队RoboMaster2020赛季视觉部分)

如有疑问或者想交流的朋友欢迎积极联系我们，联系方式如下：

|作者|负责部分|QQ|
|-|-|-|
|刘臣轩|代码框架与视觉识别|1412167533|
|赵桐|相机驱动|783944621|

---

## 一、运行环境
* 计算设备：MiniPC 或 DJI Manifold 2 (兼容 intel NUC 和搭载相应尺寸扩展板的 Nvidia Jetson TX1, TX2 设备）
* 工业摄像头：华谷动力 WP-UT130
* 操作系统：Windows 10 64位 或 Ubuntu 16.04 ( Linux内核版本号从2.6.32到4.18.0 )
* 运行库：OpenCV 4，MV Viewer相机驱动
* 工具链：CMake3 with Visual Studio 2019 或 CMake3 with Qt Creater 5

相机驱动下载地址：
[相机驱动](http://download.huaraytech.com/pub/sdk/) ( Windows 安装 Ver2.2.2，Linux 安装 Ver2.2.1 )

## 二、程序编译运行以及开机自启部署

### 1.编译运行

* Ubuntu 16.04
```shell
mkdir build
cd build
cmake ..
make
sudo ./FightingVision2020
```

* Windows 10

### 2.开机自启部署

* Ubuntu 16.04
```shell
cd Tools
sudo ./Deployment.sh
```

注：开机自启状态查询
systemctl status FightingVision2020.service （服务详细信息）
systemctl is-active FightingVision2020.service (仅显示是否 Active)