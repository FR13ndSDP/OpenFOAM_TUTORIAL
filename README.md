# OpenFOAM® TUTORIAL

![license](https://img.shields.io/badge/license-GPL-orange)&ensp; ![release](https://img.shields.io/badge/release-ver1.0-brightgreen) 



#### 介绍

Introduces basic C++ concepts to beginner users of the OpenFOAM open-source CFD libraries.

本教程是基于 **UnnamedMoose/BasicOpenFOAMProgrammingTutorials**  的中文翻译，并做了一些改动。

#### 使用方法

所有的代码是在WSL Ubuntu 18.04LTS 环境下编译测试的，使用OpenFOAM-v1912。推荐你也使用相同的OpenFOAM版本，因为不同版本的OpenFOAM底层代码可能很不同，尤其是相比于openfoam.org版本。

每个教程都是独立的，没有依赖关系，大部分都可以通过简单地执行`wmake`进行编译，然后打开`testCase`文件夹来运行算例。运行的步骤和方法在`Allrun`文件中。在编译或运行完成后，你可以使用`Allwclean`和`Allclean`来分别清理编译输出与算例运行输出，使文档结构复原。

因此，所有教程完整的运行步骤可以通过在每个教程的顶层目录运行以下命令完成：

```
./Allwmake
cd testCase
./Allrun
```

此外，还有一个```testAll```脚本可以顺序地构建和测试每个教程并清理屏幕输出。 这主要用于检查版本兼容性。运行这个脚本，如果一切顺利，你会得到如下的输出：

```
Checking: OFtutorial00_helloWorld/
    PASS: ./Allwmake
    PASS: ./Allrun
Checking: OFtutorial01_inputOutput/
    PASS: ./Allwmake
    PASS: ./Allrun
Checking: OFtutorial02_commandLineArgumentsAndOptions/
    PASS: ./Allwmake
    PASS: ./Allrun
...
```

最简单的理解本教程的方法就是打开源码，阅读它、并阅读注释和动手修改，实践来完全理解每一个算例。

----

#### Tutorial 0 - Hello world

展示了一个简单的OpenFOAM代码是如何运行的，在屏幕上打印一些简单但重要的信息:smile:

----

#### Tutorial 1 - Input and output

展示字典中的内容并将输出保存在文件中

---

#### Tutorial 3 - Understanding the mesh

讨论OpenFOAM是如何描述网格的并且使用代码进行网格的交互

---

#### Tutorial 4 - Basic field operations

**TODO**: 加入教程描述