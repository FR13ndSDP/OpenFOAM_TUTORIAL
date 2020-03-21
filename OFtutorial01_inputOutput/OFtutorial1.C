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

int main(int argc, char *argv[])
{
    // 算例初始化
    #include "setRootCase.H"

    // 这两个头文件创建时间(实例为runTime)和fvMesh(实例为mesh)
    #include "createTime.H"
    #include "createMesh.H"

    // ---
    // 创建自定义字典
    dictionary customDict;
    const word dictName("customProperties");

    // Create and input-output object - this holds the path to the dict and its name
    // 创建输入输出对象，其中包含了字典的名称以及路径
    IOobject dictIO
    (
        dictName, // 文件名，即""customProperties"
        mesh.time().constant(), // 文件路径
        mesh, // objectRegistry为mesh，这指定了对象所属的类
        IOobject::MUST_READ // 指定文件为必读
    );

    //检查字典是否存在且符合OF格式
    if (!dictIO.typeHeaderOk<dictionary>(true))
        FatalErrorIn(args.executable()) << "Cannot open specified refinement dictionary "
            << dictName << exit(FatalError);

    // 字典对象实例化
    customDict = IOdictionary(dictIO);

    // ---
    // 从字典中读取不同的信息

    //lookup并不需要知道我们要找的变量类型
    //这个操作从字典中读取 "someWord"的值并存放在word变量someWord中
    //word是String的派生类
    word someWord;
    customDict.lookup("someWord") >> someWord;

    // 这个方法需要让lookup知道变量类型，并且如果值没有找到，你可以指定一个默认值
    //此例中就指定了一个默认值1.0
    scalar someScalar( customDict.lookupOrDefault<scalar>("someScalar", 1.0) );

    // Switch类型使得boolean类型可以被从字典中读取，它支持yes/on/true/1 或 no/off/false/0
    bool someBool ( customDict.lookupOrDefault<Switch>("someBool",true) );

    // 同样方式来读取数组
    List<scalar> someList ( customDict.lookup("someList") );

    // 读取哈希表
    HashTable<vector,word> someHashTable ( customDict.lookup("someHashTable") );

    // 总结一下我们都读取了什么
    Info << nl << "Read the following:" << nl << nl
         << "someWord " << someWord << nl << nl
         << "someScalar " << someScalar << nl << nl
         << "someList " << someList << nl << nl
         << "someHashTable " << someHashTable << nl << nl
         << "someBool " << someBool << nl << nl
         << endl;

    // ---
    // 创建一个字典并写入
    // 指定输出字典的路径
    fileName outputDir = mesh.time().path()/"postProcessing";
    // 创建此文件夹
    mkDir(outputDir);

    //指向输出字典的文件指针
	autoPtr<OFstream> outputFilePtr;
    // 打开新创建的文件夹
    outputFilePtr.reset(new OFstream(outputDir/"customOutputFile.dat"));

    // 写入一些东西
    outputFilePtr() << "SDPTOP" << endl;
    outputFilePtr() << "0 1 2 3 4 5" << endl;

    // 在前面读取的哈希表中加入新元素并写入
    someHashTable.insert("newNEWKey", vector(1., 0., 0.));
    outputFilePtr() << someHashTable << endl;
    Info<<"写入成功，在postProcessing/customOutputFile.dat中查看\n"<<endl;
    Info<< "End\n" << endl;
    return 0;
}


// ************************************************************************* //
