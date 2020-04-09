/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2020 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::customSource

Description

SourceFiles
    customSource.C
    customSourceTemplates.C

\*---------------------------------------------------------------------------*/

#ifndef _Template_H
#define _Template_H

#include "fvCFD.H"
#include "fvOption.H"
#include "fvMesh.H"
#include "fvMatrix.H"
#include "geometricOneField.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes
class Istream;
class Ostream;

// Forward declaration of friend functions and operators
class customSource;
Istream& operator>>(Istream&, customSource&);
Ostream& operator<<(Ostream&, const customSource&);

namespace fv
{
/*---------------------------------------------------------------------------*\
                         Class customSource Declaration
\*---------------------------------------------------------------------------*/

class customSource
:
    public option
{
protected:
    // Protected Data

        // - 圆盘区域法向量
        vector diskDir_;

        // 圆盘中心
        point diskCenter_;

        // 直径
        scalar diameter_;

        // 厚度
        scalar t_;

        // 
        scalar Cp_;

        scalar Ct_;

        point upstreamPoint_;

        // 上游cell ID
        label upstreamCellId_;

        // 施加源项的cell列表
        labelList cells_;

        // cell体积和
        scalar V_;
private:

    // Private Member Functions

        // 检查从字典输入的数据
        void checkData() const;

        // 源项
        template<class RhoFieldType>
        void calculateMomentumSource
        (
            vectorField& Usource,
            const labelList& cells,
            const scalarField& v,
            const RhoFieldType& rho,
            const vectorField& U
        ) const;

        customSource(const customSource&);

        void operator=(const customSource&);

public:
    // RTS 信息
    TypeName("customSource");

    // 构造函数
    customSource
    (
        const word& name,
        const word& modelType,
        const dictionary& dict,
        const fvMesh& mesh
    );
    // 析构函数
    virtual ~customSource()
    {}
    
    // 成员函数

        // 获取值
        scalar Cp() const
        {
            return Cp_;
        }

        scalar Ct() const
        {
            return Ct_;
        }

        const vector& diskDir()
        {
            return diskDir_;
        }

        inline scalar diskArea() const
        {
            return constant::mathematical::pi * pow(diameter_/2., 2);
        }

        scalar V() const
        {
            return V_;
        }

        // 动量方程源项
        virtual void addSup
        (
            fvMatrix<vector>& eqn,
            const label fieldi
        );
        // 可压缩动量方程
         virtual void addSup
        (
            const volScalarField& rho,
            fvMatrix<vector>& eqn,
            const label fieldi
        );

    // IO
        virtual bool read(const dictionary& dict); 
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
} // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "customSourceTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
