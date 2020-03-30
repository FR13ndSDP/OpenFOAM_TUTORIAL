//Include基本头文件-fvCFD.H,从而使用最基本的类fvMesh,labels等
#include "fvCFD.H"

class customClass // 类声明
{
private:
    label myInt_;

public:
    customClass();
    ~customClass();

    // 关键字inline指定函数为内联函数，它避免了链接带来的开销从而使程序更快
    // .get()是静态的，不能改变成员myInt_
    inline label get() const {return myInt_;}; // 输出
    inline void set(label newInt) {myInt_=newInt;}; // 读入参数并设定

    label basicFunction() const;

    // 传入mesh的引用在内存上更高效
    void meshOpFunction(fvMesh& mesh);
};
