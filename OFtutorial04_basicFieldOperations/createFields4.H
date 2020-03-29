Info << "读取粘度\n" << endl;
// 如果粘度在 /0下，（和p,u在同一目录下),使用下面的读取流程
volScalarField nu
(
    IOobject
    (
        "nu",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    mesh, // 初始化场为mesh的大小并将场置为0
    dimensionedScalar(dimViscosity, Zero)
);

// 如果在transportProperties中，使用下面的流程
if (!nu.headerOk())
{
    IOdictionary transportProperties //读取文件设置
    (
        IOobject
        (
            "transportProperties",           // 文件名
            runTime.constant(),              // 文件路径"./constant"
            mesh,                            // 注册对象
            IOobject::MUST_READ_IF_MODIFIED, // 如果运行时值改变了，就进行读取
            IOobject::NO_WRITE               // 只读
        )
    );
    nu = dimensionedScalar("nu", dimViscosity, transportProperties);
}

Info<< "读压力场\n" << endl;
volScalarField p // 注意p为标量场
(
	IOobject
	(
		"p", // 场的名字
        runTime.timeName(), 
        mesh,
        IOobject::MUST_READ, 
        IOobject::AUTO_WRITE 
	),
	mesh 
);

Info<< "读速度场\n" << endl;
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