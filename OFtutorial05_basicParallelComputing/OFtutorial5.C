#include "fvCFD.H"

int main(int argc, char *argv[])
{

    argList::addNote("并行运算");
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

	//对于并行运行的算例，区域被分成几块，每个都运行在单独的核心上并且具有mesh\U\p等实例
	//Pout允许每个核心进行并行输出，不像Info只允许head process(processor0)使用
    // Pout是std::cout的并行封装
    Pout << "来自处理器 " << Pstream::myProcNo() << " 的问候! 我将处理 "
         << mesh.C().size() << " 个cells" << endl;

    //遍历所有cell并将体积相加
    scalar meshVolume(0.); //初始化为零
    forAll(mesh.V(),cellI) // for all cellI in mesh, calculate volume
        meshVolume += mesh.V()[cellI];
    // 将所有处理器的值相加
    Pout << "这个处理器上的网格体积: " << meshVolume << endl;
    reduce(meshVolume, sumOp<scalar>());
    Info << "所有处理器上网格总体积: " << meshVolume
        // reduction操作直接改变meshVolume值而不用定义新变量
         << " ，是 " << returnReduce(mesh.C().size(), sumOp<label>()) << " 个cells的总和" << endl;
    // sumOp<typeName>制定了进行的迭代操作是相加，指明作用类型为scalar
    // Custom reduction operations are easy to implement but need fluency in
    // object-oriented programming in OpenFOAM, so we'll skip this for now.

    // scatter 使得值从处理器总和中分离
    Pstream::scatter(meshVolume);
    Pout << "这个处理器上的网格体积现在为 " << meshVolume << endl;

    // 检查某一个量在所有核心上的分布经常是有用的，通过list实现
    // 下面检查每个处理器下有几个internalFace,几个patch
    // nProcs:处理器个数
    List<label> nInternalFaces (Pstream::nProcs()), nBoundaries (Pstream::nProcs()); // 声明列表及其大小
    nInternalFaces[Pstream::myProcNo()] = mesh.Cf().size(); // 读取每个处理器的size
    nBoundaries[Pstream::myProcNo()] = mesh.boundary().size();

    // The list may then be gathered on the head node as
    Pstream::gatherList(nInternalFaces);
    Pstream::gatherList(nBoundaries);
    // Scattering a list is also possbile
    Pstream::scatterList(nInternalFaces);
    Pstream::scatterList(nBoundaries);

    // Note how the gathered lists hold information for all processors now.
    if (Pstream::master()) // 判断我是否为master处理器，也就是Info输出的处理器
    {
        forAll(nInternalFaces,i)
            Info << "处理器 " << i << " 有 " << nInternalFaces[i]
                 << " 个内部面和 " << nBoundaries[i] << " 个边界面" << endl;
    }

    // 当网格被剖分，原本的内部面可能变成patch，也就是所谓的"处理器边界"
    // 分区域将视这些处理器边界为边界条件
    forAll(mesh.boundary(),patchI)
        Pout << "Patch " << patchI << " 名字叫 " << mesh.boundary()[patchI].name() << endl;

    // 检查处理器边界
    forAll(mesh.boundary(),patchI)
    {
        const polyPatch& pp = mesh.boundaryMesh()[patchI];
        if (isA<processorPolyPatch>(pp))
            Pout << "Patch " << patchI << " 名叫 " << mesh.boundary()[patchI].name()
                 << " 一定是个处理器边界!" << endl;
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

	Info<< "\n开启时间循环\n" << endl;

    while (runTime.loop())
    {
        Info<< "时间 = " << runTime.timeName() << nl << endl;

        // assign values to the field;
		//sin函数需要无量纲参数，因此使用.value()把时间无量纲化
		//r拥有长度量纲，因此加上去的微小量也要有长度量纲
        // 最终结果应和压强量纲匹配, kg/(m*s^2)
		//[kg,m,s,K,kg/mol,A,cd]
		p = Foam::sin(2.*constant::mathematical::pi*f*runTime.time().value())
            / (r/rFarCell + dimensionedScalar("small", dimLength, 1e-12))
            * dimensionedScalar("tmp", dimensionSet(0, 3, -2, 0, 0), 1.);

        // 这里的目的是更新处理器边界上的值，如果不这么做，计算梯度时会出错
        p.correctBoundaryConditions();

        // 计算速度场
		U = fvc::grad(p)*dimensionedScalar("tmp", dimTime, 1.);
		runTime.write();
	}
	
    Info<< "End\n" << endl;

    return 0;
}

// ************************************************************************* //
