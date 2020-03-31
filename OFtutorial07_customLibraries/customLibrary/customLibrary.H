#include "fvCFD.H"

// 这个函数只是简单地计算每个cell中心到参考点的距离。r 是一个引用，用来避免传入大量的值，提高IO效率
scalar computeR(const fvMesh& mesh, volScalarField& r, dimensionedVector x0);

// 计算速度场，pName是在mesh中要找的对象名，拥有默认值 word p
void computeU(const fvMesh& mesh, volVectorField& U, word pName = "p");
