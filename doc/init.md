## 初始化manifold

#### 1.刷机

见刷机教程

[https://dl.djicdn.com/downloads/manifold-2/20190528/Manifold_2_User_Guide_v1.0_CHS.pdf](https://dl.djicdn.com/downloads/manifold-2/20190528/Manifold_2_User_Guide_v1.0_CHS.pdf)

使用安装Ubuntu 16.04的物理机或虚拟机皆可，约半小时

#### 2.卸载预装ros、OpenCV、Qt、LibreOffice

Manifold预装有ros，旧版本OpenCV，Qt4和5、LibreOffice，为了防止多个版本共存，进行卸载，可以省下一大堆空间:smile:，需要时手动安装。

```
# 卸载ros，open，qt，libreoffice
sudo apt-get purge ros-*
sudo apt-get purge libopencv*
sudo apt-get purge libreoffice-common
sudo apt-get autoremove
sudo apt-get update
sudo apt-get install apt-transport-https
```

运行最后一行命令报错：Data from such a repository can't be authenticated and is therefore potentially dangerous to use.

```
sudo chmod 777 /tmp
sudo apt-get update --allow-unauthenticated
sudo apt-get install apt-transport-https
```

```
# 卸载qt4和5
# 查看现在安装的qt版本
qt chooser -list-versions
# 卸载qt4
sudo apt-get autoremove --purge libqt4-[a-z]*
# 卸载qt5（卸不干净）
sudo apt-get remove qt*
```

#### 3.更换apt源

将/etc/apt/sources.list.d/ 目录下的本地源cuda-9-0-list-local.list，ros-latest.list，visionworks-repo.list等文件中的deb行注释

使用清华源

```
sudo gedit /etc/apt/sources.list
```

清空原有内容，输入：

```
# 默认注释了源码镜像以提高 apt update 速度，如有需要可自行取消注释
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/ xenial main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/ xenial main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/ xenial-updates main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/ xenial-updates main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/ xenial-backports main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/ xenial-backports main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/ xenial-security main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/ xenial-security main restricted universe multiverse
```

或直接去 [清华镜像](mirror.tuna.tsinghua.edu.cn)，相关链接 -- 使用帮助 -- Ubuntu Ports(ARM系统) -- 16.04LTS复制。

#### 4.开机自动挂载硬盘

manifold中emmc大小为32G，实际为29.5G，安装完系统后约剩余18G。使用内置的硬盘，开机时自动挂载，以保存录制的视频等文件。

```
# 查看分区
sudo fdisk -lu
# 对硬盘分区
sudo fdisk /dev/sda
```

此时输入d，删除已有的分区，输入n新建分区，所有参数均选择默认，最后输入w保存退出。再次查看分区，硬盘sda下，有一个为sda1的分区。

```
# 格式化分区
sudo mkfs -t ext4 /dev/sda1
sudo mount -t ext4 /dev/sda1 /home/dji/data
```

设置开机挂载(用户读写权限设置可以在fstab中设置，但不太管用 :confused:)

https://help.ubuntu.com/community/Fstab

```
sudo gedit /etc/fstab
末尾添加 /dev/sda1/ /home/dji/data ext4 defaluts 0 1
sudo chown dji:dji /home/dji/data/
reboot
```

#### 5.配置工业相机

使用的工业相机型号为MindVision MV-SUA33GC，linux SDK下载地址：[](http://mindvision.com.cn/rjxz/list_12.aspx?lcid=138)

安装：

```
cd ~/dji/linuxSDK_V2.1.0.25
./install.sh
reboot
```

编译GTK demo

```
sudo apt-get install libglib2.0-dev
sudo cp /usr/lib/aarch64-linux-gnu/glib-2.0/include/glibconfig.h /usr/include/
sudo cp /usr/lib/aarch64-linux-gnu/gtk-2.0/include/gdkconfig.h /usr/include/
```

#### 6.更换gcc和g++版本

系统默认的gcc/g++版本为5.5.0，这里更新至7.4.0，根据提示输入数字切换版本，只需切换gcc。

```
# 查看版本
gcc -v
g++ -v
# 更改版本
sudo update-alternatives --config gcc
```
