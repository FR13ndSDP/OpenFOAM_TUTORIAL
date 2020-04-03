/*---------------------------------------------------------------------------*\
类
    Foam::pipeCalc

描述
    这个类的函数对象计算并输出与管或相似几何中流动有关的多种量

源文件
    pipeCalc.C

\*---------------------------------------------------------------------------*/

#ifndef pipeCalc_H
#define pipeCalc_H

#include "volFieldsFwd.H"
#include "Switch.H"
#include "fvc.H"
#include "fvMeshFunctionObject.H"
#include "logFiles.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{

/*---------------------------------------------------------------------------*\
                          类 pipeCalc 声明
\*---------------------------------------------------------------------------*/

class pipeCalc
:
    // NOTE: derive from the base fvMeshFunctionObject and logFiles in order to
    // provide a ready interface for dealing with output files
    // NOTE: 从基类fvMeshFunctionObject和logFiles 继承而来，后者为结果输出提供保障
    public fvMeshFunctionObject,
    public logFiles
{
private:
    // 私有数据

        //- Name of this set of pipeCalc objects
        word name_;

        //- 开/关 控制
        bool active_;

        //- Name of velocity field, default is "U"
        //- 速度场名字，默认为 "U"
        word UName_;

        // NOTE: these fields hold the information about the faceZone
        // Name of the face zone
        // NOTE: 下面这些场掌握着面区的信息
        // 面区名称
        word faceZoneName_;

        // index of the face zone in the mesh container
        // mesh 容器中面区的下标
		label faceZoneLabel_;

        // indices of the faces of the CS in the mesh - keep as reference, should be faster for large sets
        // 使用引用比巨大的数组要快
        const labelList& faces_;

    // 私有成员函数

        //- Disallow default bitwise copy construct
        //- 不允许默认的逐位复制构造方式
        pipeCalc(const pipeCalc&);

        //- Disallow default bitwise assignment
        //- 禁用默认的逐位赋值
        void operator=(const pipeCalc&);

protected:
    // NOTE: define a list of files this object writes to; the underlying logFiles
    // object will handle output to correct streams automatically.
    // NOTE: 定义了此对象写入的文件列表；潜在的logFiles对象使得输出到正确的IO流中
    //- 枚举变量 fileID
    enum fileID
    {
        MAIN_FILE = 0
    };

    // NOTE: Create file names for each of the output files
    // NOTE: 创建每个输出文件的文件名
    wordList createFileNames(const dictionary& dict) const;

    // NOTE: this first gets declared in the baseline object and gets used to
    // write the header in the output file.
    // NOTE: 此函数首先是在基类中定义的，用于在输出文件中写入header
    virtual void writeFileHeader(const label i);

public:

    //- Runtime type information
    //- Runtime 类型的信息
    TypeName("pipeCalc");

    // 构造函数

        //- Construct for given Time and dictionary.
        //- 给定时间和字典的构造
        pipeCalc
        (
            const word& name,
            const Time& runTime,
            const dictionary& dict
        );

    //- 析构函数
    virtual ~pipeCalc();

    // 成员函数

    //- Return name of the set of pipeCalc
    //- 返回
    virtual const word& name() const { return name_; }

    //- Read the pipeCalc data
    //- 读入pipeCalc数据
    virtual bool read(const dictionary&);

    //- Execute, currently does nothing
    virtual bool execute();

    //- Execute at the final time-loop, currently does nothing
    virtual bool end();

    //- Called when time was set at the end of the Time::operator++
    virtual void timeSet();

    // NOTE: Key method which implements the object's actual functionality
    virtual bool write();

    //- Update for changes of mesh
    virtual void updateMesh(const mapPolyMesh&) {}

    //- Update for changes of mesh
    virtual void movePoints(const polyMesh&) {}
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace functionObjectFile
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
