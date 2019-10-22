# FightingVision2020
An open source vision software for RoboMaster2020

[^_^]: # (哈哈我是注释，不会在浏览器中显示)
[^_^]: # (战队还没有确定要开源，所以先注释掉)
[^_^]: # (本代码为中南大学FYT战队RoboMaster2020赛季视觉部分)

如有疑问或者想交流的朋友欢迎积极联系我们，联系方式如下：

|作者|负责部分|QQ|
|-|-|-|
|刘臣轩|代码框架与视觉算法|1412167533|
|赵桐|相机驱动与自启脚本|783944621|

---

## 一、运行环境
* 计算设备：MiniPC 或 DJI Manifold 2 (兼容 intel NUC 以及 Nvidia Jetson TX1, TX2）
* 工业摄像头：华谷动力 WP-UT130
* 操作系统：Windows 10 64位 或 Ubuntu 16.04 ( Linux内核版本号从2.6.32到4.18.0 )
* 运行库：OpenCV 4，MV Viewer相机驱动
* 工具链：
  * Windows 10 : CMake3 + Visual Studio 2019
  * Ubuntu 16.04 : CMake3 + GCC5

相机驱动下载地址：
[相机驱动](http://download.huaraytech.com/pub/sdk/) ( Windows 安装 Ver2.2.2，Linux 安装 Ver2.2.1 )

## 二、程序下载编译运行

* Ubuntu 16.04
```shell
# 下载 FightingVision2020 源码
git clone https://github.com/HuskyRye/FightingVision2020.git
cd FightingVision2020

# 编译源码
mkdir build
cd build
cmake ..
make

# 运行
sudo ./FightingVision2020
```

* Windows 10

  使用 Visual Studio 2019 打开 CMake 项目，编译运行

## 三、开机自启部署（仅支持Ubuntu）
### 1.部署
```shell
cd Tools
sudo chmod +x ./Deploy.sh
sudo ./Deploy.sh
```
### 2.开机自启状态查询
```shell
systemctl status FightingVision2020.service     #服务详细信息
systemctl is-active FightingVision2020.service  #仅显示是否 Active
```

### 3.取消部署
```shell
cd Tools
sudo chmod +x ./Undeploy.sh
sudo ./Undeploy.sh
```
