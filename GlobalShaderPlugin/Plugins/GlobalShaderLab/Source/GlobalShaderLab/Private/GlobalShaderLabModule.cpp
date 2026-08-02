#include "GlobalShaderLabModule.h"

#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"            // AddShaderSourceDirectoryMapping
#include "Misc/Paths.h"

DEFINE_LOG_CATEGORY_STATIC(LogGlobalShaderLab, Log, All);

void FGlobalShaderLabModule::StartupModule()
{
	// 引擎不会自动为插件注册 /Plugin/<Name> 虚拟着色器路径映射，必须手动注册。
	// 这一步把虚拟路径 /Plugin/GlobalShaderLab 映射到插件的 Shaders/ 物理目录，
	// 这样 IMPLEMENT_GLOBAL_SHADER 里的 "/Plugin/GlobalShaderLab/Private/xxx.usf" 才能被找到。
	FString PluginShaderDir = FPaths::Combine(
		IPluginManager::Get().FindPlugin(TEXT("GlobalShaderLab"))->GetBaseDir(),
		TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/GlobalShaderLab"), PluginShaderDir);

	UE_LOG(LogGlobalShaderLab, Log, TEXT("GlobalShaderLab 模块已启动 —— Shader 路径映射已注册：%s"), *PluginShaderDir);
}

void FGlobalShaderLabModule::ShutdownModule()
{
	UE_LOG(LogGlobalShaderLab, Log, TEXT("GlobalShaderLab 模块已卸载。"));
}

IMPLEMENT_MODULE(FGlobalShaderLabModule, GlobalShaderLab)
