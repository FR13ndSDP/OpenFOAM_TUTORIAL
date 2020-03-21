/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2015 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

// Include the code for the custom classes declared in .H and defined
// in .C files.
// NOTE: check how the Make/files changed to make sure the additional code gets
// compiled before the main utility.
// 自定义的类在.H中声明并在.C文件中定义 现在我们包含这些头文件
// NOTE: 查看Make/files的变化 它确保我们加入的代码能在主程序之前被编译
#include "customClass.H"
#include "derivedClass.H"

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

    // Create a custom class instance
    // 创建自定义类实例
    customClass customInstance;
    Info << "Default value " << customInstance.get() << endl;

    // Set a new value
    // 赋给它新值
    customInstance.set(10);
    Info << "New value " << customInstance.get() << endl;

    // Call a basic function
    // 调用函数
    customInstance.basicFunction();

    // Pass a reference to the mesh to the custom class and let it do its things
    // TODO:加入翻译
    customInstance.meshOpFunction(mesh);
    Info << "Yet another value " << customInstance.get() << endl;

    // Now, create an instance of a derived class which inherits from an IOdictionary object
    // 现在建一个新的实例 它是继承自IOdictionary对象的派生类
    myDict myTransportProperties
    (
        IOobject
        (
            "transportProperties",
            runTime.constant(),
            mesh,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    );

    // Create a viscosity scalar using our new class
    // 使用我们的新类创建一个标量，表示粘度
    dimensionedScalar nu
    (
        "nu",
        dimViscosity,
        myTransportProperties.lookup("nu")
    );
    Info << "Created a viscosity scalar: " << nu << endl;

    // List the contents of the dictionary using the derived class method
    // implemented specifically for this purpose
    // 使用派生类的方法列出字典中的内容
    myTransportProperties.printTokensInTheDict();

    Info<< "End\n" << endl;

    return 0;
}

// ************************************************************************* //
