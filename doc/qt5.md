## 在manifold上安装qt5开发环境

### 使用源码方式安装

#### 1.安装相应的库

```
sudo apt-get install libxcb-xinerama0-dev 
sudo apt-get install '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev
```

直接使用apt安装。

#### 2.配置configure

```
cd /home/dji/data/qt-everywhere-src-5.15.2
./configure -opensource -nomake examples -nomake tests -skip qtwebengine
```

由于移植系统时，只会复制emmc中的内容，硬盘中的内容不会复制，此处选择的安装路径为默认的/usr/local，编译路径在/home/dji/data，即硬盘挂载的目录下，以防止emmc内存不足。注意不要加入-developer-build选项

注意若不跳过webengine模块，会遇到一堆bug，且耗费时间极长，源码大小为2.1G。

参考：[](https://wiki.qt.io/Building_Qt_5_from_Git)

#### 3.build&install

```
make -j6
sudo make install
cd /usr/local/Qt-5.15.2/bin
./qmake -v
```

经过漫长的等待之后，编译完成。使用-v参数输出为：

QMake version 3.1   Using Qt version 5.15.2 in /usr/local/Qt-5.15.2/lib

#### 4.安装Qt creator

上述完成了Qt 5.15.2的编译安装，可以使用命令行方式进行编译，但是我们需要给它套一个壳:sweat_smile:，即安装IDE：Qt creator。参考：https://wiki.qt.io/Building_Qt_Creator_from_Git/zh

首先需要将3中安装的qmake设置为环境变量，manifold原系统中装有qmake4.8.7，在终端中输入qmake时，输出的版本信息为4.8.7，需要将/usr/bin中的软连接qmake删除，将安装的qmake 5.15.2 创建一个软链接。

```
cd /usr/bin
sudo rm qmake qmake-qt4 qml* qt*
sudo ln -s /usr/local/Qt-5.15.2/bin/qmake ./qmake
```

Qt creator源码的文档里面要求最好安装ninja和LLVM，这里只安装了ninja。下面开始编译：

```
sudo apt-get install ninja-build
cd /home/dji/data/
mkdir qt-creator-build
cd qt-creator-build
qmake -r ../qtcreator.pro
make -j6
```

接下来又是漫长的等待（连个进度条或者类似cmake的百分比都没，折磨）

编译结束之后，将qtcreator安装至目录/bin下，并添加到环境变量

```
sudo make install
sudo ln -s /bin/qtcreator /usr/bin/qtcreator
sudo qtcreator
```

可以看到qtcreator的界面，接下来就可以使用qt写程序了:happy:（不使用sudo，初始化会报错，待解决）

#### 5.配置工程

打开qtcreator之后，新建命令行工程：Projects-New-Application-Qt Console Application，修改工程名称和路径，Build system选择默认qmake，跳过翻译文件，在Kit Selection选项下选择系统检测到的默认Desktop，不添加版本控制，完成即可。

PS:安装完qt和qtcreator后，emmc使用率为10.7/29.5GB，还有约17.3GB剩余。编译qt时一定要先看官方文档！！！不要先看csdn上的博客！！！Total time wasted on building qt and qt creator : 2 days.

### 使用apt安装(不推荐)

```
sudo apt-get install qt5-default
sudo apt-get install aptitude
sudo aptitude install qtcreator
sudo apt-get install qtdeclarative5-dev
```

使用apt-get安装qtcreator时，可能会有libclang1-3.6的依赖错误，因此选择aptitude解决该问题，在跳出第一个选项时选择N，在第二个选项选择Y，（会把一大堆东西都删了，如果没有移除预装的ros等环境），等待几分钟后安装完成。

使用apt方式安装的qt版本为5.5过旧，不使用。

### Bug 记录

#### 1.Parse error at “std“

删除build目录下的.qmake.stash文件即可

```
rm ./build/.qmake.stash
```

#### 2.Cannot load library socketcan

安装libsocketcan-dev

```
sudo apt-get install libsocketcan-dev
```

