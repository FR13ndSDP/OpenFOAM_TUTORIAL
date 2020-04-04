#include "prescribedPipeInletFvPatchVectorField.H"

// * * * * * * * * * * * * * * * * 构造函数  * * * * * * * * * * * * * * //

Foam::prescribedPipeInletFvPatchVectorField::prescribedPipeInletFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF
)
:
    // NOTE: 调用默认构造函数（基类构造函数）确保所有正常初始化
    fixedValueFvPatchVectorField(p, iF),
    // NOTE： 使用初始化列表给成员赋默认值
    approximationType_("exponential"),
    flowSpeed_(0.),
	deltaByR_(0.),
	centrepoint_(vector::zero),
	R_(0.),
	lambda_(0.)
{}

Foam::prescribedPipeInletFvPatchVectorField::prescribedPipeInletFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const dictionary& dict
)
:
    // NOTE: 这个构造函数从U文件边界条件里里读所有的控制参数，在这里作为一个字典的引用引入
    fixedValueFvPatchVectorField(p, iF),
    approximationType_("exponential"),
    flowSpeed_(0.),
	deltaByR_(0.),
	centrepoint_(vector::zero),
	R_(0.),
	lambda_(0.)
{
    // NOTE: 调用重载算符 = 将边界条件条件实现在每个face上，此例中value是关键字 $internalField
    fvPatchVectorField::operator=(vectorField("value", dict, p.size()));

    // NOTE: 寻找必要的参数
    // 带有默认值 exponential的lookup
    approximationType_ = dict.lookupOrDefault<word>("approximationType","exponential");
    dict.lookup("flowSpeed") >> flowSpeed_;
	dict.lookup("deltaByR") >> deltaByR_;
	centrepoint_ = dict.lookupOrDefault<vector>("centrepoint",vector::zero);
	dict.lookup("R") >> R_;
	lambda_ = dict.lookupOrDefault<scalar>("lambda",0.);

    // NOTE: 调用.updateCoeffs() 来计算入口参数，使用了刚初始化好的值
	updateCoeffs();
}

Foam::prescribedPipeInletFvPatchVectorField::prescribedPipeInletFvPatchVectorField
(
    const prescribedPipeInletFvPatchVectorField& ptf,
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    // NOTE: 这个和接下来的两个构造函数都是通过别的类来定义的
    fixedValueFvPatchVectorField(ptf, p, iF, mapper),
    approximationType_(ptf.approximationType_),
    flowSpeed_(ptf.flowSpeed_),
	deltaByR_(ptf.deltaByR_),
	centrepoint_(ptf.centrepoint_),
	R_(ptf.R_),
	lambda_(ptf.lambda_)
{}

Foam::prescribedPipeInletFvPatchVectorField::prescribedPipeInletFvPatchVectorField
(
    const prescribedPipeInletFvPatchVectorField& rifvpvf
)
:
    fixedValueFvPatchVectorField(rifvpvf),
    approximationType_(rifvpvf.approximationType_),
    flowSpeed_(rifvpvf.flowSpeed_),
    deltaByR_(rifvpvf.deltaByR_),
    centrepoint_(rifvpvf.centrepoint_),
    R_(rifvpvf.R_),
    lambda_(rifvpvf.lambda_)
{}

Foam::prescribedPipeInletFvPatchVectorField::prescribedPipeInletFvPatchVectorField
(
    const prescribedPipeInletFvPatchVectorField& rifvpvf,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchVectorField(rifvpvf, iF),
    approximationType_(rifvpvf.approximationType_),
    flowSpeed_(rifvpvf.flowSpeed_),
    deltaByR_(rifvpvf.deltaByR_),
    centrepoint_(rifvpvf.centrepoint_),
    R_(rifvpvf.R_),
    lambda_(rifvpvf.lambda_)
{}

// * * * * * * * * * * * * * * * 成员函数  * * * * * * * * * * * * * //

// NOTE: 这是计算实际进口参数的关键代码
void Foam::prescribedPipeInletFvPatchVectorField::updateCoeffs()
{
    // 基类中的方法，如果边界条件得到了更新，就返回true
    if (updated())
    {
        return;
    }
    
    // 入口的法向速度赋给入口patch, 负号是因为patch法向默认指向几何外
	vectorField Uin = (-1.)*(patch().Sf()/patch().magSf()) * flowSpeed_;

    // 遍历每个面并对靠近壁面的网格进行边界层近似
	forAll(patch().Cf(), faceI)
	{
        // 距离壁面的无量纲距离
		scalar yOverDelta ( (1.-mag(centrepoint_ - patch().Cf()[faceI])/R_)/deltaByR_ );

		if (approximationType_.compare("parabolic") == 0)
		{
			if (yOverDelta < 1.0) // 处在边界层内
				Uin[faceI] *= (2*yOverDelta-pow(yOverDelta,2.0)); // 乘以一个壁面函数
		}
		else if (approximationType_.compare("Polhausen") == 0)
		{
			if (yOverDelta < 1.0)
				Uin[faceI] *= 1.-(1.+yOverDelta)*pow(1.-yOverDelta,3.) + lambda_/6.*yOverDelta*pow(1.-yOverDelta,3.);
		}
		else if (approximationType_.compare("exponential") == 0)
		{
			if (yOverDelta < 1.0)
				Uin[faceI] *= pow(yOverDelta,1./7.);
		}
		else
		{
			FatalErrorIn
		    (
		        "prescribedPipeInletFvPatchVectorField::updateCoeffs()"
		    )   << "未知边界层近似类型" << approximationType_ << nl << nl
		        << "可用类型 :" << nl
		        << tab << "parabolic" << nl
		        << tab << "Polhausen" << nl
		        << tab << "exponential" << nl
		        << exit(FatalError);
		}
	}



    // 把这个patch上的值设为刚计算出的流速
    this->operator==(Uin);

    // 调用基类方法来确保所有其他的边角得到更新
    fixedValueFvPatchVectorField::updateCoeffs();
}


void Foam::prescribedPipeInletFvPatchVectorField::write(Ostream& os) const
{
    fvPatchVectorField::write(os);
    os.writeKeyword("approximationType") << approximationType_ << token::END_STATEMENT << nl;
    os.writeKeyword("flowSpeed") << flowSpeed_ << token::END_STATEMENT << nl;
    os.writeKeyword("deltaByR") << deltaByR_ << token::END_STATEMENT << nl;
    os.writeKeyword("centrepoint") << centrepoint_ << token::END_STATEMENT << nl;
    os.writeKeyword("R") << R_ << token::END_STATEMENT << nl;
    os.writeKeyword("lambda") << lambda_ << token::END_STATEMENT << nl;
    writeEntry(os, "value", *this);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// 将定定义好的边界加到 0.org/U 中边界的选择范围内
namespace Foam
{
    makePatchTypeField // 加入到patch的RTS（run-time-selection）
    (
        fvPatchVectorField, // 加入到 fvPatchVectorField 中
        prescribedPipeInletFvPatchVectorField // 自定义的边界条件
    );
}

// ************************************************************************* //
