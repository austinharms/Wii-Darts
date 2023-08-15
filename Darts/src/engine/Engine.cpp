#include "Engine/Engine.h"

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

void Engine::Start()
{
	s_engine.InternalStart();
}

void Engine::Quit()
{
	s_engine.InternalQuit();
}

void Engine::InternalStart()
{
	// If the default root entity has not changed don't start
	if (!m_switchRootEntity) return;

	m_quit = false;
	while (!m_quit) {
		m_tempAllocator.ClearAllocations();

		if (m_switchRootEntity) {
			m_switchRootEntity = false;
			m_renderer.Disable();
			m_rootEntities[m_activeRootEntity].Unload();
			m_rootEntities[m_activeRootEntity].~RootEntity();
			m_activeRootEntity ^= 0x01;
			m_sceneAllocator.ClearAllocations();
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
