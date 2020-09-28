# Callisto Spacenode Software Linux OS part

## 背景介绍

Spacenode软件是SPC开发的一款用于批量处理SpaceChain比特币安全支付交易的星上软件，该版本为运行在Callisto社区开发板上的改版，与在轨节点上运行的软件功能完全相同。软件整体分为Linux OS运行部分和SpaceChain OS运行部分两部分,两部分软件通过混合系统的虚拟网卡进行数据交互，协同完成交易处理工作。

本目录下为Spacenode Linux OS运行部分的软件工程。

## 功能介绍

* 数据文件获取
* 数据文件解析
* 双系统间数据交互
* 签名结果文件打包与上传

## 编译

* 下载编译链工具:在toolschain目录下提供了Callisto开发板对应的编译链工具下载链接。
* 更换编译链工具:编译链工具下载完成后，解压到toolschain目录，执行changeToolChain.sh脚本自动更换编译链工具。
* 编译:执行make命令，开始编译。
* 在build目录下会生成编译好的可执行文件。
