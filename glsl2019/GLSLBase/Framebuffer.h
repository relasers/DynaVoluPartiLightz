#pragma once

class Framebuffer {
public:
	static constexpr int MAX_RENDER_TARGET = 16;

public:
	Framebuffer(int num_rendertarget);
	~Framebuffer();

	void create_rendertarget_buffer(int rt_idx, GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum data_type, const void* init_datas = nullptr);
	void create_depth_stencil_buffer(GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum data_type, const void* init_datas = nullptr);
	void generate_framebuffer();

	void bind();
	void release();

	void set_texture_min_mag_filter(GLint param) { mTextureMinMagFilter = param; }

	GLuint get_rendertarget_buffer(int rt_idx) const { return mRendertargetbuffers[rt_idx]; }
	GLuint get_depth_stencil_buffer() const { return mDepthStencilbuffer; }

private:
	const int mNumRendertarget;
	GLuint mRendertargetbuffers[MAX_RENDER_TARGET];
	GLuint mDepthStencilbuffer;
	GLuint mFrameBufferObject;

	GLuint mTextureMinMagFilter;
};