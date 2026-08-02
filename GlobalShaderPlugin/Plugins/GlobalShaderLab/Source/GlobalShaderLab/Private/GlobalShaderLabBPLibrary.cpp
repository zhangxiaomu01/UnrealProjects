#include "GlobalShaderLabBPLibrary.h"
#include "GlobalShaderLabShaders.h"

#include "Engine/TextureRenderTarget2D.h"
#include "TextureResource.h"
#include "GlobalShader.h"            // GetGlobalShaderMap / FGlobalShaderMap / TShaderMapRef
#include "ShaderParameterStruct.h"
#include "PixelShaderUtils.h"        // FPixelShaderUtils::AddFullscreenPass
#include "RenderGraphUtils.h"        // FComputeShaderUtils::AddPass / AddCopyTexturePass
#include "RenderGraphResources.h"    // FRDGTextureUAVDesc
#include "RenderGraphBuilder.h"      // FRDGBuilder
#include "RHICommandList.h"          // FRHICommandListImmediate
#include "RHIResources.h"            // FRHITexture
#include "CommonRenderResources.h"   // CreateRenderTarget
#include "RenderCommandFence.h"

#define LOCTEXT_NAMESPACE "GlobalShaderLab"

// -----------------------------------------------------------------------------
// 通用说明：
//   渲染提交必须发生在渲染线程。我们用 ENQUEUE_RENDER_COMMAND 把“录制 RDG 图”的工作
//   切到渲染线程执行。FRDGBuilder 负责：声明资源、添加 Pass、Execute 时一次性提交给 RHI。
//   这正是理解 UE 现代渲染管线的核心：CPU 录制图(RDG) -> 一次性编译/执行 -> GPU。
// -----------------------------------------------------------------------------

void UGlobalShaderLabBPLibrary::DrawGradientToRenderTarget(
	UObject* WorldContextObject,
	UTextureRenderTarget2D* RenderTarget,
	FLinearColor ColorA,
	FLinearColor ColorB,
	float Time)
{
	if (!RenderTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("DrawGradientToRenderTarget: RenderTarget 为空。"));
		return;
	}

	// 取渲染目标资源（必须在游戏线程拿，再传给渲染线程）。
	FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RTResource)
	{
		return;
	}

	// 捕获需要的值，切到渲染线程录制 RDG 图。
	ENQUEUE_RENDER_COMMAND(DrawGradient)(
		[RTResource, ColorA, ColorB, Time](FRHICommandListImmediate& RHICmdList)
		{
			FRDGBuilder GraphBuilder(RHICmdList);

			const FIntPoint Extent = RTResource->GetSizeXY();

			// 1) 分配参数结构体（RDG 负责在图执行期间保活）。
			FGradientPS::FParameters* Params = GraphBuilder.AllocParameters<FGradientPS::FParameters>();
			Params->ColorA = FVector4f(ColorA.R, ColorA.G, ColorA.B, ColorA.A);
			Params->ColorB = FVector4f(ColorB.R, ColorB.G, ColorB.B, ColorB.A);
			Params->TextureSize = FVector2f(Extent.X, Extent.Y);
			Params->Time = Time;

			// 2) 把外部 RHI 纹理“注册”进 RDG，作为本 Pass 的渲染目标（输出）。
			FRDGTextureRef OutputTexture = GraphBuilder.RegisterExternalTexture(
				CreateRenderTarget(RTResource->TextureRHI, TEXT("GradientOutput")));
			Params->RenderTargets[0] = FRenderTargetBinding(OutputTexture, ERenderTargetLoadAction::EClear);

			// 3) 取出全局 Shader：GlobalShaderMap 在引擎启动时就为所有 FGlobalShader 编译好。
			FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
			TShaderMapRef<FGradientPS> PixelShader(GlobalShaderMap);

			// 4) 添加全屏 Pass：引擎会自动绘制一个覆盖整个视口的大三角形，
			//    我们的 Pixel Shader 负责给每个像素着色。视口范围 = 整张纹理。
			FPixelShaderUtils::AddFullscreenPass(
				GraphBuilder,
				GlobalShaderMap,
				RDG_EVENT_NAME("GlobalShaderLab_GradientPS"), // 抓帧(Malioc/RenderDoc)时能看到这个名字
				PixelShader,
				Params,
				FIntRect(0, 0, Extent.X, Extent.Y));

			// 5) 执行：真正把上面录制的图提交到 GPU。
			GraphBuilder.Execute();
		});
}

void UGlobalShaderLabBPLibrary::DrawComputePatternToRenderTarget(
	UObject* WorldContextObject,
	UTextureRenderTarget2D* RenderTarget,
	float Time)
{
	if (!RenderTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("DrawComputePatternToRenderTarget: RenderTarget 为空。"));
		return;
	}

	FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RTResource)
	{
		return;
	}

	ENQUEUE_RENDER_COMMAND(DrawComputePattern)(
		[RTResource, Time](FRHICommandListImmediate& RHICmdList)
		{
			FRDGBuilder GraphBuilder(RHICmdList);

			const FIntPoint Extent = RTResource->GetSizeXY();
			// 取输出纹理格式：AddCopyTexturePass 要求源/目标格式一致。
			const EPixelFormat Format = RTResource->TextureRHI->GetFormat();

			// 1) 创建一张可被计算着色器写入(UAV)的中间纹理。
			//    为什么不直接写 RenderTarget？因为 UTextureRenderTarget2D 的底层纹理通常没有 UAV 标志，
			//    所以更稳妥的通用做法是：CS 写中间纹理 -> 拷贝到 RenderTarget。
			const FRDGTextureDesc Desc = FRDGTextureDesc::Create2D(
				Extent,
				Format,
				FClearValueBinding::None,
				TexCreate_UAV | TexCreate_ShaderResource);
			FRDGTextureRef Intermediate = GraphBuilder.CreateTexture(Desc, TEXT("PatternIntermediate"));

			// 2) 填充参数：注意 UAV 通过 CreateUAV 生成视图后绑定。
			FPatternCS::FParameters* Params = GraphBuilder.AllocParameters<FPatternCS::FParameters>();
			Params->TextureSize = Extent;
			Params->Time = Time;
			Params->OutputTexture = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(Intermediate));

			FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
			TShaderMapRef<FPatternCS> ComputeShader(GlobalShaderMap);

			// 3) 派发：[numthreads(8,8,1)]，所以线程组数 = ceil(尺寸 / 8)。
			constexpr int32 GroupSize = 8;
			const FIntVector GroupCount(
				FMath::DivideAndRoundUp(Extent.X, GroupSize),
				FMath::DivideAndRoundUp(Extent.Y, GroupSize),
				1);

			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("GlobalShaderLab_PatternCS"),
				ComputeShader,
				Params,
				GroupCount);

			// 4) 把计算结果拷贝到外部 RenderTarget（拷贝是同格式 blit，安全可靠）。
			FRDGTextureRef OutputTexture = GraphBuilder.RegisterExternalTexture(
				CreateRenderTarget(RTResource->TextureRHI, TEXT("PatternOutput")));
			AddCopyTexturePass(GraphBuilder, Intermediate, OutputTexture);

			GraphBuilder.Execute();
		});
}

#undef LOCTEXT_NAMESPACE
