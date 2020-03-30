#include "fvCFD.H"

// 自定义的类在.H中声明并在.C文件中定义 现在我们包含这些头文件
// NOTE: 查看Make/files的变化 它确保我们加入的代码能在主程序之前被编译
#include "customClass.H"
#include "derivedClass.H"

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

    // Create a custom class instance
    // 创建自定义类实例
    customClass customInstance;
    Info << "默认值 " << customInstance.get() << endl;

    // Set a new value
    // 赋给它新值
    customInstance.set(10);
    Info << "新的值 " << customInstance.get() << endl;

    // Call a basic function
    // 调用函数
    customInstance.basicFunction();

    // 传给他mesh参数，具体看customClass的定义
    customInstance.meshOpFunction(mesh);
    Info << "现在又有另一个值 " << customInstance.get() << endl;

    // Now, create an instance of a derived class which inherits from an IOdictionary object
    // 现在建一个新的实例 它继承自IOdictionary对象的派生类
    myDict myTransportProperties
    (
        IOobject
        (
            "transportProperties",
            runTime.constant(),
            mesh,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    );

    // Create a viscosity scalar using our new class
    // 使用我们的新类创建一个标量，表示粘度
    dimensionedScalar nu 
    (
        "nu",
        dimViscosity,
        myTransportProperties
    );
    Info << "创建了一个粘度标量: " << nu << endl;

    // List the contents of the dictionary using the derived class method
    // implemented specifically for this purpose
    // 使用派生类的方法列出字典中的内容
    myTransportProperties.printTokensInTheDict();

    Info<< "End\n" << endl;

    return 0;
}

// ************************************************************************* //
