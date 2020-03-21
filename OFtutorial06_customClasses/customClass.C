#include "customClass.H"

customClass::customClass()
{
    myInt_= 0;
}

customClass::~customClass()
{}

label customClass::basicFunction() const
{
    Info << "调用 customClass::basicFunction()" << endl;
    return myInt_*2;
}

void customClass::meshOpFunction(fvMesh& mesh)
{
    Info << "自定义类得到的网格有 " << mesh.C().size() << " 个cell" << endl;
    myInt_ = mesh.C().size();
}
