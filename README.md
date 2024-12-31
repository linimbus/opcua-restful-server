# opcua-restful-server 

## V1.0版本

## 介绍
xxx

## 软件架构
xxx

## 编译教程

### linux 版本编译

1. 准备一台 linux amd64 设备，以 ubuntu 24.04 版本为例，其他系统类似，区别主要是依赖安装包的命令不一样。

2. 准备依赖软件包

```
sudo apt-get update
sudo apt-get install build-essential python3 \
            tar curl zip unzip gcc g++ git clang-format libmysqlcppconn-dev \
            pkg-config automake autoconf libtool flex bison linux-tools-generic
```

3. 克隆 vcpkg 项目到本地目录。

```
git clone https://github.com/microsoft/vcpkg
cd vcpkg
./bootstrap-vcpkg.sh
```

4. 设置 vcpkg 环境变量到 PATH

```
export VCPKG_ROOT=/root/workspace/vcpkg
export PATH=${VCPKG_ROOT}:${PATH}
```

5. 克隆本项目到本地目录

```
git clone git@github.com:linimbus/opcua.git
```

6. 在 odbc 项目目录执行
```
bash build_release.sh
```

注意：如果失败，可能是网络下载问题，建议设置 HTTP_PROXY & HTTPS_PROXY 代理服务。

#### windows 版本编译

1.  准备一台 windows 10/11 环境

2.  安装 Visual Studio 2022 Community 版本， 并且安装c++开发环境, https://visualstudio.microsoft.com/vs/community/

3.  `git clone https://github.com/microsoft/vcpkg.git`

4.  `.\vcpkg\bootstrap-vcpkg.bat`

5.  `.\vcpkg\vcpkg integrate install`

6.  添加系统环境变量： VCPKG_ROOT=C:\workspace\vcpkg

7.  安装cmake工具: https://cmake.org/download/

8.  `git clone git@github.com:linimbus/opcua.git`

9. vscode IDE 打开flight-odbc目录

10. CTRL+SHFIT+P 执行 CMake Build 选项

## 使用说明

1.  编译完成之后得到两个二进制文件

linux 平台的文件为：
```
publish/bin/opcua-restful-server
```

windows 平台的文件为：
```
.\publish\bin\opcua-restful-server.exe
```

2.  启动服务端程序参考

```
./publish/bin/opcua-restful-server -listen_address "0.0.0.0" -listen_port 3344
```

3.  启动客户端程序参考

```
./publish/bin/opcua-restful-server
```

启动成功之后，会出现命令行字符；可以输入 help 命令获取帮助信息。
