#include "GlobalShaderPlugin.h"
#include "Modules/ModuleManager.h"

// 注册工程的主游戏模块。这里用默认实现，逻辑都放在插件里。
IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, GlobalShaderPlugin, "GlobalShaderPlugin" );
