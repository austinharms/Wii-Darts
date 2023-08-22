#ifndef DARTS_IMGUI_CONFIG_H_
#define DARTS_IMGUI_CONFIG_H_
extern void ImGuiAssertCallback(const char* exp);
#define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT struct ImDrawVert { ImVec2 pos; float posZ; ImU32 col; ImVec2 uv; };
#define IM_ASSERT(exp) do { if (!(exp)) { ImGuiAssertCallback(#exp); } } while(0)
#define IMGUI_DEFINE_MATH_OPERATORS

// Custom color shift to make color format compatible 
#define IM_COL32_R_SHIFT    24
#define IM_COL32_G_SHIFT    16
#define IM_COL32_B_SHIFT    8
#define IM_COL32_A_SHIFT    0
#define IM_COL32_A_MASK     0x000000FF
#endif // !DARTS_IMGUI_CONFIG_H_
