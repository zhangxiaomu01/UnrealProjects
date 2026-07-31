using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
	// 学习 GlobalShader 必备的几个渲染相关模块：
	//   RenderCore  —— FGlobalShader 基类、FPixelShaderUtils、FComputeShaderUtils、AddCopyTexturePass
	//   RHI         —— FRDGBuilder、FRDGTextureDesc、FRenderTargetBinding、CreateRenderTarget 等底层接口
	//   (不需要 Renderer 模块：本示例不使用 FScreenPass* 等管线内部类型)
	public class GlobalShaderLab : ModuleRules
	{
		public GlobalShaderLab(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"RenderCore",
				"RHI",
				"Projects",
			});
		}
	}
}
