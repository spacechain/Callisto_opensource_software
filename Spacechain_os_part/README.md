# Callisto Spacenode Software Spacechain OS part

## 背景介绍

Spacenode软件是SPC开发的一款用于批量处理SpaceChain比特币安全支付交易的星上软件，该版本为运行在Callisto社区开发板上的改版，与在轨节点上运行的软件功能完全相同。软件整体分为Linux OS运行部分和SpaceChain OS运行部分两部分,两部分软件通过混合系统的虚拟网卡进行数据交互，协同完成交易处理工作。

本目录下为Spacenode Spacechain OS运行部分的软件工程。

部分代码复用自trezor-crypto加密库。

## 功能介绍

* 状态监控
* 脚本、依赖库、应用升级
* 交易签名

## 编译

* 执行make指令开始编译。
* 在build目录下会生成编译好的可执行文件。
