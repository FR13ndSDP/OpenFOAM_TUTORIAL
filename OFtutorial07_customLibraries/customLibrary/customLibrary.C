#include "customLibrary.H"

scalar computeR(const fvMesh& mesh, volScalarField& r, dimensionedVector x0)
{
    r = mag(mesh.C()-x0);
    return returnReduce(max(r).value(), maxOp<scalar>()); // maxOp<scalar>用来转换成无量纲量
}

void computeU(const fvMesh& mesh, volVectorField& U, word pName)
{
    // 这种写法允许在只知道场的名字情况下，在mesh类中找到场对象
    const volScalarField& pField = mesh.lookupObject<volScalarField>(pName);

    // 跟之前一样
    U = fvc::grad(pField)*dimensionedScalar("tmp", dimTime, 1.);
}
