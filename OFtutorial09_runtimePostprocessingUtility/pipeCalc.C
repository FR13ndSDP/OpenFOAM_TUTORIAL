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

// NOTE: this returns a list of file names which match indices of the enum
// defined in the header of this class. These names are used to create matching
// files by the logFiles object.
// NOTE: 返回文件名列表，这些文件名用来创建与 logFiles 对象相对应的文件
Foam::wordList Foam::functionObjects::pipeCalc::createFileNames
(
    const dictionary& dict
) const
{
    DynamicList<word> names(1);

    // use type of the utility as specified in the dict as the top-level dir name
    const word objectType = dict.lookup("type"); // pipeCalc

    // Name for file(MAIN_FILE=0)
    names.append(objectType);

    return names;
}

// NOTE: 此方法首先在 logFiles.H 中被声明， 当基类的write()函数被调用时自动调用该函数
// 目的是在输出数据文件中加header
void Foam::functionObjects::pipeCalc::writeFileHeader(const label i)
{
    // Find the correct file to write to from the enum defined in the header.
    // 通过头文件中定义的枚举类型到正确的文件来进行输出
    switch (fileID(i))
    {
        case MAIN_FILE: // 密码正确
        {
            writeHeader(file(i), "Flow rate through face zone");
            writeHeaderValue(file(i), "Face zone name", faceZoneName_);
            writeCommented(file(i), "Time [s] | Flow rate [m3s-1]");
            file() << endl;
            break; // exit the case structure
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
    // NOTE: call the base class constructor
    fvMeshFunctionObject(name, runTime, dict),
    logFiles(obr_, name),

    name_(name),
    active_(true),
    UName_("U"),
    // NOTE: Read the face zone to integrate over. Get its name from the dict, find
    // it in the mesh, and get a reference to the list of its faces.
    faceZoneName_(dict.lookup("faceZoneName")),
    faceZoneLabel_( mesh_.faceZones().findZoneID(faceZoneName_) ),
    faces_( mesh_.faceZones()[faceZoneLabel_] )
{
    // NOTE: calls the separate .read() method to import the controls from the dict.
    // dict reference is passed automatically by the OpenFOAM runtime object manager.
    read(dict);

    // built-in logFiles method for creating file streams.
    resetNames(createFileNames(dict));

    if (active_)
    {
        // 进行另外一个初始化，如果必要的话

        // NOTE: type() 返回头文件中TypeName()定义的名称， name_从字典中读取
        // Finished initialising pipeCalc: pipeCalculator
        Info << "Finished initialising " << type() << ": " << name_ << nl << endl;
    }
}

// * * * * * * * * * * * * * * * * 析构函数 * * * * * * * * * * * * * * * * //

Foam::functionObjects::pipeCalc::~pipeCalc()
{}

// * * * * * * * * * * * * * * * * 成员函数 * * * * * * * * * * * * * * * * //

// - active_时更新Uname,默认U，返回true
bool Foam::functionObjects::pipeCalc::read(const dictionary& dict)
{
    if (active_)
    {
        UName_ = dict.lookupOrDefault<word>("UName", "U");
    }
    return true;
}
// - 暂时没有功能
bool Foam::functionObjects::pipeCalc::execute()
{
    if (active_)
    {
        // 此函数在写入结果之前被调用，本应该放入一些东西来满足其他的函数的依赖（比如场计算）
    }
    return true;
}
// - 在active_时，执行excute_，返回true
bool Foam::functionObjects::pipeCalc::end()
{
    if (active_)
    {
        execute();
    }
    return true;
}
// - 暂时没有功能
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

        // Output to file - only execute on the master thread to avoid the file
        // getting written into from a few processors at the same time
        if (Pstream::master())
        {
            // Call the base class method which checks if the output file exists
            // and creates it, if necessary. That also calls the .writeFileHeader()
            // method of the derived class.
            logFiles::write();

            // Add the entry for this time step that has just been computed.
            file(MAIN_FILE) << obr_.time().value() << tab << flowRate << endl;
        }
    }
    return true;
}

// ************************************************************************* //
