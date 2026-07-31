using UnrealBuildTool;

// 工程主模块：仅作为容器入口。真正的渲染/Shader 逻辑都在 Plugins/GlobalShaderLab 里，
// 这样职责清晰，也方便以后把插件迁移到其它工程。
public class GlobalShaderPlugin : ModuleRules
{
	public GlobalShaderPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
		});
	}
}
