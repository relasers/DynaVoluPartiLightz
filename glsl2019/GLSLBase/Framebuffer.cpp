#include "stdafx.h"
#include "Framebuffer.h"

Framebuffer::Framebuffer(int num_rendertarget)
	: mNumRendertarget(num_rendertarget)
{
}

Framebuffer::~Framebuffer()
{
	glDeleteTextures(mNumRendertarget, mRendertargetbuffers);
	glDeleteTextures(1, &mDepthStencilbuffer);
	glDeleteFramebuffers(1, &mFrameBufferObject);
}

void Framebuffer::create_rendertarget_buffer(int rt_idx, GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum data_type, const void * init_datas)
{
	glGenTextures(1, &mRendertargetbuffers[rt_idx]);
	glBindTexture(GL_TEXTURE_2D, mRendertargetbuffers[rt_idx]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mTextureMinMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mTextureMinMagFilter);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, data_type, init_datas);
}

void Framebuffer::create_depth_stencil_buffer(GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum data_type, const void * init_datas)
{
	glGenTextures(1, &mDepthStencilbuffer);
	glBindTexture(GL_TEXTURE_2D, mDepthStencilbuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mTextureMinMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mTextureMinMagFilter);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, data_type, init_datas);
}

void Framebuffer::generate_framebuffer()
{
	glGenFramebuffers(1, &mFrameBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferObject);
	for (int i = 0; i < mNumRendertarget; ++i) {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, mRendertargetbuffers[i], 0);
	}
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, mDepthStencilbuffer, 0);

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