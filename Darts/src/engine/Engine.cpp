#include "engine/Engine.h"
#include <fstream>
#include <fat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ogc/lwp_watchdog.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <stdio.h>
#include <new>

// Prevent the use of mem2 as we use that for scene and temp allocators
//u32 MALLOC_MEM2 = 0;
#if MEM2_ENGINE_ALLOCATION
Engine* g_engine = nullptr;
#else
Engine Engine::s_engine;
#endif

void WiiResetCallback(u32 irq, void* ctx) { Engine::Quit(); }
void WiiPowerCallback() { Engine::Quit(); }
void WiimotePowerCallback(int32_t chan) { Engine::Quit(); }

Engine::Engine() {
	m_quit = true;
	m_switchRootEntity = false;
	m_activeRootEntity = 0;
	m_mainArgCount = 0;
	m_mainArgValues = nullptr;
	m_delta = 0;
}

Engine::~Engine() {

}

Engine& Engine::GetEngine() {
#if MEM2_ENGINE_ALLOCATION
	return *g_engine;
#else
	return s_engine;
#endif
}

void Engine::Create(int argc, char** argv)
{
	SetupFS();
#if MEM2_ENGINE_ALLOCATION
	uint8_t* mem2Start = ((uint8_t*)SYS_GetArena2Lo());
	// Discard an extra 100000 bytes on the top of mem2 as this seems to cause errors when written to
	uint8_t* mem2End = ((uint8_t*)SYS_GetArena2Hi()) - 100000;
	if ((size_t)mem2End - (size_t)mem2Start < sizeof(Engine)) {
		Engine::Log("Failed to create Engine, MEM2 not big enough");
		exit(1);
	}

	SYS_SetArena2Lo(mem2Start + sizeof(Engine));
	g_engine = (Engine*)mem2Start;
	new(g_engine) Engine();
	Engine::Log("Engine Created");
	GetEngine().Init(argc, argv);
#else
	GetEngine().Init(argc, argv);
#endif
}

void Engine::Init(int argc, char** argv) {
	m_mainArgCount = argc;
	m_mainArgValues = argv;
	settime((uint64_t)0);
	if (!SetupAllocators()) {
		Engine::Log("Failed to setup allocators");
		Engine::Quit();
		// the reset of setup depends on the allocators, so abort setup
		exit(1);
	}

	m_renderer.Init();
	m_input.Init();
	m_GUI.Init();
	SYS_SetResetCallback(WiiResetCallback);
	SYS_SetPowerCallback(WiiPowerCallback);
	WPAD_SetPowerButtonCallback(WiimotePowerCallback);
	Log("Engine Init");
}

WD_NODISCARD Renderer& Engine::GetRenderer()
{
	return GetEngine().m_renderer;
}

WD_NODISCARD Input& Engine::GetInput()
{
	return GetEngine().m_input;
}

WD_NODISCARD void* Engine::AllocateSceneMem(size_t size)
{
	return GetEngine().m_sceneAllocator.Allocate(size);
}

WD_NODISCARD void* Engine::AllocateAlignedSceneMem(size_t size)
{
	// Ensure size is in 32 byte sized blocks
	size += size % 32;
	return GetEngine().m_sceneAllocator.AllocateTail(size);
}

WD_NODISCARD void* Engine::AllocateTempUpdateMem(size_t size)
{
	return GetEngine().m_tempAllocator.AllocateTail(size);;
}

void Engine::Start()
{
	GetEngine().InternalStart();
}

void Engine::Quit()
{
	GetEngine().InternalQuit();
}

void Engine::Log(const char* msg)
{
	std::ofstream file("sd:/log.txt", std::ios_base::app);
	file << msg << "\n";
	file.flush();
}

float Engine::GetDelta()
{
	return GetEngine().m_delta;
}

void Engine::GetMainArgs(int* argc, char*** argv)
{
	if (argc) *argc = GetEngine().m_mainArgCount;
	if (argv) *argv = GetEngine().m_mainArgValues;
}

void Engine::InternalStart()
{
	// If the default root entity has not changed don't start
	if (!m_switchRootEntity) return;
	m_quit = false;
	uint64_t lastDelta = gettime();
	while (!m_quit) {
		m_tempAllocator.ClearAllocations();
		m_delta = (float)(gettime() - lastDelta) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
		lastDelta = gettime();

		if (m_switchRootEntity) {
			m_switchRootEntity = false;
			m_renderer.Disable();
			m_rootEntities[m_activeRootEntity].Unload();
			m_rootEntities[m_activeRootEntity].~RootEntity();
			m_activeRootEntity ^= 0x01;
			m_sceneAllocator.ClearAllocations();
			AlignSceneTailAllocator();
			m_GUI.UpdateFontAtlas();
			m_rootEntities[m_activeRootEntity].Load();
			m_renderer.Enable();
			m_tempAllocator.ClearAllocations();
		}

		m_input.PollEvents();
		m_GUI.ProcessEvents();
		if (m_GUI.GetUsingInput())
			m_input.DisableInputs();
		m_rootEntities[m_activeRootEntity].Update();
		m_renderer.StartFrame();
		m_GUI.StartFrame();
		m_rootEntities[m_activeRootEntity].Render();
		m_input.EnableInputs();

		orient_t ori;
		gforce_t gf;
		vec3w_t acc;
		WPAD_Orientation(0, &ori);
		WPAD_GForce(0, &gf);
		WPAD_Accel(0, &acc);

		ImGui::Begin("Wiimote Data");
		ImGui::Text("Orient: Pitch: %f Yaw: %f Roll: %f", ori.pitch, ori.yaw, ori.roll);
		ImGui::Text("GForce: X: %f Y: %f Z: %f", gf.x, gf.y, gf.z);
		ImGui::Text("Accel: X: %f Y: %f Z: %f", acc.x, acc.y, acc.z);
		ImGui::End();

		m_GUI.RenderUI();
		m_renderer.EndFrame();
	}

	m_renderer.Disable();
	m_rootEntities[m_activeRootEntity].Unload();
	m_rootEntities[m_activeRootEntity].~RootEntity();
	Log("Engine Exit");
}

void Engine::InternalQuit()
{
	m_quit = true;
}

void Engine::AlignSceneTailAllocator()
{
	// Allocate a 0 byte block to get current alignment
	size_t currentTail = (size_t)m_sceneAllocator.AllocateTail(0);
	// Allocate and waste the required amount of bytes for the next allocation to be aligned
	m_sceneAllocator.AllocateTail(currentTail % 32);
}

void Engine::SetupFS()
{
	fatInitDefault();
	std::ofstream file("sd:/log.txt");
	file << "FS Init\n";
	file.flush();

	//if (chdir("sd:/apps/darts/") != 0) GetRenderer().SetClearColor(0x00ff00ff);
	//if ((_cwd = getcwd(nullptr, 0)) == nullptr) Logger::fatal("Failed to get working directory");
}

WD_NODISCARD bool Engine::SetupAllocators()
{
	constexpr size_t SceneMemSize = 32000000;
	constexpr size_t TempMemSize = 8000000;
	constexpr size_t RequiredMem2Size = SceneMemSize + TempMemSize;
	uint8_t* mem2Start = ((uint8_t*)SYS_GetArena2Lo());
	// Discard an extra 100000 bytes on the top of mem2 as this seems to cause errors when written to
	uint8_t* mem2End = ((uint8_t*)SYS_GetArena2Hi()) - 100000;

	if ((size_t)mem2End - (size_t)mem2Start < RequiredMem2Size) {
		Engine::Log("Not enough memory, less then 40MB of mem2");
		return false;
	}

	// Use 32MB for scene allocator
	uint8_t* sceneEnd = mem2Start + SceneMemSize;
	m_sceneAllocator.Init(mem2Start, sceneEnd);
	// Use 8MB for temp allocator
	uint8_t* tempEnd = sceneEnd + TempMemSize;
	m_tempAllocator.Init(sceneEnd, tempEnd);
	SYS_SetArena2Lo(tempEnd);

	char* buf = (char*)m_tempAllocator.Allocate(512);
	sprintf(buf, "Allocated Engine Allocators\n\tMEM2:\n\t\tSize: %i\n\t\tStart: %08X\n\t\tEnd: %08X\n\tTemp:\n\t\tSize: %i\n\t\tStart: %08X\n\t\tEnd: %08X\n\tScene:\n\t\tSize: %i\n\t\tStart: %08X\n\t\tEnd: %08X\n\tMEM2 malloc:\n\t\tSize: %i\n\t\tStart: %08X\n\t\tEnd: %08X", (size_t)mem2End - (size_t)mem2Start, (size_t)mem2Start, (size_t)mem2End, m_tempAllocator.GetStackSize(), (size_t)sceneEnd, (size_t)tempEnd, m_sceneAllocator.GetStackSize(), (size_t)mem2Start, (size_t)sceneEnd, (size_t)mem2End - (size_t)tempEnd, (size_t)tempEnd, (size_t)mem2End);
	Engine::Log(buf);
	m_tempAllocator.ClearAllocations();
	return true;
}

void* Engine::StackBufferAllocate(size_t size)
{
	return GetEngine().m_tempAllocator.Allocate(size);
}

void Engine::StackBufferRestore(const void* ptr)
{
	if (ptr) GetEngine().m_tempAllocator.RestoreHead((void*)ptr);
}
