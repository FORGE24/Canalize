#pragma once

#include "CanalizeAPI.h"
#include "PluginSystem.h"
#include "TerrainGen.h"
#include "WorldLoader.h"
#include "NativeLog.h"
#include "MathUtil.h"

// NeoForge-like APIs
#include "canalize/api/JNIHelper.h"
#include "canalize/block/BlockState.h"
#include "canalize/world/ChunkAccess.h"
#include "canalize/forge/EventBus.h"
#include "canalize/forge/ChunkEvents.h"

// Macro to define the plugin entry point
#ifdef _WIN32
#define CANALIZE_PLUGIN_ENTRY extern "C" __declspec(dllexport) void CanalizePluginInit(Canalize::PluginManager& manager)
#else
#define CANALIZE_PLUGIN_ENTRY extern "C" __attribute__((visibility("default"))) void CanalizePluginInit(Canalize::PluginManager& manager)
#endif
