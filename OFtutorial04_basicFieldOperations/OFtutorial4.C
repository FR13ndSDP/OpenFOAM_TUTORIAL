#include "fvCFD.H"

// 函数声明：计算给定时间t，给定位置x,给定参考点和尺度因子的压力值
// 计算p,参数为参考点x0,位置x,时间t,尺度因子。
scalar calculatePressure(scalar t, vector x, vector x0, scalar scale);

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

	// 读入场的所有参数，包括压力、速度、粘度
	#include "createFields4.H"
	
	// 定义一个不变的参考点
	const vector originVector(0.05,0.05,0.005);

	// 计算cell中心到参考点最大距离 用作计算压力的尺度因子
	const dimensionedScalar rFarCell = max( // 找出最大距离
		// 向量相减后取模
		// mesh.C()存储了网格的中心坐标场
		mag(dimensionedVector("x0",dimLength,originVector)-mesh.C()) // mag计算模
		); 
	const scalar farCell=rFarCell.value();// 有量纲距离转换到无量纲数
	Info<<"有量纲长度="<<rFarCell<<endl;
	Info<<"无量纲长度="<<farCell<<endl;

	// 当controlDict里要求开启循环，便执行下面的代码
	Info<< "\n开始时间循环\n" << endl;

	// 这将会自动增加时间
    while (runTime.loop())
    {
        Info<< "时间 = " << runTime.timeName() << nl << endl;

		// 循环所有cell计算压力
		for (label cellI=0; cellI<mesh.C().size(); cellI++)
		{
			// .value() 转换时间到无量纲数
			// 调用之前声明的函数
			p[cellI] = calculatePressure(runTime.time().value(), mesh.C()[cellI], originVector, farCell);
			// NOTE:
			// 这将会同时计算边界的值
		}
		//用压力梯度来冒充速度，由于压力梯度量纲不是m/s
		//乘以单位时间来获取正确的量纲，不必要，只是为描述正确
		// tmp是给单位时间随便起的名字
		// p单位是m^2/s^2,梯度单位m/s^2
		U = fvc::grad(p)*dimensionedScalar("tmp",dimTime,1.);//场操作fvc比循环高效
		
		// 如果controlDict要求了，写入结果到runTime.
		runTime.write();
	}

	Info << "结束！ 画出p的iso-contours范围(-10,10) 并对U使用glyph filter" << endl;

    Info<< "结束\n" << endl;

    return 0;
}

// 自定义函数定义
scalar calculatePressure(scalar t, vector x, vector x0, scalar scale)
{
	scalar r (mag(x-x0)/scale);
	scalar rR (1./(r+1e-12));
	// 频率
	scalar f (2.);
	// 避免歧义，使用Foam命名空间的sin函数
	return Foam::sin(2.*Foam::constant::mathematical::pi*f*t)*rR;
}

// ************************************************************************* //
