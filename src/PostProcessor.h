#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <functional>
#include <list>
#include <memory>
#include "Types.h"
#include "Textures.h"
#include "Graphics/ObjectHandle.h"

namespace graphics {
	class ShaderProgram;
}

struct VIRegsSample;
struct FrameBuffer;

class PostProcessor {
public:
	void init();
	void destroy();

	using PostprocessingFunc = std::function<FrameBuffer*(PostProcessor&, const VIRegsSample&, FrameBuffer*)>;
	using PostprocessingList = std::list<PostprocessingFunc>;
	const PostprocessingList & getPostprocessingList() const;

	static PostProcessor & get();

private:
	PostProcessor();
	PostProcessor(const PostProcessor & _other) = delete;

	FrameBuffer * _doGammaCorrection(const VIRegsSample& regs, FrameBuffer * _pBuffer);
	FrameBuffer * _doOrientationCorrection(const VIRegsSample& regs, FrameBuffer * _pBuffer);
	FrameBuffer * _doFXAA(const VIRegsSample& regs, FrameBuffer * _pBuffer);

	void _createResultBuffer(const FrameBuffer * _pMainBuffer);
	void _preDraw(FrameBuffer * _pBuffer);
	void _postDraw();
	FrameBuffer * _doPostProcessing(FrameBuffer * _pBuffer, graphics::ShaderProgram * _pShader);

	std::unique_ptr<graphics::ShaderProgram> m_gammaCorrectionProgram;
	std::unique_ptr<graphics::ShaderProgram> m_orientationCorrectionProgram;
	std::unique_ptr<graphics::ShaderProgram> m_FXAAProgram;
	std::unique_ptr<FrameBuffer> m_pResultBuffer;
	CachedTexture * m_pTextureOriginal;
	PostprocessingList m_postprocessingList;
};

#endif // POST_PROCESSOR_H
