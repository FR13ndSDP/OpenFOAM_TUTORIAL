#include "derivedClass.H"

myDict::myDict(const IOobject& ioObj)
:
    // 初始化
    IOdictionary(ioObj)
{
// 什么都不做，因为我们没有任何自定义的场
}

myDict::~myDict()
{}

void myDict::printTokensInTheDict() const
{
    // retrieve the list of non-space characters in the file
    // 获取列表中的无空格分割词
    List<token> characters(this->tokens());

    // 创建一个输入输出流，它将掌握打印的信息
    // 注意使用std命名空间
    std::stringstream ss;
    ss << "Tokens in the file:";
    // 遍历标记(token)
    forAll(characters,i)
        // 如果entry是word，加入输出信息中
        if (characters[i].isWord())
            ss << "\n" << tab << characters[i].wordToken();

    // 打印信息-转换成C风格字符串，这样更美观
    Info << ss.str().c_str() << endl;
}
