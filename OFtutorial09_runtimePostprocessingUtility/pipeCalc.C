#include "pipeCalc.H"
#include "addToRunTimeSelectionTable.H"
// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
    defineTypeNameAndDebug(pipeCalc, 0);
    addToRunTimeSelectionTable(functionObject, pipeCalc, dictionary);
} // End namespace functionObjects
} // End namespace Foam

// * * * * * * * * * * * * * * Protected members * * * * * * * * * * * * * * //
//  === 向log.run中写内容需要的函数 ===

// NOTE: 这个函数从controlDict读取type，最终被logFiles写入到log.run中
Foam::wordList Foam::functionObjects::pipeCalc::createFileNames
(
    const dictionary& dict
) const
{
    DynamicList<word> names(1);

    const word objectType = dict.lookup("type"); // pipeCalc

    names.append(objectType);

    return names;
}

// NOTE: 此方法首先在 logFiles.H 中被声明， 当基类的write()函数被调用时自动调用该函数
// 目的是在输出数据文件（/postProcessing/runTime/*.dat）中加header
void Foam::functionObjects::pipeCalc::writeFileHeader(const label i)
{
    // 通过头文件中定义的枚举类型到正确的文件来进行输出
    switch (fileID(i))
    {
        case MAIN_FILE: // /postProcessing/0
        {
            writeHeader(file(i), "通过面区的流率");
            writeHeaderValue(file(i), "面区名", faceZoneName_);
            writeCommented(file(i), "Time [s] | Flow rate [m3s-1]");
            file() << endl;
            break; // 跳出switch结构
        }
        default:
        {
            FatalErrorInFunction
                << "Unhandled file index: " << i
                << abort(FatalError);
        }
    }
}

// * * * * * * * * * * * * * * * * 构造函数  * * * * * * * * * * * * * * * //

Foam::functionObjects::pipeCalc::pipeCalc
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    // NOTE: 调用基类构造函数
    fvMeshFunctionObject(name, runTime, dict),
    logFiles(obr_, name),

    name_(name),
    active_(true),
    UName_("U"),
    // NOTE: 读取到面区中每个面的下标
    faceZoneName_(dict.lookup("faceZoneName")),
    faceZoneLabel_( mesh_.faceZones().findZoneID(faceZoneName_) ),
    faces_( mesh_.faceZones()[faceZoneLabel_] )
{
    // logFiles方法创建输出流
    resetNames(createFileNames(dict));

    if (active_)
    {
        // 进行另外一个初始化，如果必要的话

        // NOTE: type() 返回头文件中TypeName()定义的名称， name_从字典中读取
        // Finished initialising pipeCalcu: pipeCalculator
        Info << "Finished initialising " << type() << ": " << name_ << nl << endl;
    }
}

// * * * * * * * * * * * * * * * * 析构函数 * * * * * * * * * * * * * * * * //

Foam::functionObjects::pipeCalc::~pipeCalc()
{}

// * * * * * * * * * * * * * * * * 成员函数 * * * * * * * * * * * * * * * * //

bool Foam::functionObjects::pipeCalc::execute()
{
    if (active_)
    {
        // 此函数在写入结果之前被调用，本应该放入一些东西来满足其他的函数的依赖（比如场计算）
    }
    return true;
}

bool Foam::functionObjects::pipeCalc::end()
{
    if (active_)
    {
        execute();
    }
    return true;
}

void Foam::functionObjects::pipeCalc::timeSet()
{}

// 计算通过面区流量并写入文件
bool Foam::functionObjects::pipeCalc::write()
{
    if (active_)
    {
        // NOTE: 这是成员函数的主要部分，在这里真正实现实际的功能

        // 取得速度场的引用
        // obr_ 引用区域的注册对象
        const volVectorField& U = obr_.lookupObject<volVectorField>(UName_);

        // 面上插值
		surfaceVectorField Uface = fvc::interpolate(U);

        // 遍历每个面区，计算流过面区法向的流量
        // 假设面区中所有面都不在处理器边界，如果出现了这种情况，会出现数组越界
        // A check could be made by using:
        // if ((faces_[faceI] > mesh_.owner().size()) || (mesh_.owner().size() == 0))
        // and the boundary values could be used instead. This is not done here
        // for simplicity.
        scalar flowRate(0.);

        forAll(faces_, faceI)
            // Q = \mathbf{U} \cdot \mathbf{\hat{n}} A
            flowRate += Uface[faces_[faceI]] & mesh_.Sf()[faces_[faceI]];

        // reduce for parallel running
        reduce(flowRate, sumOp<scalar>());

        // Total flow rate 0.00108663 through 1400 faces
        Info << "Total flow rate " << flowRate << " through "
             << returnReduce(faces_.size(), sumOp<label>()) << " faces" << nl << endl;

        // 输出到文件 - 仅在主进程输出以避免文件同时被多个核心写入
        if (Pstream::master())
        {
            // 调用基类的函数，将会检查log文件是否存在并创建它。同时调用派生类中的
            // .writeFileHeader()
            logFiles::write();

            // 将此条目添加到后处理文件中
            file(MAIN_FILE) << obr_.time().value() << tab << flowRate << endl;
        }
    }
    return true;
}

// ************************************************************************* //
