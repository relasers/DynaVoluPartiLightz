#pragma once

class Rendertarget {
public:
	Rendertarget(GLint data_format, GLsizei width, GLsizei height, GLenum pixel_format, GLenum data_type, const void* init_datas);
	~Rendertarget();

	void bind();
	void release();

	GLuint get_rendertargetbuffer() const;
	GLuint get_depthbuffer() const;

private:
	GLuint mFrameBufferObject;
	GLuint mRendertargetbuffer;
	GLuint mDepthbuffer;
};