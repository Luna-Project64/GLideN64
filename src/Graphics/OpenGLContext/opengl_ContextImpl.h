#pragma once
#include <memory>
#include <Graphics/ContextImpl.h>
#include "opengl_TextureManipulationObjectFactory.h"
#include "opengl_BufferManipulationObjectFactory.h"
#include "opengl_GLInfo.h"
#include "opengl_CachedFunctions.h"

namespace glsl {
	class CombinerProgramBuilder;
}

namespace opengl {

	class ContextImpl : public graphics::ContextImpl
	{
	public:
		ContextImpl();
		~ContextImpl();

		void init() override;

		void destroy() override;

		void enable(graphics::Parameter _parameter, bool _enable) override;

		void cullFace(graphics::Parameter _mode) override;

		void enableDepthWrite(bool _enable) override;

		void setDepthCompare(graphics::Parameter _mode) override;

		void setViewport(s32 _x, s32 _y, s32 _width, s32 _height) override;

		void setScissor(s32 _x, s32 _y, s32 _width, s32 _height) override;

		void setBlending(graphics::Parameter _sfactor, graphics::Parameter _dfactor) override;

		void setBlendColor(f32 _red, f32 _green, f32 _blue, f32 _alpha) override;

		void clearColorBuffer(f32 _red, f32 _green, f32 _blue, f32 _alpha) override;

		void clearDepthBuffer() override;

		graphics::ObjectHandle createTexture(graphics::Parameter _target) override;

		void deleteTexture(graphics::ObjectHandle _name) override;

		void init2DTexture(const graphics::Context::InitTextureParams & _params) override;

		void update2DTexture(const graphics::Context::UpdateTextureDataParams & _params) override;

		void setTextureParameters(const graphics::Context::TexParameters & _parameters) override;

		graphics::ObjectHandle createFramebuffer() override;

		void deleteFramebuffer(graphics::ObjectHandle _name) override;

		graphics::ObjectHandle createRenderbuffer() override;

		void initRenderbuffer(const graphics::Context::InitRenderbufferParams & _params) override;

		void addFrameBufferRenderTarget(const graphics::Context::FrameBufferRenderTarget & _params) override;

		graphics::PixelWriteBuffer * createPixelWriteBuffer(size_t _sizeInBytes) override;

		graphics::CombinerProgram * createCombinerProgram(Combiner & _color, Combiner & _alpha, const CombinerKey & _key) override;

		bool saveShadersStorage(const graphics::Combiners & _combiners) override;

		bool loadShadersStorage(graphics::Combiners & _combiners) override;

		graphics::ShaderProgram * createDepthFogShader() override;

		graphics::ShaderProgram * createMonochromeShader() override;

		graphics::TexDrawerShaderProgram * createTexDrawerDrawShader() override;

		graphics::ShaderProgram * createTexDrawerClearShader() override;

		graphics::ShaderProgram * createTexrectCopyShader() override;

		graphics::DrawerImpl * createDrawerImpl() override;

		graphics::TextDrawer * createTextDrawer() override;

		f32 getMaxLineWidth() override;

	private:
		std::unique_ptr<CachedFunctions> m_cachedFunctions;
		std::unique_ptr<Create2DTexture> m_createTexture;
		std::unique_ptr<Init2DTexture> m_init2DTexture;
		std::unique_ptr<Update2DTexture> m_update2DTexture;
		std::unique_ptr<Set2DTextureParameters> m_set2DTextureParameters;

		std::unique_ptr<CreateFramebufferObject> m_createFramebuffer;
		std::unique_ptr<CreateRenderbuffer> m_createRenderbuffer;
		std::unique_ptr<InitRenderbuffer> m_initRenderbuffer;
		std::unique_ptr<AddFramebufferRenderTarget> m_addFramebufferRenderTarget;
		std::unique_ptr<CreatePixelWriteBuffer> m_createPixelWriteBuffer;

		std::unique_ptr<glsl::CombinerProgramBuilder> m_combinerProgramBuilder;
		GLInfo m_glInfo;
	};

}
