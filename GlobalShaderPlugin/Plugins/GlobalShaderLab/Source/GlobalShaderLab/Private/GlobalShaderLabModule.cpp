#include "GlobalShaderLabModule.h"

DEFINE_LOG_CATEGORY_STATIC(LogGlobalShaderLab, Log, All);

void FGlobalShaderLabModule::StartupModule()
{
	UE_LOG(LogGlobalShaderLab, Log, TEXT("GlobalShaderLab 模块已启动 —— FGlobalShader 类型已注册到引擎。"));
}

void FGlobalShaderLabModule::ShutdownModule()
{
	UE_LOG(LogGlobalShaderLab, Log, TEXT("GlobalShaderLab 模块已卸载。"));
}

IMPLEMENT_MODULE(FGlobalShaderLabModule, GlobalShaderLab)
