/*---------------------------------------------------------------------------*\
类名
    Foam::prescribedPipeInletFvPatchVectorField

组
    grpInletBoundaryConditions grpWallBoundaryConditions

描述
    预先描述圆柱形管的进口边界条件。来流方向由面法向自动确定，只需要给定来流速度
    \heading Patch usage

    \table
        Property    		| Description             								| Required  | Default value
        R           		| pipe radius 											| yes     	|
        flowSpeed   		| magnitude of flow velocity      						| yes       |
        deltaByR			| wall function thickness as a fraction of pipe radius  | yes 		|
        approximationType	| name of approximation profile							| no		| exponential
        centrePoint			| point on the centreline of the pipe					| no 		| (0 0 0)
        lambda				| pressure gradient coefficient 						| no 		| 0
    \endtable

    边界条件应用示例：
    \verbatim
    inlet
	{
		// 预先定义一个抛物线分布入口
	    type        prescribedPipeInlet;
	    // (optional, default exponential) type of profile used for approximation
	    approximationType exponential;
	    // 管半径
	    R	$Rpipe;
	    // 管中来流速度
	    flowSpeed	$flowVelocity;
	    // (optional, defaults at origin) centrepoint of the pipe used to determine the radius of each face
	    centrepoint	(0 0 0);
	    // inlet BL thickness as a function of radius
        // 来流边界层厚度，是半径的函数
	    deltaByR $deltaByR;
	    // (optional, default 0) pressure gradient coefficient (see Polhausen profile literature for explanation)
	    lambda 0.;
	    // dummy value overwritten by the BC, used for initialisation only
        // 假值，只用于初始化
	    value		$internalField;
	}
	\endverbatim

参考
    Foam::fixedValueFvPatchField

源文件
    prescribedPipeInletFvPatchVectorField.C

\*---------------------------------------------------------------------------*/

#ifndef prescribedPipeInletFvPatchVectorField_H
#define prescribedPipeInletFvPatchVectorField_H

#include "fvPatchFields.H"
#include "fixedValueFvPatchFields.H"
#include "Switch.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
               类 prescribedPipeInletFvPatchVectorField 声明
\*---------------------------------------------------------------------------*/

// NOTE: 此边界条件是fisedValue 边界条件的派生，因为它确实描述了固定值边界条件，尽管在所有面上不均匀（抛物线分布），而fixedValue 则是均匀分布的
class prescribedPipeInletFvPatchVectorField
:
    public fixedValueFvPatchVectorField // 自定义边界public继承自fixedValue边界类
{
private:
    // 私有数据
        // NOTE: 这些数据用来给定进口参数

        // 表明进行近似的类型（可选，默认为指数型）
    	word approximationType_;

    	// 流速
    	scalar flowSpeed_;

    	// 边界层厚度
    	scalar deltaByR_;

    	// 管子中心坐标 (可选, 默认为origin)
    	vector centrepoint_;

    	// 管半径
    	scalar R_;

        // 压力梯度系数（可选，默认为0）
    	scalar lambda_;

public:

    //- 返回类型信息
    // NOTE: this gets used by the runtimeSelector. In other words, this is the
    // name under which OpenFOAM knows this BC.
    TypeName("prescribedPipeInlet");

    // 构造函数

        //- 从patch和内部场构建
        prescribedPipeInletFvPatchVectorField
        (
            const fvPatch&,
            const DimensionedField<vector, volMesh>&
        );

        //- 从patch,内部场和字典构建
        prescribedPipeInletFvPatchVectorField
        (
            const fvPatch&,
            const DimensionedField<vector, volMesh>&,
            const dictionary&
        );

        //- 通过映射给定的prescribedPipeInletFvPatchVectorField到新patch来构造
        prescribedPipeInletFvPatchVectorField
        (
            const prescribedPipeInletFvPatchVectorField&,
            const fvPatch&,
            const DimensionedField<vector, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- 构建一个copy
        prescribedPipeInletFvPatchVectorField
        (
            const prescribedPipeInletFvPatchVectorField&
        );

        //- Construct and return a clone
        //- 构造并返回一个克隆
        virtual tmp<fvPatchVectorField> clone() const
        {
            return tmp<fvPatchVectorField>
            (
                new prescribedPipeInletFvPatchVectorField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        //- 构造一个copy，带有内部场引用
        prescribedPipeInletFvPatchVectorField
        (
            const prescribedPipeInletFvPatchVectorField&,
            const DimensionedField<vector, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchVectorField> clone
        (
            const DimensionedField<vector, volMesh>& iF
        ) const
        {
            return tmp<fvPatchVectorField>
            (
                new prescribedPipeInletFvPatchVectorField(*this, iF)
            );
        }

    // 成员函数

        // 计算函数

            //- 更新与patch场关联的的系数
            virtual void updateCoeffs();

        // I-O

            //- 写
            virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // 结束Foam命名空间

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
