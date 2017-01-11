#pragma once
#include <array>
#include <Graphics/GraphicsDrawerImpl.h>
#include "opengl_GLInfo.h"

namespace opengl {
	class CachedVertexAttribArray;

	class UnbufferedDrawer : public graphics::DrawerImpl
	{
	public:
		UnbufferedDrawer(const GLInfo & _glinfo, CachedVertexAttribArray * _cachedAttribArray);
		~UnbufferedDrawer();

		void drawTriangles(const DrawTriangleParameters & _params) override;

		void drawRects(const DrawRectParameters & _params) override;

		void drawLine(f32 _width, SPVertex * _vertices) override;

	private:
		bool _updateAttribPointer(u32 _index, const void * _ptr);

		const GLInfo & m_glInfo;
		CachedVertexAttribArray * m_cachedAttribArray;
		std::array<const void*, MaxAttribIndex> m_attribsData;
	};

}
