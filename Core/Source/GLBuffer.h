#pragma once
#include <glad/gl.h>
#include <Vertex.h>
#include <vector>
#include <iostream>

class GLBuffer
{
public:
	~GLBuffer() { Delete(); }

	virtual void GenBuffers(GLenum bufferType, int numBuffers)
	{
		_bufferType = bufferType;
		_bufferCount = numBuffers;
		glGenBuffers(_bufferCount, &_bufferId);

		Bind();
		Unbind();
	}
	
	virtual void Bind() { glBindBuffer(_bufferType,_bufferId); }
	virtual void Unbind() { glBindBuffer(_bufferType, 0); }
	virtual void Delete() { glDeleteBuffers(_bufferCount, &_bufferId); }

	GLuint GetBufferId() { return _bufferId; }
	GLenum GetBufferType() { return _bufferType; }
	GLsizei GetBufferCount() { return _bufferCount; }
protected:
	GLuint _bufferId;
	GLenum _bufferType;
	GLsizei _bufferCount;
};

class VertexBufferObject : public GLBuffer {
public:
	void GenBuffers(int numBuffers) { GLBuffer::GenBuffers(GL_ARRAY_BUFFER, numBuffers); }
	void UpdateBuffers(std::vector<Vertex>* vertexData)
	{
		Bind();
		glBufferData(_bufferType, vertexData->size() * sizeof(Vertex), vertexData->data(), GL_DYNAMIC_DRAW);
		Unbind();
	}
};

class ElementBufferObject : public GLBuffer {
public:
	void GenBuffers(int numBuffers) { GLBuffer::GenBuffers(GL_ELEMENT_ARRAY_BUFFER, numBuffers); }
	void UpdateBuffers(std::vector<GLuint>* elementData)
	{
		Bind();
		glBufferData(_bufferType, elementData->size() * sizeof(GLuint), elementData->data(), GL_DYNAMIC_DRAW);
		Unbind();
	}
};

class FrameBufferObject : public GLBuffer {
public:
	void GenBuffers(int numBuffers, GLuint width, GLuint height)
	{
		_height = height; 
		_width = width;
		_bufferCount = numBuffers;

		glGenFramebuffers(_bufferCount, &_bufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, _bufferId);

		glGenTextures(_bufferCount, &_texId);
		glBindTexture(GL_TEXTURE_2D, _texId);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texId, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		UnBind();
	}

	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _bufferId);
		glViewport(0, 0, _width, _height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void UnBind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	void Delete()
	{
		glDeleteFramebuffers(_bufferCount, &_bufferId);
		glDeleteTextures(_bufferCount, &_texId);
	}

	void UpdateBuffers(std::vector<GLuint>* elementData)
	{
		Bind();
		glBufferData(_bufferType, elementData->size() * sizeof(GLuint), elementData->data(), GL_DYNAMIC_DRAW);
		Unbind();
	}

	GLuint GetTextureId() { return _texId; }

private:
	GLuint _texId;
	GLuint _height;
	GLuint _width;
};

class VertexArrayObject : public GLBuffer {
public:
	void GenBuffers(int numBuffers)
	{
		_bufferCount = numBuffers;
		glGenVertexArrays(numBuffers, &_bufferId);
		Bind();
		Unbind();
	}

	void Bind() { glBindVertexArray(_bufferId); }
	void Unbind() { glBindVertexArray(0); }

	void UpdateBuffers(VertexBufferObject* vbo)
	{
		vbo->Bind();

		GLuint curVertexId = 0; // Using this makes it easier to rearrange order of Attributes without changing hardcoded numbers

		glEnableVertexAttribArray(curVertexId); // vertex position 0
		glVertexAttribPointer(curVertexId++, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

		glEnableVertexAttribArray(curVertexId); // vertex color 1
		glVertexAttribPointer(curVertexId++, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

		//glEnableVertexAttribArray(curVertexId); // vertex tex coords
		//glVertexAttribPointer(curVertexId++, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		vbo->Unbind();
	}
};