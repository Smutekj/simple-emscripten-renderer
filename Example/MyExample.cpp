#include <EglContext.h>
#include <VertexArray.h>
#include <Shader.h>
#include <View.h>
// #include "PVRShell/PVRShell.h

class OpenGLESIntroducingPVRShell : public pvr::Shell
{
	EglContext m_context;

	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<VertexArray> m_verts;

public:
	// following function must be override
	virtual pvr::Result initApplication();
	virtual pvr::Result initView();
	virtual pvr::Result releaseView();
	virtual pvr::Result quitApplication();
	virtual pvr::Result renderFrame();
};

pvr::Result OpenGLESIntroducingPVRShell::initApplication()
{
	setBackBufferColorspace(pvr::ColorSpace::lRGB);
	return pvr::Result::Success;
}

pvr::Result OpenGLESIntroducingPVRShell::quitApplication() { return pvr::Result::Success; }

pvr::Result OpenGLESIntroducingPVRShell::initView()
{
	// Initialize the PowerVR OpenGL bindings. Must be called before using any of the gl:: commands.
	m_context.init(getWindow(), getDisplay(), getDisplayAttributes());

	m_shader = std::make_unique<Shader>("../../Resources/Shaders/basictex.vert", "../../Resources/Shaders/fullpass.frag");
	m_verts = std::make_unique<VertexArray>(*m_shader);
	if (m_verts)
	{
		auto& verts = *m_verts;
		verts.resize(3);
		verts[0] = { { 0, 0 }, { 1., 1., 0., 1. }, { 0, 0 } };
		verts[1] = { { 1, 0 }, { 0., 0., 0., 1. }, { 1, 0 } };
		verts[2] = { { 0, 1 }, { 1., 0., 0., 1. }, { 0, 1 } };
	}
	else { throw std::runtime_error("Vertex Array creation not succesful!"); }

	// Sets the clear colour
	gl::ClearColor(0.00f, 0.70f, 0.67f, 1.0f);

	// Enable culling
	gl::Enable(GL_CULL_FACE);
	return pvr::Result::Success;
}

pvr::Result OpenGLESIntroducingPVRShell::releaseView()
{
	m_context.release();

	return pvr::Result::Success;
}

pvr::Result OpenGLESIntroducingPVRShell::renderFrame()
{
	gl::Clear(GL_COLOR_BUFFER_BIT);
	
	View basic_view;
	m_verts->draw(basic_view);

	if (m_context.isApiSupported(pvr::Api::OpenGLES3))
	{
		GLenum invalidateAttachments[2];
		invalidateAttachments[0] = GL_DEPTH;
		invalidateAttachments[1] = GL_STENCIL;

		gl::InvalidateFramebuffer(GL_FRAMEBUFFER, 2, &invalidateAttachments[0]);
	}
	else if (gl::isGlExtensionSupported("GL_EXT_discard_framebuffer"))
	{
		GLenum invalidateAttachments[2];
		invalidateAttachments[0] = GL_DEPTH_EXT;
		invalidateAttachments[1] = GL_STENCIL_EXT;

		gl::ext::DiscardFramebufferEXT(GL_FRAMEBUFFER, 2, &invalidateAttachments[0]);
	}

	m_context.swapBuffers();
	return pvr::Result::Success;
}

/// <summary>This function must be implemented by the user of the shell. The user should return its pvr::Shell object defining the behaviour of the application.</summary>
/// <returns>Return a unique ptr to the demo supplied by the user.</returns>
std::unique_ptr<pvr::Shell> pvr::newDemo() { return std::make_unique<OpenGLESIntroducingPVRShell>(); }
