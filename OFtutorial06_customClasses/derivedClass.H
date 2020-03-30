#include "fvCFD.H"

// use a standard C++ stringstream object
// 一个c++字符输入输出标准库
#include <sstream>

// 派生自IOdictionary
// public继承方式，这样拥有最高访问权限
class myDict : public IOdictionary
{
public:
    myDict(const IOobject& ioObj);
    ~myDict();

    // custom method which prints the tokens (variables) defined in the file
    void printTokensInTheDict() const;
};
