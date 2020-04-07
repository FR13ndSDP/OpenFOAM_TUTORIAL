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

#include "cellModeller.H"

int main(int argc, char* argv[])
{
    #include "setRootCase.H"

    Info << "创建时间\n" << endl;
    Time runTime
    (
        Time::controlDictName, // 'controlDict'
        args.rootPath(), // OFtutorial11_modifyingTheMesh
        args.caseName() // testCase
    );
    // 禁用后处理函数
    runTime.functionObjects().off();
    
    // 创建网格类型
    const cellModel& hex = *(cellModeller::lookup("hex")); // 六面体
    const cellModel& prism = *(cellModeller::lookup("prism")); // 五面体（hex沿体对角线切一半）
    const cellModel& pyr = *(cellModeller::lookup("pyr")); // 五面体（金字塔）
    const cellModel& tet = *(cellModeller::lookup("tet")); // 四面体
    Info << "创建了各种网格类型" << endl;

    // 创建点云
    pointField points(17);
    points[ 0] = vector(0.0, 0, 0);
    points[ 1] = vector(0.5, 0, 0);
    points[ 2] = vector(1.0, 0, 0);
    points[ 3] = vector(0.0, 1, 0);
    points[ 4] = vector(0.5, 1, 0);
    points[ 5] = vector(1.0, 1, 0);
    points[ 6] = vector(0.0, 0, 0.1);
    points[ 7] = vector(0.5, 0, 0.1);
    points[ 8] = vector(1.0, 0, 0.1);
    points[ 9] = vector(0.0, 1, 0.1);
    points[10] = vector(0.5, 1, 0.1);
    points[11] = vector(1.0, 1, 0.1);
    points[12] = vector(0.7, 0.5, -0.5); // 金字塔尖
    points[13] = vector(0.7, 1.0, -0.5);
    points[14] = vector(0.7, 1.2, -0.5);
    points[15] = vector(0.5, 1.2, 0.0);
    points[16] = vector(1.0, 1.2, 0.0);
    Info << "创建了点云" << endl;

    // 创建一个list的list,包含所有cell的顶点信息
    cellShapeList cells;

    labelList cellPoints(8);
    // 第一个六面体
    cellPoints[0] = 0;
    cellPoints[1] = 3;
    cellPoints[2] = 9;
    cellPoints[3] = 6;
    cellPoints[4] = 1;
    cellPoints[5] = 4;
    cellPoints[6] = 10;
    cellPoints[7] = 7;
    cells.append(cellShape(hex,cellPoints));

    // 第二个六面体
    cellPoints[0] = 1;
    cellPoints[1] = 4;
    cellPoints[2] = 10;
    cellPoints[3] = 7;
    cellPoints[4] = 2;
    cellPoints[5] = 5;
    cellPoints[6] = 11;
    cellPoints[7] = 8;
    cells.append(cellShape(hex,cellPoints));

    // 金字塔
    cellPoints.resize(5);
    cellPoints[0] = 4;
    cellPoints[1] = 5;
    cellPoints[2] = 2;
    cellPoints[3] = 1;
    cellPoints[4] = 12;
    cells.append(cellShape(pyr,cellPoints));
    
    // 四面体
    cellPoints.resize(4);
    cellPoints[0] = 4;
    cellPoints[1] = 5;
    cellPoints[2] = 13;
    cellPoints[3] = 12;
    cells.append(cellShape(tet,cellPoints));

    // 半个hex
    cellPoints.resize(6);
    cellPoints[0] = 13;
    cellPoints[1] = 4;
    cellPoints[2] = 5;
    cellPoints[3] = 14;
    cellPoints[4] = 15;
    cellPoints[5] = 16;
    cells.append(cellShape(prism, cellPoints));

    Info << "创建了cell" << endl;

    // 和cell相似的方法创建patch
    faceListList patchFaces;

    // 创建一个面
    labelList patchPoints(4);
    patchPoints[0] = 3; // 从域外看顺时针方向
    patchPoints[1] = 9;
    patchPoints[2] = 6;
    patchPoints[3] = 0;
    patchFaces.append(faceList(1,face(patchPoints)));

    // 先组成面再组成patch
    // 即多个面组成patch
    faceList faces(4);
    patchPoints[0] = 14;
    patchPoints[1] = 15;
    patchPoints[2] = 4;
    patchPoints[3] = 13;
    faces[0] = face(patchPoints);
    patchPoints[0] = 15;
    patchPoints[1] = 16;
    patchPoints[2] = 5;
    patchPoints[3] = 4;
    faces[1] = face(patchPoints);
    patchPoints[0] = 16;
    patchPoints[1] = 14;
    patchPoints[2] = 13;
    patchPoints[3] = 5;
    faces[2] = face(patchPoints);
    patchPoints.resize(3);
    patchPoints[0] = 15;
    patchPoints[1] = 14;
    patchPoints[2] = 16;
    faces[3] = face(patchPoints);
    patchFaces.append(faces);

    // tet的外部面
    faces.resize(2);
    patchPoints[0] = 4;
    patchPoints[1] = 12;
    patchPoints[2] = 13;
    faces[0] = face(patchPoints);
    patchPoints[0] = 13;
    patchPoints[1] = 12;
    patchPoints[2] = 5;
    faces[1] = face(patchPoints);
    patchFaces.append(faces);

    // 金字塔外部面
    faces.resize(3);
    patchPoints[0] = 4;
    patchPoints[1] = 1;
    patchPoints[2] = 12;
    faces[0] = face(patchPoints);
    patchPoints[0] = 12;
    patchPoints[1] = 1;
    patchPoints[2] = 2;
    faces[1] = face(patchPoints);
    patchPoints[0] = 12;
    patchPoints[1] = 2;
    patchPoints[2] = 5;
    faces[2] = face(patchPoints);
    patchFaces.append(faces);

    Info << "创建了patch" << endl;

    // patch的名称
    wordList boundaryPatchNames;
    boundaryPatchNames.append("hexSide0");
    boundaryPatchNames.append("prismFaces");
    boundaryPatchNames.append("tetFaces");
    boundaryPatchNames.append("pyramidFaces");

    // patch类型 - 类型和物理类型
    wordList boundaryPatchTypes(patchFaces.size());
    boundaryPatchTypes[0] = "symmetryPlane";
    boundaryPatchTypes[1] = "patch";
    boundaryPatchTypes[2] = "wall";
    boundaryPatchTypes[3] = "symmetry";

    wordList boundaryPatchPhysicalTypes(patchFaces.size());
    boundaryPatchPhysicalTypes[0] = "symmetryPlane";
    boundaryPatchPhysicalTypes[1] = "patch";
    boundaryPatchPhysicalTypes[2] = "wall";
    boundaryPatchPhysicalTypes[3] = "symmetry";

    // 网格区域的名字 - 使用OF默认
    word regionName = polyMesh::defaultRegion;

    // 未定义区域默认值
    word defaultFacesName = "defaultFaces";
    word defaultFacesType = emptyPolyPatch::typeName;

    Info << "创建了物理类型和默认类型" << endl;

    // 创建网格
    polyMesh mesh
    (
        IOobject
        (
            regionName,
            runTime.constant(),
            runTime
        ),
        clone(points),
        cells,
        patchFaces,
        boundaryPatchNames,
        boundaryPatchTypes,
        defaultFacesName,
        defaultFacesType,
        boundaryPatchPhysicalTypes
    );
    Info << "创建了网格" << endl;

    // 写入网格
    Info << nl << "Writing extruded mesh to time = " << runTime.timeName() << nl << endl;
    mesh.write();

    Info<< "End\n" << endl;

    return 0;

}