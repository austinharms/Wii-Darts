#include "EngineCore.h"

using namespace darts;

EngineCore EngineCore::s_engine;

EngineCore::EngineCore() {
	m_stopFlag = true;
}

EngineCore::~EngineCore() {

}

WD_NODISCARD Renderer& EngineCore::GetRenderer()
{
	return s_engine.m_renderer;
}

WD_NODISCARD Input& EngineCore::GetInput()
{
	return s_engine.m_input;
}

void EngineCore::Start()
{
	s_engine.InternalStart();
}

void EngineCore::Stop()
{
	s_engine.InternalStop();
}

void EngineCore::InternalStart()
{
	m_stopFlag = false;
	m_renderer.Enabled();
	while (!m_stopFlag) {
		m_input.PollEvents();
		m_renderer.DrawTestRect();
		m_renderer.SwapBuffers();
	}

	m_renderer.Disable();
}

void EngineCore::InternalStop()
{
	m_stopFlag = true;
}
