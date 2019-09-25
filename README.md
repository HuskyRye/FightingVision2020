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
|刘志磊|something|-|

---

## 一、运行环境
* 计算设备：MiniPC 或 DJI Manifold 2 (兼容 intel NUC 和搭载相应尺寸扩展板的 Nvidia Jetson TX1, TX2 设备）
* 工业摄像头：华谷动力 WP-UT130
* 操作系统：Windows10 64位 或 Ubuntu18.04 ( Linux内核版本号从2.6.32到4.18.0 )
* 运行库：OpenCV 4.1.1，MV Viewer相机驱动
* 工具链：CMake3.8+ with Visual Studio 2019 或 CMake3.8+ with Qt Creater 5

相机驱动下载地址：
[相机驱动](http://download.huaraytech.com/pub/sdk/) ( Windows 安装 Ver2.2.2，Linux 安装 Ver2.2.1 )

编译方式：
* 使用 CMake 编译后 make
* 或使用 Visual Studio 2019 或 Qt Creater 打开 CMake 项目 (通过 CMakeLists.txt )

**注意: Windows 平台需在编译后将 Runtime 文件夹内的文件拷贝到可执行文件的同级目录里**
