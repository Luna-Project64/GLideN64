#include <Config.h>
#include <CRC.h>
#include "GLFunctions.h"
#include "opengl_Attributes.h"
#include "opengl_DynBufferedDrawer.h"

using namespace graphics;
using namespace opengl;

const u32 DynBufferedDrawer::m_bufMaxSize = 64*1024;

extern "C" void __stdcall GL_BufferDataDynamic(GLenum target, GLsizeiptr size, const void* data, GLenum usage);

DynBufferedDrawer::DynBufferedDrawer(CachedVertexAttribArray * _cachedAttribArray, CachedBindBuffer * _bindBuffer)
: m_cachedAttribArray(_cachedAttribArray)
, m_bindBuffer(_bindBuffer)
{
	m_vertices.resize(VERTBUFF_SIZE);
	/* Init buffers for rects */
	glGenVertexArrays(1, &m_rectsBuffers.vao);
	glBindVertexArray(m_rectsBuffers.vao);
	_initBuffer(m_rectsBuffers.vbo, m_bufMaxSize);
	m_cachedAttribArray->enableVertexAttribArray(rectAttrib::position, true);
	m_cachedAttribArray->enableVertexAttribArray(rectAttrib::texcoord0, true);
	m_cachedAttribArray->enableVertexAttribArray(rectAttrib::texcoord1, true);
	glVertexAttribPointer(rectAttrib::position, 4, GL_FLOAT, GL_FALSE, sizeof(RectVertex), (const GLvoid *)(offsetof(RectVertex, x)));
	glVertexAttribPointer(rectAttrib::texcoord0, 2, GL_FLOAT, GL_FALSE, sizeof(RectVertex), (const GLvoid *)(offsetof(RectVertex, s0)));
	glVertexAttribPointer(rectAttrib::texcoord1, 2, GL_FLOAT, GL_FALSE, sizeof(RectVertex), (const GLvoid *)(offsetof(RectVertex, s1)));

	/* Init buffers for triangles */
	glGenVertexArrays(1, &m_trisBuffers.vao);
	glBindVertexArray(m_trisBuffers.vao);
	_initBuffer(m_trisBuffers.vbo, m_bufMaxSize);
	_initBuffer(m_trisBuffers.ebo, m_bufMaxSize);
	m_cachedAttribArray->enableVertexAttribArray(triangleAttrib::position, true);
	m_cachedAttribArray->enableVertexAttribArray(triangleAttrib::color, true);
	m_cachedAttribArray->enableVertexAttribArray(triangleAttrib::texcoord, true);
	m_cachedAttribArray->enableVertexAttribArray(triangleAttrib::modify, true);
	m_cachedAttribArray->enableVertexAttribArray(triangleAttrib::numlights, false);
	glVertexAttribPointer(triangleAttrib::position, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)(offsetof(Vertex, x)));
	glVertexAttribPointer(triangleAttrib::color, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)(offsetof(Vertex, r)));
	glVertexAttribPointer(triangleAttrib::texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)(offsetof(Vertex, s)));
	glVertexAttribPointer(triangleAttrib::modify, 4, GL_BYTE, GL_TRUE, sizeof(Vertex), (const GLvoid *)(offsetof(Vertex, modify)));
}

void DynBufferedDrawer::_initBuffer(Buffer & _buffer, GLuint _bufSize)
{
	_buffer.size = _bufSize;
	glGenBuffers(1, &_buffer.handle);
	m_bindBuffer->bind(Parameter(_buffer.type), ObjectHandle(_buffer.handle));
	GL_BufferDataDynamic(_buffer.type, _bufSize, nullptr, GL_DYNAMIC_DRAW);
}

DynBufferedDrawer::~DynBufferedDrawer()
{
	m_bindBuffer->bind(Parameter(GL_ARRAY_BUFFER), ObjectHandle::null);
	m_bindBuffer->bind(Parameter(GL_ELEMENT_ARRAY_BUFFER), ObjectHandle::null);
	GLuint buffers[3] = { m_rectsBuffers.vbo.handle, m_trisBuffers.vbo.handle, m_trisBuffers.ebo.handle };
	glDeleteBuffers(3, buffers);
	glBindVertexArray(0);
	GLuint arrays[2] = { m_rectsBuffers.vao, m_trisBuffers.vao };
	glDeleteVertexArrays(2, arrays);
}

void DynBufferedDrawer::_updateBuffer(Buffer & _buffer, u32 _count, u32 _dataSize, const void * _data)
{
	m_bindBuffer->bind(Parameter(_buffer.type), ObjectHandle(_buffer.handle));
	void* buffer_pointer = glMapBufferRange(_buffer.type, 0, _dataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	memcpy(buffer_pointer, _data, _dataSize);
	glUnmapBuffer(_buffer.type);
}

void DynBufferedDrawer::_updateRectBuffer(const graphics::Context::DrawRectParameters & _params)
{
	const BuffersType type = BuffersType::rects;
	if (m_type != type) {
		glBindVertexArray(m_rectsBuffers.vao);
		m_type = type;
	}

	Buffer & buffer = m_rectsBuffers.vbo;
	const u32 dataSize = _params.verticesCount * static_cast<u32>(sizeof(RectVertex));

	_updateBuffer(buffer, _params.verticesCount, dataSize, _params.vertices);
}


void DynBufferedDrawer::drawRects(const graphics::Context::DrawRectParameters & _params)
{
	_updateRectBuffer(_params);

	m_cachedAttribArray->enableVertexAttribArray(rectAttrib::texcoord0, _params.texrect);
	m_cachedAttribArray->enableVertexAttribArray(rectAttrib::texcoord1, _params.texrect);

	glDrawArrays(GLenum(_params.mode), 0, _params.verticesCount);
}

void DynBufferedDrawer::_convertFromSPVertex(bool _flatColors, u32 _count, const SPVertex * _data)
{
	if (_count > m_vertices.size())
		m_vertices.resize(_count);

	for (u32 i = 0; i < _count; ++i) {
		const SPVertex & src = _data[i];
		Vertex & dst = m_vertices[i];
		dst.x = src.x;
		dst.y = src.y;
		dst.z = src.z;
		dst.w = src.w;
		if (_flatColors) {
			dst.r = src.flat_r;
			dst.g = src.flat_g;
			dst.b = src.flat_b;
			dst.a = src.flat_a;
		} else {
			dst.r = src.r;
			dst.g = src.g;
			dst.b = src.b;
			dst.a = src.a;
		}
		dst.s = src.s;
		dst.t = src.t;
		dst.modify = src.modify;
	}
}

void DynBufferedDrawer::_updateTrianglesBuffers(const graphics::Context::DrawTriangleParameters & _params)
{
	const BuffersType type = BuffersType::triangles;

	if (m_type != type) {
		glBindVertexArray(m_trisBuffers.vao);
		m_type = type;
	}

	_convertFromSPVertex(_params.flatColors, _params.verticesCount, _params.vertices);
	const u32 vboDataSize = _params.verticesCount * static_cast<u32>(sizeof(Vertex));
	Buffer & vboBuffer = m_trisBuffers.vbo;
	_updateBuffer(vboBuffer, _params.verticesCount, vboDataSize, m_vertices.data());

	if (_params.elements == nullptr)
		return;

	const u32 eboDataSize = static_cast<u32>(sizeof(GLushort)) * _params.elementsCount;
	Buffer & eboBuffer = m_trisBuffers.ebo;
	_updateBuffer(eboBuffer, _params.elementsCount, eboDataSize, _params.elements);
}

void DynBufferedDrawer::drawTriangles(const graphics::Context::DrawTriangleParameters & _params)
{
	_updateTrianglesBuffers(_params);

	if (isHWLightingAllowed())
		glVertexAttrib1f(triangleAttrib::numlights, GLfloat(_params.vertices[0].HWLight));

	if (_params.elements == nullptr) {
		glDrawArrays(GLenum(_params.mode), 0, _params.verticesCount);
		return;
	}

	glDrawElements(GLenum(_params.mode), _params.elementsCount, GL_UNSIGNED_SHORT, 0);
}

void DynBufferedDrawer::drawLine(f32 _width, SPVertex * _vertices)
{
	const BuffersType type = BuffersType::triangles;

	if (m_type != type) {
		glBindVertexArray(m_trisBuffers.vao);
		m_type = type;
	}

	_convertFromSPVertex(false, 2, _vertices);
	const GLsizeiptr vboDataSize = 2 * sizeof(Vertex);
	Buffer & vboBuffer = m_trisBuffers.vbo;
	_updateBuffer(vboBuffer, 2, vboDataSize, m_vertices.data());

	glLineWidth(_width);
	glDrawArrays(GL_LINES, 0, 2);
}
