#include "stdafx.h"
#include "Rendertarget.h"

Rendertarget::Rendertarget(GLint data_format, GLsizei width, GLsizei height, GLenum pixel_format, GLenum data_type, const void * init_datas)
{
	glGenTextures(1, &mRendertargetbuffer);
	glBindTexture(GL_TEXTURE_2D, mRendertargetbuffer);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, data_format, width, height, 0, pixel_format, data_type, init_datas);

	glGenTextures(1, &mDepthbuffer);
	glBindTexture(GL_TEXTURE_2D, mDepthbuffer);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glGenFramebuffers(1, &mFrameBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferObject);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mRendertargetbuffer, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthbuffer, 0);

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
		std::cout << "Bind framebuffer object error \n";
		while (true);
	}
}

Rendertarget::~Rendertarget()
{
}

void Rendertarget::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferObject);
}

void Rendertarget::release()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Rendertarget::get_rendertargetbuffer() const
{
	return mRendertargetbuffer;
}

GLuint Rendertarget::get_depthbuffer() const
{
	return mDepthbuffer;
}
