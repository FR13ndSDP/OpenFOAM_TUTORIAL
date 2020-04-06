/**
 * @类
 *     Foam::functionObject::pipeCalc
 * @描述
 *     计算管中流动的流量
 * @源文件
 *     pipeCalc.C
 */

#ifndef pipeCalc_H
#define pipeCalc_H

#include "volFields.H"
#include "fvMeshFunctionObject.H"
#include "logFiles.H"
#include "addToRunTimeSelectionTable.H"
#include "fvc.H"
#include "Switch.H"

namespace Foam
{
namespace functionObjects
{
// - pipeCalc类声明
class pipeCalc
:
    // 继承自这两个类
    public fvMeshFunctionObject,
    public logFiles
{

private:
    // 被调用时的名称
    word name_;
    // 后处理开关状态
    bool active_;
    // 流速的名称，默认为"U"
    word UName_;
    // 面区名
    word faceZoneName_;
    // faceZone的下标
    label faceZoneLabel_;
    // faceZone中各面的下标
    const labelList&faces_;
    // 构造函数，使用引用构造
    pipeCalc(const pipeCalc&);
    // 引用赋值，而不是逐位赋值
    void operator=(const pipeCalc&);

protected:
    // 根据dict信息建立输出文件
    wordList createFileNames(const dictionary& dict) const;
    // 写文件头
    virtual void writeFileHeader(const label i);
    // 枚举变量
    enum fileID
    {
        MAIN_FILE = 0
    };

public:
    // 真正被添加到RTS中的名字
    // controlDict中调用使用的名称
    // 也是postProcesing中文件夹的名称
    TypeName("pipeCalcu")

    // 构造函数
    pipeCalc
    (
        const word& name,
        const Time& runTime,
        const dictionary& dict
    );

    virtual ~pipeCalc();
// === 成员函数 ===
    virtual const word& name() const { return name_; }

    virtual bool execute();

    virtual bool end();

    virtual void timeSet();

    virtual bool write();
};
// - 结束类声明
} // end namespace functionObjects
} // end namespace Foam

#endif
