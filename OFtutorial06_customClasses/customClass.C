#include "customClass.H"

customClass::customClass() // 构造函数
{
    myInt_= 0;
}

customClass::~customClass() // 析构函数
{}

label customClass::basicFunction() const // 一个简单的静态函数，返回label类型
{
    Info << "调用 customClass::basicFunction()" << endl;
    return myInt_*2;
}

void customClass::meshOpFunction(fvMesh& mesh) // 传入mesh参数，类型是fvMesh类，成员有polyMesh,fvScheme等
{
    Info << "自定义类得到的网格有 " << mesh.C().size() << " 个cell" << endl;
    myInt_ = mesh.C().size();
}
