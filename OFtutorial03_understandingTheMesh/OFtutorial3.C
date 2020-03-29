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
    #include "setRootCase.H"

    // 分别创建time(实例称为runTime)和fvMesh(实例称为mesh)类
    #include "createTime.H"
    #include "createMesh.H"

	// 下面几行中的 .timeName(), .C() and .Cf() 都是对象的[方法].
	// mesh.C() 和 .Cf() 返回描述cell和内部面中心的向量场
	// 使用mesh.C().size()方法获取网格大小 
	Info << "你好，最新的时间文件夹是 " << runTime.timeName() << nl
		 << "网格有 " << mesh.C().size() << " 个cell和 " << mesh.Cf().size()
         << " 个内部面. Wubalubadubdub!" << nl << endl;

    // 通过标准的c++循环来遍历cell是可行的（尽管并不推荐）
    for (label cellI = 0; cellI < mesh.C().size(); cellI++)
        if (cellI%20 == 0) // 只展示每20个cell的信息以防止塞满屏幕
            Info << "Cell " << cellI << " 中心在 " << mesh.C()[cellI] << endl;
    Info << endl;

    // 每个cell都是由面(faces)组成的，这些面要么是内部面，要么组成边界(boundry)，或者用
    // OF的叫法，称为patch。内部面都具有主cell(owner)和临cell(neighbour)
    for (label faceI = 0; faceI < mesh.owner().size(); faceI++)
        if (faceI%40 == 0)
            Info << "内部面 " << faceI << " 中心在 " << mesh.Cf()[faceI]
                 << " with owner cell " << mesh.owner()[faceI]
                 << " and neighbour " << mesh.neighbour()[faceI] << endl;
    Info << endl;

    // 现在使用boundaryMesh对象来获取边界条件
    // 实际上，所有的边界面也都包含在constant/polyMesh/faces 中，
    // 但是在那个文件中，所有的内部面首先被定义
    // 另外，constant/polyMesh/boundary 文件定义了边界定义的起始面(faceI)的索引
	// OpenFOAM提供宏定义forAll执行在场或列表中的遍历，为你节省时间
    forAll(mesh.boundaryMesh(), patchI)
        Info << "Patch " << patchI << ": " << mesh.boundary()[patchI].name() << " with "
             << mesh.boundary()[patchI].Cf().size() << " faces. Starts at total face "
             << mesh.boundary()[patchI].start() << endl;
    Info << endl;

    // 使用以下的方式获取边界的临接面
    // 此外，一个面的法向量和它的面积也是很有用的
    label patchFaceI(0);
    forAll(mesh.boundaryMesh(), patchI)
        Info << "Patch " << patchI << " has its face " << patchFaceI << " adjacent to cell "
             << mesh.boundary()[patchI].patch().faceCells()[patchFaceI]
             << ". It has normal vector " << mesh.boundary()[patchI].Sf()[patchFaceI] // 法向量，模为面积
             << " and surface area " << mag(mesh.boundary()[patchI].Sf()[patchFaceI]) // mag计算模
             << endl;
    Info << endl;

    // 对于内部面，方法.Sf()可以用来直接被mesh对象调用
    // 此外还有一个捷径方法.magSf()来返回一个标量，代表表面面积
    // 内部面的法向量由owner指向neighbour，并且owner的下标比neighbour小
    // 对边界面来说，法向量总是指向计算域之外，他有一个虚拟的neighbour

    // ==
    // 点是如何组成面的
    // 先定义一些引用，他们是consr类型，因为我们没打算修改
    // 这些list包括边界条件的一些信息，可以使用下面的代码进行检测是否为边界
    const faceList& fcs = mesh.faces();
    const List<point>& pts = mesh.points();
    const List<point>& cents = mesh.faceCentres();

    forAll(fcs,faceI) // for all faceI in fcs
        if (faceI%80==0) // 只展示下标是80的整数倍的face
        {
            if (faceI<mesh.Cf().size()) // 判断为internal mesh
                Info << "内部面 ";
            else
            {
                forAll(mesh.boundary(),patchI) // for all patches in boundary
                    // 逐一检测faceI是否在patchI中，
                    if ((mesh.boundary()[patchI].start()<= faceI) &&
                        (faceI < mesh.boundary()[patchI].start()+mesh.boundary()[patchI].Cf().size())) // 用来检测是否为边界
                    {
                        Info << "非内部面在patch " << patchI << ", faceI ";
                        break; // 退出forall
                    }
            }

            Info << faceI << " 中心坐标在 " << cents[faceI]
                 << " 有 " << fcs[faceI].size() << " 顶点:"; 
            forAll(fcs[faceI],vertexI)
                // Note how fcs[faceI] holds the indices of points whose coordinates
                // are stored in the pts list.
                Info << " " << pts[fcs[faceI][vertexI]];
            Info << endl;
        }
    Info << endl;

    // 原始的cavity算例中，前后面定义为empty，这是一个特殊的边界条件。他的.CF()场大小为零
    label patchID(0);
    const polyPatch& pp = mesh.boundaryMesh()[patchID]; // pp是boundary 0的引用
    if (isA<emptyPolyPatch>(pp)) // pp不是emmpty（是moving wall），因此后面不会打印
    {
        // patch patchID is of type "empty".
        Info << "You will not see this." << endl;
    }

    // 通过patch的名字获取其信息
    word patchName("movingWall");// 要找的patch: movingWall
    patchID = mesh.boundaryMesh().findPatchID(patchName);
    Info << "取回patch " << patchName << " 在下标 " << patchID << " ，只是使用它的名字" << nl << endl;

    Info<< "结束\n" << endl;

    return 0;
}


// ************************************************************************* //
