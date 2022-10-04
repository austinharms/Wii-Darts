#include <stdint.h>
#include <gccore.h>
#include <stack>
#include <list>

#include "Transform.h"
#include "Mesh.h"

#ifndef WIIDARTS_RENDERER_H_
#define WIIDARTS_RENDERER_H_
namespace wiidarts {
	class Renderer
	{
	public:
		static const uint32_t RGBA_COLOR_WHITE = 0xffffffff;
		static Renderer& getInstance()
		{
			static Renderer instance;
			return instance;
		}

		~Renderer();
		void swapFrameBuffers();
		void pushTransform(const Transform& t);
		void popTransform();
		void drawMesh3D(const Mesh& mesh, const uint32_t color = RGBA_COLOR_WHITE);
		void drawMesh2D(const Mesh& mesh, const uint32_t color = RGBA_COLOR_WHITE);
		void drawMesh3D(const Mesh& mesh, const Transform& transform, const uint32_t color = RGBA_COLOR_WHITE);
		void drawMesh2D(const Mesh& mesh, const Transform& transform, const uint32_t color = RGBA_COLOR_WHITE);
		void drawMeshIdentity3D(const Mesh& mesh, const uint32_t color = RGBA_COLOR_WHITE);
		void drawMeshIdentity2D(const Mesh& mesh, const uint32_t color = RGBA_COLOR_WHITE);
		uint16_t getScreenWidth() const;
		uint16_t getScreenHeight() const;
		GXRModeObj* getMode() const;

	private:
		const static uint32_t DEFAULT_FIFO_SIZE = 256 * 1024;

		GXRModeObj* _vmode;
		void* _frameBuffers[2];
		void* _fifoBuffer;
		std::stack<Transform, std::list<Transform>> _transformStack;
		Mtx44 _perspectiveMatrix;
		Mtx44 _orthographicMatrix;
		Transform _cameraTransform;
		Transform _2DTransform;
		uint8_t _activeFramebuffer;
		uint8_t _currentProjection;
		uint8_t _currentPositionMatrix;
		bool _transformStackChanged;

		Renderer();
		void resetTransformStack();
		void setPositionMatrix(uint32_t pnidx);
		void setProjectionMatrix(uint8_t projection);
		void setTransformStackPositionMatrix();
		void setIdentityPositionMatrix();
		void setOtherPositionMatrix(const Transform& transform);
		void drawRawMesh3D(const Mesh& mesh, const uint32_t color);
		void drawRawMesh2D(const Mesh& mesh, const uint32_t color);
		void drawMesh(const Mesh& mesh, const uint32_t color);
		Renderer(const Renderer&);
		void operator=(const Renderer&);
	};
}
#endif // !WIIDARTS_RENDERER_H_
