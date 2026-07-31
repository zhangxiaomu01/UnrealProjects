#include "GlobalShaderLabShaders.h"

#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

// =============================================================================
// IMPLEMENT_GLOBAL_SHADER：把 C++ 的 FGlobalShader 类，与 .usf 源文件里的入口函数绑定。
// 参数含义：
//   类名,  虚拟着色器路径,                          入口函数名, 阶段
//
// 【关键约定】插件 Shader 的虚拟路径必须是：
//     /Plugin/<插件名>/Private/<文件名>.usf
// 对应的物理文件就在插件的 Shaders/Private/ 目录下（UBT 会自动打包这个目录）。
// 运行时用这个虚拟路径去查找，所以两边名字务必一致。
// =============================================================================

IMPLEMENT_GLOBAL_SHADER(FGradientPS, "/Plugin/GlobalShaderLab/Private/GradientPS.usf", "MainPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FPatternCS, "/Plugin/GlobalShaderLab/Private/PatternCS.usf", "MainCS", SF_Compute);
