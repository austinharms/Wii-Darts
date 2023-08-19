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

// Prevent the use of mem2 as we use that for scene and temp allocators
u32 MALLOC_MEM2 = 0;

Engine Engine::s_engine;

void WiiResetCallback(u32 irq, void* ctx) { Engine::Quit(); }
void WiiPowerCallback() { Engine::Quit(); }
void WiimotePowerCallback(int32_t chan) { Engine::Quit(); }

Engine::Engine() {
	m_quit = true;
	m_switchRootEntity = false;
	m_activeRootEntity = 0;
}

Engine::~Engine() {

}

WD_NODISCARD Renderer& Engine::GetRenderer()
{
	return s_engine.m_renderer;
}

WD_NODISCARD Input& Engine::GetInput()
{
	return s_engine.m_input;
}

WD_NODISCARD void* Engine::AllocateSceneMem(size_t size)
{
	return s_engine.m_sceneAllocator.Allocate(size);
}

WD_NODISCARD void* Engine::AllocateAlignedSceneMem(size_t size)
{
	// Ensure size is in 32 byte sized blocks
	size += size % 32;
	return s_engine.m_sceneAllocator.AllocateTail(size);
}

WD_NODISCARD void* Engine::AllocateTempUpdateMem(size_t size)
{
	return s_engine.m_tempAllocator.AllocateTail(size);;
}

void Engine::Start(int argc, char** argv)
{
	s_engine.InternalStart(argc, argv);
}

void Engine::Quit()
{
	s_engine.InternalQuit();
}

void Engine::Log(const char* msg)
{
	std::ofstream file("sd:/log.txt", std::ios_base::app);
	file << msg << "\n";
	file.flush();
}

float Engine::GetDelta()
{
	return s_engine.m_delta;
}

void Engine::GetMainArgs(int* argc, char*** argv)
{
	if (argc) *argc = s_engine.m_mainArgCount;
	if (argv) *argv = s_engine.m_mainArgValues;
}
 
void Engine::Init() {
	SetupFS();
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

void Engine::InternalStart(int argc, char** argv)
{
	m_mainArgCount = argc;
	m_mainArgValues = argv;

	// If the default root entity has not changed don't start
	if (!m_switchRootEntity) return;
	m_quit = false;
	Init();
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
		m_rootEntities[m_activeRootEntity].Update();
		m_renderer.StartFrame();
		m_GUI.StartFrame();
		m_rootEntities[m_activeRootEntity].Render();
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
	if (!fatInitDefault()) {
		GetRenderer().SetClearColor(0xff0000ff);
	}

	std::ofstream file("sd:/log.txt");
	file << "FS Init\n";
	file.flush();

	//if (chdir("sd:/apps/darts/") != 0) GetRenderer().SetClearColor(0x00ff00ff);
	//if ((_cwd = getcwd(nullptr, 0)) == nullptr) Logger::fatal("Failed to get working directory");
}

WD_NODISCARD bool Engine::SetupAllocators()
{
	void* startOfMem2 = ((uint8_t*)SYS_GetArena2Lo());
	// Discard an extra 100000 bytes on the top of mem2 as this seems to cause errors when written to
	void* endOfMem2 = ((uint8_t*)SYS_GetArena2Hi()) - 100000;

	if ((size_t)endOfMem2 - (size_t)startOfMem2 < 20000000) {
		Engine::Log("Not enough memory, less then 20MB of mem2");
		return false;
	}

	// Use 8MB for temp storage and the rest for scene mem
	size_t tempAllocatorSize = 8000000;
	void* tempEnd = (void*)(((uint8_t*)startOfMem2) + tempAllocatorSize);
	m_tempAllocator.Init(startOfMem2, tempEnd);
	m_sceneAllocator.Init(tempEnd, endOfMem2);
	char* buf = (char*)m_tempAllocator.Allocate(256);
	sprintf(buf, "Allocated Allocators\n\tMEM2:\n\t\tSize: %i\n\t\tStart: %08X\n\t\tEnd: %08X\n\tTemp:\n\t\tSize: %i\n\t\tStart: %08X\n\t\tEnd: %08X\n\tScene:\n\t\tSize: %i\n\t\tStart: %08X\n\t\tEnd: %08X", (size_t)endOfMem2 - (size_t)startOfMem2, (size_t)startOfMem2, (size_t)endOfMem2, m_tempAllocator.GetStackSize(), (size_t)startOfMem2, (size_t)tempEnd, m_sceneAllocator.GetStackSize(), (size_t)tempEnd, (size_t)endOfMem2);
	Engine::Log(buf);
	m_tempAllocator.ClearAllocations();
	return true;
}

void* Engine::StackBufferAllocate(size_t size)
{
	return s_engine.m_tempAllocator.Allocate(size);
}

void Engine::StackBufferRestore(const void* ptr)
{
	if (ptr) s_engine.m_tempAllocator.RestoreHead((void*)ptr);
}
