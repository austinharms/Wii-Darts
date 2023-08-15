#ifndef DARTS_ENGINE_CORE_H_
#define DARTS_ENGINE_CORE_H_
#include "Core.h"
#include "Renderer.h"
#include "Input.h"
#include "Allocator.h"
#include "RootEntity.h"
#include <utility>
#include <new>

enum WDAllocatorEnum
{
	WD_ALLOCATOR_SCENE = 0, // Engine::AllocateSceneMem(size)
	WD_ALLOCATOR_ALIGNED_SCENE, // Engine::AllocateAlignedSceneMem(size)
	WD_ALLOCATOR_UPDATE_TEMP, // Engine::AllocateTempUpdateMem(size)
	WD_ALLOCATOR_TEMP, // Engine::AllocateTempMem(size, false)
	WD_ALLOCATOR_TEMP_KEEP // Engine::AllocateTempMem(size, true)
};

class Engine
{
public:
	~Engine();
	WD_NOCOPY(Engine);
	WD_NODISCARD static Renderer& GetRenderer();
	WD_NODISCARD static Input& GetInput();

	// Allocates memory for scene use
	// this memory will be reset when a new RootEntity is loaded
	// returned pointer should NOT be freed
	WD_NODISCARD static void* AllocateSceneMem(size_t size);

	// Allocates 32byte aligned memory for scene use
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

	// Allocates memory base on the supplied allocator enum
	WD_NODISCARD static void* Allocate(size_t size, WDAllocatorEnum allocator);

	// Reads the file and returns a buffer with the files data
	// if fileSize is not null, sets fileSize to the size in bytes of the returned buffer
	// Returns nullptr on error and fileSize will be unchanged
	WD_NODISCARD static uint8_t* ReadFile(const char* filepath, size_t* fileSize = nullptr, WDAllocatorEnum allocator = WD_ALLOCATOR_TEMP_KEEP);

	template <class RootEntityT, typename ...Args>
	static void SetRootEntity(Args&&... args) {
		WD_STATIC_ASSERT(sizeof(RootEntityT) == sizeof(RootEntity), "Engine::SetRootEntity RootEntityT size was not that of RootEntity");
		new((void*)&(s_engine.m_rootEntities[s_engine.m_activeRootEntity ^ 0x01])) RootEntityT(std::forward<Args>(args)...);
		s_engine.m_switchRootEntity = true;
	}

	static void Start();
	static void Quit();

private:
	static Engine s_engine;

	Renderer m_renderer;
	Input m_input;
	Allocator m_sceneAllocator;
	Allocator m_tempAllocator;
	RootEntity m_rootEntities[2];
	uint8_t m_activeRootEntity;
	bool m_quit;
	bool m_switchRootEntity;

	Engine();
	void InternalStart();
	void InternalQuit();
	// Must be called after the scene allocator is reset
	// Ensures allocations make from the tail of the scene allocator are 32 byte aligned
	void AlignSceneTailAllocator();
};
#endif // !DARTS_ENGINE_CORE_H_
