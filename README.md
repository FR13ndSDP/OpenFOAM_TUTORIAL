## OpenFOAM® Tutorials

![license](https://img.shields.io/badge/license-GPL-orange)&ensp; ![release](https://img.shields.io/badge/release-ver1.0-brightgreen) 



#### 介绍

Introduces basic C++ concepts to beginner users of the OpenFOAM open-source CFD libraries.

Forked from **UnnamedMoose/BasicOpenFOAMProgrammingTutorials** 

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

基本场操作，包括速度、压力场的建立和求解

---------

#### Tutorial 5 - Basic parallel computing

关于并行计算的机理与如何进行并行信息的交互

---------

#### Tutorial6 - Custom Classes

自定义类、派生类的声明和定义，其中定义了一个继承自IOdictionary的类，用于读取myTransportProperties

---------

#### Tutorial 7 - Custom libraries

展示了如何自定义一个动态链接库，将tutorial4和5中的函数写在了库中

---------

#### Tutorial 8 - Custom boundary condition

展示如何定制一个边界条件。

我们没有建立一个工程，而是仅仅实现了一个库来完成。它定义了一个圆管的入口条件，其中的入口边界层厚度是可以预先指定的。

此边界条件是固定值边界（fixedValue）边界的派生类，向其中加入了几个控制量，得以定制入口参数。其中需要注意的要点用NOTE：
指明。需要注意的实现方法有：两个构造函数，默认的和从字符串构造边界条件的，以及.updateCoeffs()函数

测试算例是一根直管，使用基本的simpleFoam来解算，需要注意的是0.org/U中边界条件的定义和system/controlDict中自定义库的合并。
这个模拟是粗网格上的3D RANS，所以在低配机器上可能要花费几分钟的时间。边界条件带来的的影响可以通过绘制x方向的速度来进行观察

---------

#### Tutorial 9 - Runtime post processing utility



Discusses the implementation of a runtime post-processing utility which
computes the flow rate through a face zone defined in the mesh using the
topoSet utility.

The utility is implemented as a runtime postprocessing object derived from
the built-in fvMeshFunctionObject and logFiles classes. It integrates the normal
velocity through a specified face zone at each required time step and writes the
result to a file, as well as prints in on the screen. The key methods to
pay attention to are 1) the constructor 2) writeFileHeader(), 3) createFileNames(),
and 4) write(), which implements the actual maths behind the functionality.
Key elements of the code are highlighted with the keyword NOTE:. It is
important to note that the utility gets compiled as a library, which then
gets linked to the main solver, following the OpenFOAM runtime utility
convention.

The test case is the same pipe as in Tutorial 8, except it uses a uniform
inflow BC and is not run until full convergence. It is worth to note
the definition of the faceZone of interest in system/topoSet. This may be
visualised by selecting "Include zones" in paraview and applying the "Extract
block" filter. As the simpleFoam solver is run, the output file gets created
by the utility in the postProcessing directory.



---------

#### Tutorial 10 - Transport equation



Introduces the concepts behind solving a simple scalar transport equation.



The solver sets up the transport problem by importing a fixed velocity field
from the last time step and solving the transport of a scalar, beta, in the
presence of the velocity, beta being also subject to diffusion characterised
by a fixed proportionality constant, gamma. The solver is conceptually similar
to the built-in scalarTransportFoam, except it solves a steady-state problem.
Key things to note are 1) the syntax behind the scalar transport equation
2) how OpenFOAM translates the syntax into specific operations and associates
them with entries in system/fvSolution and system/fvSchemes dictionaries
3) inclusion of the boundary condition definitions in 0/beta into the equation
4) units of the equations being solved and how OpenFOAM handles them.

The test case is a simple 2D square domain with fixed scalar inlets at the bottom
and the left-hand side. Transport takes place in the presence of a velocity
field convecting away from the beta inlets. Once the case is run, it is best
to visualise the initial conditions in the "beta" field and the solution to the
transport equation saved as the "result" field.

Recommended reading:

\- Wikipedia is always a good start: 

  https://en.wikipedia.org/wiki/Convection%E2%80%93diffusion_equation

\- Very brief description of the physical and mathematical concepts behind

  the scalar transport equation by CFD-online: 

  https://www.cfd-online.com/Wiki/Generic_scalar_transport_equation

\- Chapters 3, 4 and especially 5 in "Numerical Methods in Heat, Mass,

  and Momentum Transfer" by Murthy, J. Y. 2002: 

  https://engineering.purdue.edu/~scalo/menu/teaching/me608/ME608_Notes_Murthy.pdf



![Alt text](OFtutorial10_transportEquation/testCase/2DconvectionDiffusion.png?raw=true "Tutorial 10 - result of 2D convection-diffusion with inlets at left and bottom edges")



---------

#### Tutorial 11 - Modifying the mesh



Demonstrates how to use points to generate different cell types, patches,
and export the finished grid to an OpenFOAM case.

Also recommended to view the 'meshPoints.pdf' or Gmsh files to get a better
idea of how the mesh is actually constructed from points.

![Alt text](OFtutorial11_modifyingTheMesh/testCase/cellTypes.png?raw=true "Tutorial 11 - different cell topologies")



---------

#### Tutorial 12 - Adding a custom momentum source



Shows a modified version of the actuatorDisk momentum source which does not use
a cellSet in order to mark cells for applying the source. Instead, it identifies
the cells inside of the constructor which allows easier adjustment of the disk
parameters and could be developed further to include a dynamic variant. Main
part of the implementation is located in "customActuationDiskSourceTemplates.C"
and the cell selection algorithm is implemented in the class constructor inside
"customActuationDiskSource.C". Key takeaways from the tutorial are how a fvOption
object is structured and how it may be modified to suit ones needs. It is a bit
more applied than the previous ones but hopefully will be useful to at least
a few people.

![Alt text](OFtutorial12_momentumSource/testCase/Umagnitude.png?raw=true "Tutorial 12 - velocity affected by a momentum source")