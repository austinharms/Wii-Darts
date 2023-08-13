#ifndef DARTS_ENGINE_CORE_H_
#define DARTS_ENGINE_CORE_H_
#include "Core.h"
#include "Renderer.h"
#include "Input.h"

namespace darts {
	class EngineCore
	{
	public:
		~EngineCore();
		WD_NOCOPY(EngineCore);
		WD_NODISCARD static Renderer& GetRenderer();
		WD_NODISCARD static Input& GetInput();
		static void Start();
		static void Stop();

	private:
		static EngineCore s_engine;

		Renderer m_renderer;
		Input m_input;
		bool m_stopFlag;

		EngineCore();
		void InternalStart();
		void InternalStop();
	};
}
#endif // !DARTS_ENGINE_CORE_H_
