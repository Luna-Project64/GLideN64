#include "GLFunctions.h"
#include "opengl_GLInfo.h"
#include "opengl_CachedFunctions.h"

using namespace graphics;
using namespace opengl;

/*---------------CachedEnable-------------*/

CachedEnable::CachedEnable(Parameter _parameter)
: m_parameter(_parameter)
{
}

void CachedEnable::enable(bool _enable)
{
	if (!m_parameter.isValid())
		return;
	// TODO make cacheable
	if (!update(u32(_enable)))
		return;

	if (_enable) {
		glEnable(GLenum(m_parameter));
	} else {
		glDisable(GLenum(m_parameter));
	}
}

/*---------------CachedBindTexture-------------*/

void CachedBindTexture::bind(Parameter _target, ObjectHandle _name)
{
	if (update(_name))
		glBindTexture(GLenum(_target), GLuint(_name));
}

/*---------------CachedActiveTexture-------------*/

void CachedActiveTexture::setActiveTexture(Parameter _index) {
	if (update(_index))
		glActiveTexture(GL_TEXTURE0 + GLuint(_index));
}

/*---------------CachedCullFace-------------*/

void CachedCullFace::setCullFace(Parameter _mode)
{
	if (update(_mode))
		glCullFace(GLenum(_mode));
}

/*---------------CachedDepthMask-------------*/

void CachedDepthMask::setDepthMask(bool _enable)
{
	if (update(Parameter(u32(_enable))))
		glDepthMask(GLboolean(_enable));
}

/*---------------CachedDepthMask-------------*/

void CachedDepthCompare::setDepthCompare(Parameter _mode)
{
	if (update(_mode))
		glDepthFunc(GLenum(_mode));
}

/*---------------CachedViewport-------------*/

void CachedViewport::setViewport(s32 _x, s32 _y, s32 _width, s32 _height)
{
	if (update(Parameter(_x), Parameter(_y), Parameter(_width), Parameter(_height)))
		glViewport(_x, _y, _width, _height);
}

/*---------------CachedScissor-------------*/

void CachedScissor::setScissor(s32 _x, s32 _y, s32 _width, s32 _height)
{
	if (update(Parameter(_x), Parameter(_y), Parameter(_width), Parameter(_height)))
		glScissor(_x, _y, _width, _height);
}

/*---------------CachedBlending-------------*/

void CachedBlending::setBlending(Parameter _sfactor, Parameter _dfactor)
{
	if (update(_sfactor, _dfactor))
		glBlendFunc(GLenum(_sfactor), GLenum(_dfactor));
}

/*---------------CachedBlendColor-------------*/

void CachedBlendColor::setBlendColor(f32 _red, f32 _green, f32 _blue, f32 _alpha)
{
	if (update(Parameter(_red), Parameter(_green), Parameter(_blue), Parameter(_alpha)))
		glBlendColor(_red, _green, _blue, _alpha);
}

/*---------------CachedClearColor-------------*/

void CachedClearColor::setClearColor(f32 _red, f32 _green, f32 _blue, f32 _alpha)
{
	if (update(Parameter(_red), Parameter(_green), Parameter(_blue), Parameter(_alpha)))
		glClearColor(_red, _green, _blue, _alpha);
}

/*---------------CachedVertexAttribArray-------------*/

void CachedVertexAttribArray::enableVertexAttribArray(u32 _index, bool _enable)
{
	if (m_attribs[_index] == Parameter(u32(_enable)))
		return;
	m_attribs[_index] = Parameter(u32(_enable));

	if (_enable)
		glEnableVertexAttribArray(_index);
	else
		glDisableVertexAttribArray(_index);
}

void CachedVertexAttribArray::reset()
{
	m_attribs.fill(Parameter());
}

/*---------------CachedFunctions-------------*/

CachedFunctions::CachedFunctions(const GLInfo & _glinfo)
: m_bindFramebuffer(glBindFramebuffer)
, m_bindRenderbuffer(glBindRenderbuffer)
, m_bindBuffer(glBindBuffer) {
	if (_glinfo.isGLESX) {
		// Disable parameters, not avalible for GLESX
		m_enables.emplace(GL_DEPTH_CLAMP, Parameter());
	}
}

CachedFunctions::~CachedFunctions()
{
}

void CachedFunctions::reset()
{
	for (auto it : m_enables)
		it.second.reset();

	m_activeTexture.reset();
	m_bindTexture.reset();
	m_bindFramebuffer.reset();
	m_bindRenderbuffer.reset();
	m_bindBuffer.reset();
	m_cullFace.reset();
	m_depthMask.reset();
	m_depthCompare.reset();
	m_viewport.reset();
	m_scissor.reset();
	m_blending.reset();
	m_blendColor.reset();
	m_clearColor.reset();
	m_attribArray.reset();
}

CachedEnable * CachedFunctions::getCachedEnable(Parameter _parameter)
{
	const u32 key(_parameter);
	auto it = m_enables.find(key);
	if (it == m_enables.end()) {
		auto res = m_enables.emplace(key, _parameter);
		if (res.second)
			return &(res.first->second);
		return nullptr;
	}
	return &(it->second);
}

CachedBindTexture * CachedFunctions::getCachedBindTexture()
{
	return &m_bindTexture;
}

CachedActiveTexture * CachedFunctions::geCachedActiveTexture()
{
	return &m_activeTexture;
}

CachedBindFramebuffer * CachedFunctions::geCachedBindFramebuffer()
{
	return &m_bindFramebuffer;
}

CachedBindRenderbuffer * CachedFunctions::geCachedBindRenderbuffer()
{
	return &m_bindRenderbuffer;
}

CachedBindBuffer * CachedFunctions::geCachedBindBuffer()
{
	return &m_bindBuffer;
}

CachedCullFace * CachedFunctions::getCachedCullFace()
{
	return &m_cullFace;
}

CachedDepthMask * CachedFunctions::getCachedDepthMask()
{
	return &m_depthMask;
}

CachedDepthCompare * CachedFunctions::getCachedDepthCompare()
{
	return &m_depthCompare;
}

CachedViewport * CachedFunctions::getCachedViewport()
{
	return &m_viewport;
}

CachedScissor * CachedFunctions::getCachedScissor()
{
	return &m_scissor;
}

CachedBlending * CachedFunctions::getCachedBlending()
{
	return &m_blending;
}

CachedBlendColor * CachedFunctions::getCachedBlendColor()
{
	return &m_blendColor;
}

CachedClearColor * CachedFunctions::getCachedClearColor()
{
	return &m_clearColor;
}


CachedVertexAttribArray * CachedFunctions::getCachedVertexAttribArray()
{
	return &m_attribArray;
}
