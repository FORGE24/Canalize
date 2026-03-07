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
// #include "canalize/block/Blocks.h" // Removed as it is part of BlockState.h or doesn't exist as separate file
#include "canalize/world/ChunkAccess.h"
#include "canalize/world/BlockPos.h"
#include "canalize/forge/EventBus.h"
#include "canalize/forge/ChunkEvents.h"
#include "canalize/item/Item.h"
#include "canalize/item/ItemStack.h"
#include "canalize/item/Items.h"
#include "canalize/entity/Entity.h"
#include "canalize/entity/EntityType.h"
#include "canalize/config/Config.h"
#include "canalize/gui/Screen.h"
#include "canalize/network/Packet.h"
#include "canalize/render/RenderSystem.h"

// Macro to define the plugin entry point
#ifdef _WIN32
#define CANALIZE_PLUGIN_ENTRY extern "C" __declspec(dllexport) void CanalizePluginInit(Canalize::PluginManager& manager)
#else
#define CANALIZE_PLUGIN_ENTRY extern "C" __attribute__((visibility("default"))) void CanalizePluginInit(Canalize::PluginManager& manager)
#endif
