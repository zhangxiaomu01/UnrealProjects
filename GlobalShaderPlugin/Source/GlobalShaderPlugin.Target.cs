using UnrealBuildTool;
using System.Collections.Generic;

// 游戏打包目标（Runtime）。学习时一般直接用 Editor 目标即可。
public class GlobalShaderPluginTarget : TargetRules
{
	public GlobalShaderPluginTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ExtraModuleNames.Add("GlobalShaderPlugin");
	}
}
