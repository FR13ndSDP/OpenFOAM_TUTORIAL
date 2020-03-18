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
//最基本的openfoam头文件之一 包含以使用fvMesh,label等类
//fv: finite volume的缩写 使用有限体积法是openfoam的特性
#include "fvCFD.H"

int main(int argc, char *argv[])
{
    // 检查基本的文档结构确定是否存在controlDict等
    // 也用来解析命令行参数和选项
	// 通过执行在 $FOAM_SRC/OpenFOAM/include 中的外部代码来实现
	// 具体实现代码:
    /*Foam::argList args(argc, argv); - deciphers the arguments passed to the program
    if (!args.checkRootCase()) - 确认文档结构
    {
        Foam::FatalError.exit();
    }*/
    #include "setRootCase.H"

	// OpenFOAM的输出与C++的std::cout, std::nl and std::endl类似
	// 被Foam::Info, Foam::nl, and Foam::endl代替.
	Info << "Hello there, I'm an OpenFOAM program!" << nl
		 << "你不需要生成网格." << nl
		 << tab << "又是我,只不过在新的一行里,我们继续吧!." << nl << endl;

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
