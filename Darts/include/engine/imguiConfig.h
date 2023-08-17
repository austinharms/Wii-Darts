#ifndef DARTS_IMGUI_CONFIG_H_
#define DARTS_IMGUI_CONFIG_H_
#include "Engine.h"
#include <cassert>
#define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT struct ImDrawVert { ImVec2 pos; float posZ; ImU32 col; ImVec2 uv; };
#define IM_ASSERT(exp) do { if (!(exp)) { Engine::Log(#exp); } assert(exp); } while(0)
#endif // !DARTS_IMGUI_CONFIG_H_
