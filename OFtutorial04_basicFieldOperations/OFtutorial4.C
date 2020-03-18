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

// This is a function declaration; this method will calculate some scalar value
// given the current time, location in space x, and a reference point x0. The
// function also accepts a scaling factor, scale.
// The actual implementation, or definition, is below.
//计算p,参数为参考点x0,位置x,时间t,尺度因子。
scalar calculatePressure(scalar t, vector x, vector x0, scalar scale);

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

	// This reads a dictionary file.
	Info << "Reading transportProperties\n" << endl;

	IOdictionary transportProperties//读取文件设置
	(
		IOobject
		(
		    "transportProperties", // name of the dictionary
		    runTime.constant(), // location in the case - this one is in constant 指定所要读取的位置
		    mesh, // needs the mesh object reference to do some voodoo - unimportant now
		    IOobject::MUST_READ_IF_MODIFIED, // the file will be re-read if it gets modified during time stepping
		    IOobject::NO_WRITE // read-only
		)
	);
	//开始读取
	// Create a scalar constant for kinematic viscosity by reading the value from the dictionary.
	dimensionedScalar nu//FIXME:这里有warnning
	(
		"nu", // name of the variable
		dimViscosity, // dimensions 定义为粘度量纲
		//内部定义的方式：
		// TIP: to check how this is defined, run:
		// grep -r dimViscosity $FOAM_SRC/OpenFOAM/
		// This returns:
		/*/opt/openfoam30/src/OpenFOAM/dimensionSet/dimensionSets.C:const dimensionSet dimViscosity(dimArea/dimTime);
		/opt/openfoam30/src/OpenFOAM/dimensionSet/dimensionSets.C:const dimensionSet dimDynamicViscosity(dimDensity*dimViscosity);
		/opt/openfoam30/src/OpenFOAM/dimensionSet/dimensionSets.H:extern const dimensionSet dimViscosity;*/
		// So, it becomes apparent we should check dimensionSets.C, which contain:
		/*const dimensionSet dimLength(0, 1, 0, 0, 0, 0, 0);
		const dimensionSet dimTime(0, 0, 1, 0, 0, 0, 0);
		const dimensionSet dimArea(sqr(dimLength));
		const dimensionSet dimViscosity(dimArea/dimTime);*/
		// This is what gets used here. But, an alternative would be to type in the units directly:
		// dimensionSet(0,2,-1,0,0,0,0),
		transportProperties.lookup("nu") // this takes the value from the dictionary and returns it, passing it to the object constructor as an argument 读取到nu=nu [0,2,-1,0,0,0,0] 0.01
	);
	
	// These read the fields p and U from the time folders, as specified in system/controlDict (i.e. latestTime, startTime, etc.)
	Info<< "Reading field p\n" << endl;
	volScalarField p // 注意p为标量场
	(
		IOobject
		(
		    "p", // 场的名字
		    runTime.timeName(), // name of the current time, i.e. the time folder to read from
		    mesh,
		    IOobject::MUST_READ, // always gets imported, will throw an error if the field is missing
		    IOobject::AUTO_WRITE // will get saved automatically when the controlDict parameters will request it
		),
		mesh // 初始化场为mesh的大小并将场置为0
	);

	Info<< "Reading field U\n" << endl;
	volVectorField U // 注意U为向量场
	(
		IOobject
		(
		    "U",
		    runTime.timeName(),
		    mesh,
		    IOobject::MUST_READ,
		    IOobject::AUTO_WRITE
		),
		mesh
	);

	// 定义一个不变的参考点
	const vector originVector(0.01,0.02,0.005);

	// 计算cell中心到参考点最大距离 用作计算压力的尺度因子
	// In Python, this is equivalent to:
	// np.sqrt(np.sum((x0-x)**2))
	// The .value() method is called to convert from a dimensionedScalar to a regular scalar.
	const dimensionedScalar rFarCell = max( // 找出最大距离
		// compute distance of each cell centre from x0; units of mesh.C() are those of length, as this field
		// describes position in the Cartesian reference frame.
		mag(dimensionedVector("x0",dimLength,originVector)-mesh.C())
		); 
	const scalar farCell=rFarCell.value();// 转换到无量纲数
	Info<<"dimensionedScalar=\n"<<farCell<<endl;
	Info<<"dim-less scalar=\n"<<farCell<<endl;

	// This part of the code performs time stepping for as long as is required by the simulation.
	Info<< "\nStarting time loop\n" << endl;

	// 这将会自动增加时间
    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

		// 循环所有cell计算压力
		for (label cellI=0; cellI<mesh.C().size(); cellI++)
		{
			// cellI describes a series of integers, each corresponding to an index of an individual cell in the grid.

			// Call the method and compute p.
			// Note how mesh.C() and p elements are accessed by using the [] operators, as in a regular C array.
			// .value() 转换时间到无量纲数
			p[cellI] = calculatePressure(runTime.time().value(), mesh.C()[cellI], originVector, farCell);

            // NOTE: it is also possbile to interact with boundary face values, but
            // this will be addressed in a separate tutorial.
		}

		// Calculate the gradient of p and substitute for U. Note that the units of grad(p) are not m/s,
		// so we multiply by a unit time variable to make them so. This is just for illustration purposes, of course.
		// The result will point either towards or away from the origin, depending on the sign of the
		// time varying "pressure".
		//用压力梯度来冒充速度
		//乘以单位时间来获取正确的量纲，不必要，只是为描述正确
		U = fvc::grad(p)*dimensionedScalar("tmp",dimTime,1.);//场操作fvc 比循环高效
		
		// If requested by controlDict, save the fields to files.
		runTime.write();
		// NOTE: a more appropriate way to calculate things in OpenFOAM is through performing
		// operations on field objects and not iterating cell-by-cell, where possible.
		// How to do this has been shown above, where farCell is being computed.
		// The iterative approach has been presented for completeness and to illustrate certain
		// basic features, but is, generally, discouraged, unless absolutely necessary.
	}

	Info << "Finished! Best to visualise the results by plotting p iso-contours with range (-10,10) and applying a glyph filter to the U field in Paraview." << endl;

    Info<< "End\n" << endl;

    return 0;
}

// definition of the custom function
scalar calculatePressure(scalar t, vector x, vector x0, scalar scale)
{
	// Calculates the distance between the base point and x, which is given by the magnitude (hence the name mag)
	// of the vector between x0 and x. The value is scaled by the passed factor, with the intention of making
	// it vary between 0 and 1.
	scalar r (mag(x-x0)/scale);

	// Calculate the inverse of r and apply a limiter to avoid dividing by zero.
	scalar rR (1./(r+1e-12));

	// definition of a frequency 频率
	scalar f (2.);

	// Return a sinusoidally varying pressure with maximum at x0.
	// Note how we call the OpenFOAM sin method by referring to the Foam namespace.
	// This is done to differentiate between the native C++ implementation of a method with the same name
	// and thus avoid an ambiguous expression.
	return Foam::sin(2.*Foam::constant::mathematical::pi*f*t)*rR;
}

// ************************************************************************* //
