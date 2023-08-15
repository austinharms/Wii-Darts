#ifndef DARTS_ENGINE_CORE_H_
#define DARTS_ENGINE_CORE_H_
#include "Core.h"
#include "Renderer.h"
#include "Input.h"
#include "Allocator.h"
#include "RootEntity.h"
#include <utility>
#include <new>

namespace darts {
	class EngineCore
	{
	public:
		~EngineCore();
		WD_NOCOPY(EngineCore);
		WD_NODISCARD static Renderer& GetRenderer();
		WD_NODISCARD static Input& GetInput();
		// Allocates memory for scene use
		// this memory will be reset when a new RootEntity is loaded
		// returned pointer should NOT be freed
		WD_NODISCARD static void* AllocateSceneMem(size_t size);

		// Allocates memory for scene use
		// this memory will be reset when a new RootEntity is loaded
		// returned pointer should NOT be freed
		WD_NODISCARD static void* AllocateAlignedSceneMem(size_t size);

		// Allocates memory for temporary uses
		// the allocated memory is valid until the next start of the next update 
		// returned pointer should NOT be freed
		WD_NODISCARD static void* AllocateTempUpdateMem(size_t size);
		// Allocates memory for very temporary use
		// memory allocated will be invalidated by the next call to AllocateTempMem unless keepLastAllocation is true
		// returned pointer should NOT be freed
		WD_NODISCARD static void* AllocateTempMem(size_t size, bool keepLastAllocation = false);

		template <class RootEntityT, typename ...Args>
		static void SetRootEntity(Args&&... args) {
			WD_STATIC_ASSERT(sizeof(RootEntityT) == sizeof(RootEntity), "EngineCore::SetRootEntity RootEntityT size was not that of RootEntity");
			new((void*)&(s_engine.m_rootEntities[s_engine.m_activeRootEntity ^ 0x01])) RootEntityT(std::forward<Args>(args)...);
			s_engine.m_switchRootEntity = true;
		}

		static void Start();
		static void Quit();

	private:
		static EngineCore s_engine;

		Renderer m_renderer;
		Input m_input;
		Allocator m_sceneAllocator;
		Allocator m_tempAllocator;
		RootEntity m_rootEntities[2];
		uint8_t m_activeRootEntity;
		bool m_quit;
		bool m_switchRootEntity;

		EngineCore();
		void InternalStart();
		void InternalQuit();
	};
}
#endif // !DARTS_ENGINE_CORE_H_
