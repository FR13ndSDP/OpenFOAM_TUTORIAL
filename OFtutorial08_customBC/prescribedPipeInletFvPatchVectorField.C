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
    // NOTE: this constructor reads all of the control parameters from the boundary
    // condition definition specified in the time folder U file, imported here
    // as a dictionary reference.
    // NOTE: 这个构造函数从U文件边界条件里里读所有的控制参数，在这里作为一个字典的引用引入
    fixedValueFvPatchVectorField(p, iF),
    approximationType_("exponential"),
    flowSpeed_(0.),
	deltaByR_(0.),
	centrepoint_(vector::zero),
	R_(0.),
	lambda_(0.)
{
    // NOTE: calls the = operator to assign the value to the faces held by this BC
    fvPatchVectorField::operator=(vectorField("value", dict, p.size()));

    // NOTE: 寻找必要的参数
    approximationType_ = dict.lookupOrDefault<word>("approximationType","exponential");
    dict.lookup("flowSpeed") >> flowSpeed_;
	dict.lookup("deltaByR") >> deltaByR_;
	centrepoint_ = dict.lookupOrDefault<vector>("centrepoint",vector::zero);
	dict.lookup("R") >> R_;
	lambda_ = dict.lookupOrDefault<scalar>("lambda",0.);

    // NOTE: calls the .updateCoeffs() method to calculate the inlet profile in
    // accordance with the controls which have just been read.
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
    // NOTE: this constructor, and the two subsequent ones, transfer data to the
    // instance being created from another one.
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

// void Foam::prescribedPipeInletFvPatchVectorField::autoMap
// (
//     const fvPatchFieldMapper& m
// )
// {
//     vectorField::autoMap(m);
// }
//
//
// void Foam::prescribedPipeInletFvPatchVectorField::rmap
// (
//     const fvPatchVectorField& ptf,
//     const labelList& addr
// )
// {
//     fixedValueFvPatchVectorField::rmap(ptf, addr);
// }

// NOTE: this is the key method which implements the actual maths for calculating
// the inlet profiles.
// NOTE: 这是计算实际进口参数的关键代码
void Foam::prescribedPipeInletFvPatchVectorField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

	// assign inlet velocity normal to the patch
	// by convention, patch faces point outside of the domain
	vectorField Uin = (-1.)*(patch().Sf()/patch().magSf()) * flowSpeed_;

    // go over each face and add the BL profile for faces close to the wall
	forAll(patch().Cf(), faceI)
	{
        // non-dimensional distance away from the wall
		scalar yOverDelta ( (1.-mag(centrepoint_ - patch().Cf()[faceI])/R_)/deltaByR_ );

		if (approximationType_.compare("parabolic") == 0)
		{
			if (yOverDelta < 1.0)
				Uin[faceI] *= (2*yOverDelta-pow(yOverDelta,2.0));
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
		    )   << "Unknown boundary layer profile approximation type " << approximationType_ << nl << nl
		        << "Valid types are :" << nl
		        << tab << "parabolic" << nl
		        << tab << "Polhausen" << nl
		        << tab << "exponential" << nl
		        << exit(FatalError);
		}
	}



	// set the value_ of this patch to the newly computed flow speed
    this->operator==(Uin);

    // call the base class method to make sure all the other bits and pieces get updated
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

namespace Foam
{
    makePatchTypeField
    (
        fvPatchVectorField,
        prescribedPipeInletFvPatchVectorField
    );
}

// ************************************************************************* //
