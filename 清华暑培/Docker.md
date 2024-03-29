# Docker学习

+ docker介绍
+ docker基本概念
+ docker命令
+ DockerFile
+ DockerCompose

[docker权限问题](https://www.runoob.com/note/51562)

## 介绍

容器化技术，使用go语言实现，基于Linux内核的cgroup、namespace以及UnionFS等技术实现对进程的封装隔离。

> 配环境的通过经历，特别是配置应用的运行环境，往往会有多版本冲突等问题。

虚拟化技术---> 将应用本身和运行环境在某种程度上解耦！
+ 封装：应用和运行环境封装在一起，形成一个虚拟的运行环境
+ 隔离：应用与应用之间的运行环境相互不影响。
+ 同时我们还需要一种手段，将应用和所需要的环境打包并交付给其他人使用。
+ 总结就是将系统环境和应用环境隔离


Docker作为**轻量级**的虚拟化技术

### 总结：为什么要用Docker?

+ 灵活：容器可以随时启动、停止、删除，不会对宿主机造成影响
+ 隔离：不同容器之间相互隔离
+ 可移植：可以在不同宿主机上运行
+ 可复用：打包交付给他人使用
+ 轻量级：只包含应用和其运行环境，不需要虚拟整个完整操作系统


## 概念

### 镜像

+ 操作系统分内核空间和用户空间
+ 内核启动后，挂载root文件系统`/`, 提供用户空间支持。
+ Docker镜像是一个特殊的root文件系统
  + 在这一意义上和ISO等镜像文件一样
+ 镜像是**静态的**，不会包含任何动态的数据，其内容在构建之后不会再改变

#### 分层存储

+ 听起来Docker文件像一个压缩包？
+ 但实际上Docker镜像是由一层一层的文件系统组成的，换句话说，是一堆压缩包
+ UnionFS（联合文件系统）技术可以将不同文件系统联合挂载到同一个目录下
+ 不同文件系统叠加形成了最终镜像中的root文件系统
+ 同样镜像构建也是一层层叠加进行的
+ **记住镜像是多层的！！**

### 容器

+ 通过类（静态定义），我们可以创建一个实例（动态存在）
+ 通过程序（静态定义），我们可以创建一个进程（动态存在）
+ 同样地，通过镜像（静态定义），我们可以创建一个容器（动态存在）

创建是实例时，类的定义不会被修改。启动进程时，程序的定义不会被修改。同样地，创建容器时，镜像的内容不会被修改。

#### 容器的运行

+ 镜像是多层的。容易运行的时候，Docker在镜像最顶层创建一个可读写的**容器存储层**，应用就在镜像层和存储层叠加后形成的文件系统中运行。
+ 容器本质上就是一个在这特殊文件系统中运行的一个或一些进程。特殊之处在于，这些进程运行在独立的命令空间中。
+ 容器存储层的生命周期和容器一样，容器消亡，容易存储层也随之消亡。
+ 因此，容器是**易失**的，并不能持久化保存数据
+ 要让容器持久化保存数据，需要使用**数据卷或绑定挂载**
  + 数据卷：提供一个或多个容器使用的特殊目录，绕过UnionFS，可以实现数据共享和持久化。容器消亡，数据卷不会。
  + 绑定挂载：将宿主机上的目录挂载到容器中

### 仓库

+ 镜像创建完成后，需要一个集中存储、分发镜像的服务。这类服务被称为注册服务。
+ Docker Hub作为官方的注册服务。

## 基本命令

1. `docker run`


## Dockerfile

## DockerCompose