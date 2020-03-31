#include "fvCFD.H"

// 包含自定义库的头文件
// 库可以实现几乎所有的东西：从简单的函数到几个不同的类
// 使用库的主要好处就是它允许同一段代码被不同的程序重复利用
// NOTE:检查 Make.options 的变化，确保库可以被正确链接
#include "customLibrary.H"

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"
    #include "createFields.H"

    const dimensionedVector originVector("x0", dimLength, vector(0.05, 0.05, 0.005));
    scalar f (1.);
    // NOTE： 初始化距离场，使用零值和长度量纲
    volScalarField r
    (
        IOobject
        (
            "r",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh,
        dimensionedScalar("r0", dimLength, 0.)
    );
    // NOTE: 使用库函数实现 r 和 rFarcell计算
    const scalar rFarCell = computeR(mesh, r, originVector);

    Info<< "\n开始时间循环\n" << endl;

    while (runTime.loop())
    {
        Info<< "时间 = " << runTime.timeName() << nl << endl;

        p = Foam::sin(2.*constant::mathematical::pi*f*runTime.time().value())
            / (r/rFarCell + dimensionedScalar("small", dimLength, 1e-12))
            * dimensionedScalar("tmp", dimensionSet(0, 3, -2, 0, 0), 1.);
        p.correctBoundaryConditions();

        // NOTE: 调用库函数计算U
        computeU(mesh, U, "p");

        runTime.write();
    }

    Info<< "End\n" << endl;

    return 0;
}

// ************************************************************************* //
