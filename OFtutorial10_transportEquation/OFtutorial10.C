#include "fvCFD.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char* argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

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

    solve
    (
        fvm::div(phi, beta) - fvm::laplacian(gamma, beta)
    );

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

    Info << "end" << nl << endl;
    return 0;
}
