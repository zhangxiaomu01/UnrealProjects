using UnrealBuildTool;
using System.Collections.Generic;

// 编辑器目标：日常学习和迭代都编译这个目标（Development Editor）。
public class GlobalShaderPluginEditorTarget : TargetRules
{
	public GlobalShaderPluginEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ExtraModuleNames.Add("GlobalShaderPlugin");
	}
}
