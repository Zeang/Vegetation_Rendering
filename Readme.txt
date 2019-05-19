Source_lib文件夹请放置到D盘
没有D盘的请自己设置相应文件包含路径
运行环境为VS2017 15  32位
编译
运行时请把Source_lib\dlls\assimp-vs140-mt.dll置于生成的exe目录下
或者设置Source_lib\dlls到环境变量

VegetationRendering.cpp中有main函数
其中46-47行的
TREE_NUMBER为场景中生成的树的数量
DEPTH为LOD层级
1000棵树建议DEPTH = 4
10000棵树建议DEPTH = 5 (注意栈区大小应至少设置为 DEPTH * 16 * 4 * TREE_NUMBER，一般默认为1m)
可在项目->属性->链接器->系统->堆栈的保留大小 处设置
