#ifndef DARTS_ENGINE_CORE_H_
#define DARTS_ENGINE_CORE_H_
#include "Core.h"
#include "Input.h"
#include "Renderer.h"
#include "GUI.h"
#include "Allocator.h"
#include "RootEntity.h"
#include "StackBuffer.h"
#include <utility>
#include <new>

//#define MEM2_ENGINE_ALLOCATION 0
#define MEM2_ENGINE_ALLOCATION 1

class Engine
{
public:
	~Engine();
	WD_NOCOPY(Engine);
	static void Create(int argc, char** argv);
	WD_NODISCARD static Renderer& GetRenderer();
	WD_NODISCARD static Input& GetInput();
	WD_NODISCARD static GUI& GetGUI();

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
	// if possible a StackBuffer should be used instead of this
	WD_NODISCARD static void* AllocateTempUpdateMem(size_t size);

	static void Log(const char* msg);

	static float GetDelta();

	static void GetMainArgs(int* argc, char*** argv);

	template <class RootEntityT, typename ...Args>
	static void SetRootEntity(Args&&... args) {
		WD_STATIC_ASSERT(sizeof(RootEntityT) == sizeof(RootEntity), "Engine::SetRootEntity RootEntityT size was not that of RootEntity");
		new((void*)&(GetEngine().m_rootEntities[GetEngine().m_activeRootEntity ^ 0x01])) RootEntityT(std::forward<Args>(args)...);
		GetEngine().m_switchRootEntity = true;
	}

	static void Start();
	static void Quit();

private:
	friend class StackBuffer;

#if !(MEM2_ENGINE_ALLOCATION)
	static Engine s_engine;
#endif

	Input m_input;
	Renderer m_renderer;
	GUI m_GUI;
	Allocator m_sceneAllocator;
	Allocator m_tempAllocator;
	RootEntity m_rootEntities[2];
	int m_mainArgCount;
	char** m_mainArgValues;
	float m_delta;
	uint8_t m_activeRootEntity;
	bool m_quit;
	bool m_switchRootEntity;

	static void SetupFS();
	static Engine& GetEngine();

	Engine();
	void Init(int argc, char** argv);
	bool SetupAllocators();
	void InternalStart();
	void InternalQuit();
	// Must be called after the scene allocator is reset
	// Ensures allocations make from the tail of the scene allocator are 32 byte aligned
	void AlignSceneTailAllocator();

	static void* StackBufferAllocate(size_t size);
	static void StackBufferRestore(const void* ptr);
};
#endif // !DARTS_ENGINE_CORE_H_
