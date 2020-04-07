## OpenFOAM® Tutorials

![license](https://img.shields.io/badge/license-GPL-orange)&ensp; ![release](https://img.shields.io/badge/release-ver1.0-brightgreen) 



#### 介绍

Introduces basic C++ concepts to beginner users of the OpenFOAM open-source CFD libraries.

Forked from **UnnamedMoose/BasicOpenFOAMProgrammingTutorials** 

#### 使用方法

所有的代码是在WSL Ubuntu 18.04LTS :fa-windows:  环境下编译测试的，使用OpenFOAM-7。推荐你也使用相同的OpenFOAM版本，因为不同版本的OpenFOAM底层代码可能很不同，尤其是相比于openfoam.com版本。

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

#### Tutorial 1 - 输入输出

展示字典中的内容并将输出保存在文件中

---

#### Tutorial 3 - 理解网格

讨论OpenFOAM是如何描述网格的并且使用代码进行网格的交互

---

#### Tutorial 4 - 基本场操作 Field Operation And Manipulation

基本场操作，包括速度、压力场的建立和求解

---------

#### Tutorial 5 - 基础并行计算

关于并行计算的机理与如何进行并行信息的交互

---------

#### Tutorial6 - 自定义类

自定义类、派生类的声明和定义，其中定义了一个继承自`IOdictionary`的类，用于读取`myTransportProperties` 

---------

#### Tutorial 7 - 自定义库

展示了如何自定义一个动态链接库，将tutorial4和5中的函数写在了库中

---------

#### Tutorial 8 - 自定义边界条件

展示如何定制一个边界条件。

我们没有建立一个工程，而是仅仅实现了一个库来完成。它定义了一个圆管的入口条件，其中的入口边界层厚度是可以预先指定的。

此边界条件是固定值边界（fixedValue）边界的派生类，向其中加入了几个控制量，得以定制入口参数。其中需要注意的要点用`NOTE：`指明。需要注意的实现方法有：两个构造函数，默认的和从字符串构造边界条件的，以及`.updateCoeffs()`函数

测试算例是一根直管，使用基本的`simpleFoam`来解算，需要注意的是`0.org/U`中边界条件的定义和`system/controlDict`中自定义库的合并。这个模拟是粗网格上的3D RANS，所以在低配机器上可能要花费几分钟的时间。边界条件带来的的影响可以通过绘制x方向的速度来进行观察

---------

#### Tutorial 9 - 运行时后处理

讨论运行时后处理程序的实现，这个程序计算通过一个面区的流率，利用了`topoSet`程序来生面区。

这个程序的实现是基于一个运行时后处理对象，继承自内建的 `fvMeshFunctionObject `类和 `logFiles`类，
它计算特定面区内速度的时间积分，并把结果写入到一个文件中。需要注意的有：
- 构造函数 
- `writeFileHeader`  
- `createFileNames()`
- `write()`

另外，后处理程序是作为库被编译的，并被链接到求解器。

测试案例和tutorial 8 一样，但是这里使用了均匀速度分布的入口边界条件，并且不完全收敛时就终止了运算。
值得注意的是，这里面区的概念（`topoSet`生成）可以在paraview中使用filter “Extract block”来观察，
面区内流量的信息在`postProcessing`目录下生成，可以使用gnuplot进行绘制

---------

#### Tutorial 10 - 输运方程

介绍求解一个简单的标量输运方程背后的原理

对输运问题的描述：又叫做对流-扩散方程，这里建立的求解器忽略了源项，并且求解的是稳态解。$\beta$在固定的速度场中进行扩散，拥有固定扩散率
$\gamma$，这个求解器和内置求解器`scalarTransportFoam`类似，不过这里求解的是稳态问题。需要注意的关键点有:
- 标量输运方程背后的语法
- OpenFOAM如何将这些语法转变成特定的操作，并和`fvSchemes`和`fvSolution`里的条目联系起来的
- 包含`0/beta`中定义的边界条件
- 方程的量纲和OpenFOAM是如何定义它们的

算例是一个简单的二维方形域，底部和左边是固定值边界，输运在这个域内发生，速度方向是沿着左下到右上的对角线的。为了看到$\beta$的输运过程，

推荐阅读:

\- Wikipedia is always a good start: 

  https://en.wikipedia.org/wiki/Convection%E2%80%93diffusion_equation

\- Very brief description of the physical and mathematical concepts behind

  the scalar transport equation by CFD-online: 

  https://www.cfd-online.com/Wiki/Generic_scalar_transport_equation

\- Chapters 3, 4 and especially 5 in "Numerical Methods in Heat, Mass,

  and Momentum Transfer" by Murthy, J. Y. 2002: 

  https://engineering.purdue.edu/~scalo/menu/teaching/me608/ME608_Notes_Murthy.pdf

<img src="OFtutorial10_transportEquation/testCase/T=0.2.png?raw=true" alt="Alt text" title="Tutorial 10 - result of 2D convection-diffusion with inlets at left and bottom edges" style="zoom:50%;" />
<img src="OFtutorial10_transportEquation/testCase/T=1.0.png?raw=true" alt="Alt text" title="Tutorial 10 - result of 2D convection-diffusion with inlets at left and bottom edges" style="zoom:50%;" />

----

#### Tutorial 11 - 搭建网格

展示如何生成点云、面、patch和不同的cell类型，并最终搭建出网格组合体。可以利用Matlab和`meshPoints.geo`画出点云，理解网格是如何一步步构建的。


<img src="OFtutorial11_modifyingTheMesh/geometric.png?raw=true" alt="Alt text" title="Tutorial 11 - point clouds" style="zoom: 65%;" /><img src="OFtutorial11_modifyingTheMesh/testCase/mesh.png?raw=true" alt="Alt text" title="Tutorial 11 - geometric" style="zoom:33%;" />