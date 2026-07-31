#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GlobalShaderLabBPLibrary.generated.h"

// 蓝图函数库：把这些 Shader 的触发入口暴露给蓝图，方便在编辑器里随时调用验证。
// 学习时建议：新建一个 TextureRenderTarget2D 资源 -> 调用下面的函数 -> 用纹理采样器/材质观察结果。
UCLASS()
class GLOBALSHADERLAB_API UGlobalShaderLabBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 用 Pixel Shader（FGradientPS）把一段渐变绘制到 RenderTarget。
	// ColorA/ColorB 为两端颜色，Time 驱动动画（可在 Tick 里传累计时间）。
	UFUNCTION(BlueprintCallable, Category = "GlobalShaderLab", meta = (WorldContext = "WorldContextObject"))
	static void DrawGradientToRenderTarget(
		UObject* WorldContextObject,
		UTextureRenderTarget2D* RenderTarget,
		FLinearColor ColorA,
		FLinearColor ColorB,
		float Time);

	// 用 Compute Shader（FPatternCS）把一段程序化波纹图案写入 RenderTarget。
	UFUNCTION(BlueprintCallable, Category = "GlobalShaderLab", meta = (WorldContext = "WorldContextObject"))
	static void DrawComputePatternToRenderTarget(
		UObject* WorldContextObject,
		UTextureRenderTarget2D* RenderTarget,
		float Time);
};
