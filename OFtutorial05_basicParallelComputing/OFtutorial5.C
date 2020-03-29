#include "fvCFD.H"

int main(int argc, char *argv[])
{

    argList::addNote("并行运算");
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

	//对于并行运行的算例，区域被分成几块，每个都运行在单独的核心上并且具有mesh\U\p等实例
	//Pout允许每个核心进行输出，不像Info只允许head process(processor0)使用
    Pout << "Hello from processor " << Pstream::myProcNo() << "! I am working on "
         << mesh.C().size() << " cells" << endl;

    // To exchange information between processes, special OpenMPI routines need
    // to be called.
    //遍历所有cell并将体积相加
    scalar meshVolume(0.);
    forAll(mesh.V(),cellI)
        meshVolume += mesh.V()[cellI];
    // 将所有处理器的值相加
    Pout << "Mesh volume on this processor: " << meshVolume << endl;
    reduce(meshVolume, sumOp<scalar>());
    Info << "Total mesh volume on all processors: " << meshVolume
        // reduction操作直接改变meshVolume值而不用定义新变量
         << " over " << returnReduce(mesh.C().size(), sumOp<label>()) << " cells" << endl;
    // During the reduction stage, different operations may be carried out, summation,
    // described by the sumOp template, being one of them.
    // Other very useful operations are minOp and maxOp.
    // Note how the type
    // of the variable must be added to make an instance of the template, here
    // this is done by adding <scalar> in front of the brackets.
    // Custom reduction operations are easy to implement but need fluency in
    // object-oriented programming in OpenFOAM, so we'll skip this for now.

    // scatter 使得值从处理器总和中分离
    Pstream::scatter(meshVolume);
    Pout << "Mesh volume on this processor is now " << meshVolume << endl;

    // It is often useful to check the distribution of something across all
    // processors. This may be done using a list, with each element of it
    // being written to by only one processor.
    List<label> nInternalFaces (Pstream::nProcs()), nBoundaries (Pstream::nProcs());
    nInternalFaces[Pstream::myProcNo()] = mesh.Cf().size();
    nBoundaries[Pstream::myProcNo()] = mesh.boundary().size();

    // The list may then be gathered on the head node as
    Pstream::gatherList(nInternalFaces);
    Pstream::gatherList(nBoundaries);
    // Scattering a list is also possbile
    Pstream::scatterList(nInternalFaces);
    Pstream::scatterList(nBoundaries);

    // It can also be useful to do things on the head node only
    // (in this case this is meaningless since we are using Info, which already
    // checks this and executes on the head node).
    // Note how the gathered lists hold information for all processors now.
    if (Pstream::master())
    {
        forAll(nInternalFaces,i)
            Info << "Processor " << i << " has " << nInternalFaces[i]
                 << " internal faces and " << nBoundaries[i] << " boundary patches" << endl;
    }

    // As the mesh is decomposed, interfaces between processors are turned
    // into patches, meaning each subdomain sees a processor boundary as a
    // boundary condition.
    forAll(mesh.boundary(),patchI)
        Pout << "Patch " << patchI << " named " << mesh.boundary()[patchI].name() << endl;

    // When looking for processor patches, it is useful to check their type,
    // similarly to how one can check if a patch is of empty type
    forAll(mesh.boundary(),patchI)
    {
        const polyPatch& pp = mesh.boundaryMesh()[patchI];
        if (isA<processorPolyPatch>(pp))
            Pout << "Patch " << patchI << " named " << mesh.boundary()[patchI].name()
                 << " is definitely a processor boundary!" << endl;
    }

    // ---
    // tutorial4改写成并行，不同之处用 NOTE 注明.

    // 在openfoam中常把代码块放到include里让主程序更易读，这点与标准c++的include里
	//放定义与声明有所不同。
    // 一个常见的include, 除了通用的setRootCase, createTime, 和createMesh,
    // 就是createFields, 我们把代码中建立场和transport constant的部分放到这个头文件里
    #include "createFields.H"

    // 预先计算几何信息，使用场操作而不是cell循环
	const dimensionedVector originVector("x0", dimLength, vector(0.05,0.05,0.005));
    volScalarField r (mag(mesh.C()-originVector));
    // NOTE: we need to get a global value; convert from dimensionedScalar to scalar
	const scalar rFarCell = returnReduce(max(r).value(), maxOp<scalar>());
    scalar f (1.);

	Info<< "\nStarting time loop\n" << endl;

    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        // assign values to the field;
		//sin函数需要无量纲参数，因此使用.value()把时间无量纲化
		//r拥有长度量纲，因此加上去的微小量也要有长度量纲
        // 最终结果应和压强量纲匹配, kg/(m*s^2)
		//[kg,m,s,K,kg/mol,A,cd]
		p = Foam::sin(2.*constant::mathematical::pi*f*runTime.time().value())
            / (r/rFarCell + dimensionedScalar("small", dimLength, 1e-12))
            * dimensionedScalar("tmp", dimensionSet(0, 3, -2, 0, 0), 1.);

        // NOTE: this is needed to update the values on the processor boundaries.
        // If this is not done, the gradient operator will get confused around the
        // processor patches.
        p.correctBoundaryConditions();

        // calculate velocity from gradient of pressure
		U = fvc::grad(p)*dimensionedScalar("tmp", dimTime, 1.);
		runTime.write();
	}
	
    Info<< "End\n" << endl;

    return 0;
}

// ************************************************************************* //
