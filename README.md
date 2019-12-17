关于本项目
------------------
本项目通过git开源
-  github仓库地址：https://github.com/Embdefire/emXGUI_demo_EBF6ULL

-  gitee 地址：https://gitee.com/wildfireteam/emXGUI_demo_EBF6ULL

EBF6ULL emxGUI程序简介
------------------

野火提供的emXGUI综合示例程序，是基于emXGUI图形软件库及FreeRTOS操作系统制作的人机交互界面程序。该程序界面酷炫，性能卓越，非常适合作为嵌入式人机交互界面的参考示例，在基础教程代码中已将野火所有开发板和屏幕适配，目前综合程序已适配表格1‑1中的野火系列开发板。

表格 1‑1 emXGUI综合示例程序适配的开发板

| 开发板                | 适配情况 | 图形库 | 操作系统                 |
|-----------------------|----------|--------|-----------------------|
| F429_挑战者开发板_V1   | 已适配   | emXGUI | RT-Thread，FreeRTOS     |
| F429_挑战者开发板_V2   | 已适配   | emXGUI | RT-Thread，FreeRTOS     |
| H743_Pro开发板        | 已适配   | emXGUI | FreeRTOS               |
| H750_Pro开发板        | 已适配   | emXGUI | FreeRTOS               |
| F103_霸道开发板       | 已适配   | emXGUI  |FreeRTOS               |
| i.MX 6UL系列开发板   |  已适配 | emXGUI | FreeRTOS                |
| F407_霸天虎开发板     | 正在开发 | emXGUI | FreeRTOS                |
| F767_挑战者开发板_V2  | 正在开发 | emXGUI | FreeRTOS                |
| H743_挑战者开发板_V2 | 正在开发 | emXGUI | FreeRTOS                |
| i.MX RT1052系列开发板 | 正在开发 | emXGUI | FreeRTOS                |

如何运行emXGUI综合程序
----------------------

野火emXGUI综合程序提供开源代码，用户可编译并下载该程序到开发板上运行。

### 所需要的环境

#### 硬件环境

emXGUI综合程序所需要的硬件运行环境如下：

-   本仓库配套野火EBF6UL/6ULL S1 Pro开发板（MiNi应该也能，未测试）

-   5寸液晶屏或者7寸液晶屏

-   J-Link下载器（用于下载到DDR调试）

-   Micro USB数据线（用于下载到eMMC）

#### 软件环境

-   Eclipse，用于编译、在DDR中调试

-   mfgtools-release，用于将生成的bin文件烧录到eMMC（烧录NAND中需要修改配置）中

### emXGUI综合程序资料

找到野火提供的emXGUI综合程序资料，资料中主要包含有各配套开发板的“emXGUI综合示例程序”源代码。

### 准备开发板

请按如下步骤准备开发板
 #### 1.Eclipse在DDR中调试:

-   给开发板接上配套的5寸或者7寸屏幕

-   使用J-Link下载器连接开发板，另一端连接至电脑

-   打开开发板的电源开关

-   编译下载，开始全速运行

 #### 2.使用mfgtools-release烧录到eMMC:

-   给开发板接上配套的5寸或者7寸屏幕

-   使用Micro USB数据线连接开发板，另一端连接至电脑

-   打开开发板的电源开关

-   按住RESET和MODE按键，先松开RESET,再松开MODE按键，使开发板进入下载模式

-   运行MfgTool2.exe软件，点击Start开始下载，下载完成后复位开发板即可运行emXGUI