#include "engine/Engine.h"
#include <fstream>
#include <fat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

Engine Engine::s_engine;

Engine::Engine() : m_sceneAllocator(10000000), m_tempAllocator(10000000) {
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
	return s_engine.m_tempAllocator.Allocate(size);;
}

WD_NODISCARD void* Engine::AllocateTempMem(size_t size, bool keepLastAllocation)
{
	if (!keepLastAllocation) s_engine.m_tempAllocator.ClearTail();
	return s_engine.m_tempAllocator.AllocateTail(size);
}

void Engine::Start()
{
	s_engine.InternalStart();
}

void Engine::Quit()
{
	s_engine.InternalQuit();
}

WD_NODISCARD void* Engine::Allocate(size_t size, WDAllocatorEnum allocator)
{
	switch (allocator) {
	case WD_ALLOCATOR_SCENE: return AllocateSceneMem(size);
	case WD_ALLOCATOR_ALIGNED_SCENE: return AllocateAlignedSceneMem(size);
	case WD_ALLOCATOR_UPDATE_TEMP: return AllocateTempUpdateMem(size);
	case WD_ALLOCATOR_TEMP: return AllocateTempMem(size, false);
	case WD_ALLOCATOR_TEMP_KEEP: return AllocateTempMem(size, true);
	default: return nullptr;
	}
}

WD_NODISCARD uint8_t* Engine::ReadFile(const char* filepath, size_t* fileSize, WDAllocatorEnum allocator) {
	std::ifstream file(filepath, std::ios::binary);
	file.seekg(0, std::ios::end);
	int32_t endPos = file.tellg();
	if (endPos <= 0) return nullptr;
	file.seekg(0, std::ios::beg);
	uint8_t* buf = (uint8_t*)Allocate(endPos, allocator);
	if (!buf) return nullptr;
	file.read((char*)buf, endPos);
	if (fileSize) *fileSize = (uint32_t)endPos;
	return buf;
}

void Engine::InternalStart()
{
	// If the default root entity has not changed don't start
	if (!m_switchRootEntity) return;
	m_quit = false;
	SetupFS();
	while (!m_quit) {
		m_tempAllocator.ClearAllocations();

		if (m_switchRootEntity) {
			m_switchRootEntity = false;
			m_renderer.Disable();
			m_rootEntities[m_activeRootEntity].Unload();
			m_rootEntities[m_activeRootEntity].~RootEntity();
			m_activeRootEntity ^= 0x01;
			m_sceneAllocator.ClearAllocations();
			AlignSceneTailAllocator();
			m_rootEntities[m_activeRootEntity].Load();
			m_renderer.Enable();
			m_tempAllocator.ClearAllocations();
		}

		m_input.PollEvents();
		m_rootEntities[m_activeRootEntity].Update();
		m_renderer.StartFrame();
		m_rootEntities[m_activeRootEntity].Render();
		m_renderer.EndFrame();
	}

	m_renderer.Disable();
	m_rootEntities[m_activeRootEntity].Unload();
	m_rootEntities[m_activeRootEntity].~RootEntity();
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
		Quit();
	}

	//if (chdir("sd:/apps/darts/") != 0) GetRenderer().SetClearColor(0x00ff00ff);
	//if ((_cwd = getcwd(nullptr, 0)) == nullptr) Logger::fatal("Failed to get working directory");
}
