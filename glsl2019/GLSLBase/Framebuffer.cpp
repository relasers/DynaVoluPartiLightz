#include "stdafx.h"
#include "Framebuffer.h"

Framebuffer::Framebuffer()
	: mNumRendertarget(0)
	, mFrameBufferObject(UINT32_MAX)
{
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &mFrameBufferObject);
}

void Framebuffer::create_rendertarget_buffer(GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum data_type, const void * init_datas)
{
	if (MAX_RENDER_TARGET == mNumRendertarget) return;
	mRendertargetbuffers[mNumRendertarget++].load(internal_format, width, height, format, data_type, init_datas);
}

void Framebuffer::create_depthstencil_buffer(GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum data_type, const void * init_datas)
{
	mDepthStencilbuffer.load(internal_format, width, height, format, data_type, init_datas);
}

void Framebuffer::generate_framebuffer()
{
	glGenFramebuffers(1, &mFrameBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferObject);
	for (int i = 0; i < mNumRendertarget; ++i) {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, mRendertargetbuffers[i].get(), 0);
	}
	if (GL_TRUE == mDepthStencilbuffer.get()) {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, mDepthStencilbuffer.get(), 0);
	}

	GLenum DrawBuffers[] = { 
		GL_COLOR_ATTACHMENT0
		, GL_COLOR_ATTACHMENT1
		, GL_COLOR_ATTACHMENT2
		, GL_COLOR_ATTACHMENT3
		, GL_COLOR_ATTACHMENT4
		, GL_COLOR_ATTACHMENT5
		, GL_COLOR_ATTACHMENT6
		, GL_COLOR_ATTACHMENT7
		, GL_COLOR_ATTACHMENT8
		, GL_COLOR_ATTACHMENT9
		, GL_COLOR_ATTACHMENT10
		, GL_COLOR_ATTACHMENT11
		, GL_COLOR_ATTACHMENT12
		, GL_COLOR_ATTACHMENT13
		, GL_COLOR_ATTACHMENT14
		, GL_COLOR_ATTACHMENT15
	};

	glDrawBuffers(mNumRendertarget, DrawBuffers);
	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
		std::cout << "Bind framebuffer object error \n";
		while (true);
	}
}

void Framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferObject);
}

void Framebuffer::release()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}