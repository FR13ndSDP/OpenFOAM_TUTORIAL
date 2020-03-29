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

// argc为参数个数 argv为传入参数内容
int main(int argc, char* argv[])
{
    // ===
    // 写下本程序的帮助，使用ofTutorial2 -help查看
    argList::addNote
    (
        "WRITTEN BY SOMEONE.\n"
        "\n"
        "输入变量:\n"
        "----------------\n"
        "  someWord - 一串字符\n"
        "  someScalar - 一个标量\n"
    );

    // 准备变量列表
    argList::noParallel();
    argList::validArgs.append("someWord");
    argList::validArgs.append("someScalar");

    // 准备可选变量（不是必须要传入的参数）
    argList::addOption // 可选字符串变量
    (
        "dict",//检索关键词
        "word",//类型
        "某个字典的路径"//说明
    );

    argList::addBoolOption // 可选boolean变量，当传入someSwitch时值为1,否则为0
    (
        "someSwitch",
        "0或1"
    );

    argList::addOption // 可选整数变量
    (
        "someInt",
        "label",
        "某个整数"
    );

    // ===
    // create argument list
    // This is normally defined inside setRootCase.H
    // #include "setRootCase.H"
    // args是装载参数的容器
    Foam::argList args(argc, argv);
    // 在root路径下和不存在算例文件，如果是就抛出异常
    if (!args.checkRootCase())
    {
        Foam::FatalError.exit();
    }

    // ===
    // 读取第一个参数someWord
    const word someWord = args[1];
    // NOTE: 转换字符串类型到标量类型的内建方法
    // 读入someScalar参数时读到的实际是字符串
    const scalar someScalar = args.get<scalar>(2);

    Info << "得到字符串 " << someWord << " 和标量 " << someScalar << endl;

    // ===
    // 读可选变量
    // 设置默认目录
    fileName dictPath("./system/defaultDict");

    // 当传入参数中有dict时执行：
    if (args.found("dict"))
    {
        args.readIfPresent("dict", dictPath);
        Info << "得到一个字典的路径" << endl;
    }
    Info << "字典路径： " << dictPath << endl;

    // 读boolean类型
    const bool someConstBool = args.found("someSwitch");
    Info << "Boolean设置成 ：" << someConstBool << endl;

    // 读整形
    label someInt(0);
    args.readIfPresent("someInt", someInt);
    Info << "某个整数值： " << someInt << endl;
    
    Info<< "End\n" << endl;
    //如果你使用Allrun，注意看里面都写了什么！
    return 0;
}


// ************************************************************************* //