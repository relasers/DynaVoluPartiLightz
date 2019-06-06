#pragma once
#include "Texture.h"

class Framebuffer {
public:
	static constexpr int MAX_RENDER_TARGET = 16;

public:
	Framebuffer();
	~Framebuffer();

	void create_rendertarget_buffer(GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum data_type, const void* init_datas = nullptr);
	void create_depthstencil_buffer(GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum data_type, const void* init_datas = nullptr);
	void generate_framebuffer();

	void bind();
	void release();

	GLuint get_rendertarget_buffer(int rt_idx = 0) const { return mRendertargetbuffers[rt_idx].get(); }
	GLuint get_depthstencil_buffer() const { return mDepthStencilbuffer.get(); }

private:
	int mNumRendertarget;
	Texture2D mRendertargetbuffers[MAX_RENDER_TARGET];
	Texture2D mDepthStencilbuffer;

	GLuint mFrameBufferObject;
};