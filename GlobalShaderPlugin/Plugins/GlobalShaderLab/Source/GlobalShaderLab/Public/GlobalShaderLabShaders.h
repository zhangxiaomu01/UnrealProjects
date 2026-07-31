#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"                 // FGlobalShader、DECLARE/SHADER_USE_PARAMETER_STRUCT
#include "ShaderParameterStruct.h"        // BEGIN/END_SHADER_PARAMETER_STRUCT、SHADER_PARAMETER_*
#include "RenderGraphResources.h"         // FRDGTextureUAVRef（RDG 纹理 UAV 参数所需类型）
#include "DataDrivenShaderPlatformInfo.h" // GetMaxSupportedFeatureLevel
#include "Math/IntPoint.h"

// =============================================================================
// 示例 1：一个 Pixel Shader
// -----------------------------------------------------------------------------
// FGlobalShader 是“不依赖场景/材质、全局唯一”的 Shader 基类。它最适合用来：
//   - 全屏后处理 / 工具类绘制
//   - 学习渲染管线时，单独跑一个最简单的着色器
//
// 整体套路是固定的三件套：
//   DECLARE_GLOBAL_SHADER(类名)              —— 在类内声明，注册为全局 Shader 类型
//   SHADER_USE_PARAMETER_STRUCT(类名, 父类)  —— 启用参数结构体（现代写法）
//   BEGIN_SHADER_PARAMETER_STRUCT(FParameters,) ... END —— 声明传给 GPU 的参数
// 然后在 .cpp 里用 IMPLEMENT_GLOBAL_SHADER 把它与 .usf 文件里的入口函数绑定。
// =============================================================================
class FGradientPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FGradientPS);
	SHADER_USE_PARAMETER_STRUCT(FGradientPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		// 普通常量参数：类型用 UE 的 float 向量类型（会自动映射成 HLSL 的 float4/float2）
		SHADER_PARAMETER(FVector4f, ColorA)       // 渐变上端颜色
		SHADER_PARAMETER(FVector4f, ColorB)       // 渐变下端颜色
		SHADER_PARAMETER(FVector2f, TextureSize)  // 输出纹理尺寸（像素）
		SHADER_PARAMETER(float, Time)             // 时间，用于驱动动画

		// 渲染目标绑定槽：让这个 Pass 能把颜色写到 RenderTarget 上
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	// 引擎会调用它决定“在哪些平台上编译这个 Shader 的哪些排列”。
	// 这里要求至少 SM5（即 DX11 级别），是跑光栅化 Pixel Shader 的基本门槛。
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	}
};

// =============================================================================
// 示例 2：一个 Compute Shader
// -----------------------------------------------------------------------------
// 计算着色器同样继承自 FGlobalShader，区别在于：
//   - 它不经过光栅化，没有顶点/像素阶段，而是按线程组(线程组)并行执行
//   - 输出通常写到 UAV（这里是一张可读写纹理），而不是渲染目标
// 因此参数结构体里用的是 SHADER_PARAMETER_RDG_TEXTURE_UAV 而不是 RENDER_TARGET_BINDING_SLOTS。
// =============================================================================
class FPatternCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FPatternCS);
	SHADER_USE_PARAMETER_STRUCT(FPatternCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(FIntPoint, TextureSize)                         // 纹理尺寸 = 线程覆盖范围
		SHADER_PARAMETER(float, Time)                                    // 时间
		// RDG 纹理 UAV：C++ 侧成员类型为 FRDGTextureUAVRef，
		// .usf 侧对应 RWTexture2D<float4>。元素类型由纹理格式决定。
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutputTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	}
};
