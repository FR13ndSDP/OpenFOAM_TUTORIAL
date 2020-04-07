#include "fvCFD.H"
#include "simpleControl.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char* argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

    simpleControl simple(mesh);

    #include "createFields.H"

    Info << "读取/计算面通量场 phi" << nl << endl;
    surfaceScalarField phi
    (
        IOobject
        (
            "phi",
            runTime.timeName(),
            mesh,
            IOobject::READ_IF_PRESENT,
            IOobject::AUTO_WRITE
        ),
        fvc::interpolate(U) & mesh.Sf() // m/s * m^2 == m^3/s 
    );
    // 忽略源项
    while(simple.loop(runTime))
    {
        Info << "Time=" << runTime.timeName() << nl << endl;
        solve
        (
            fvm::ddt(beta)+fvm::div(phi, beta) == fvm::laplacian(gamma, beta)
        );
        runTime.write();
    }
    // 当只需要稳态解时，去掉simple循环，将result以下面的方法输出
    /*
    volScalarField result
    (
        IOobject
        (
            "result",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        beta
    );
    result.write();
    */

    Info << "end" << nl << endl;
    return 0;
}
