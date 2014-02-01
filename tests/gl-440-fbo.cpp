///////////////////////////////////////////////////////////////////////////////////
/// OpenGL Samples Pack (ogl-samples.g-truc.net)
///
/// Copyright (c) 2004 - 2014 G-Truc Creation (www.g-truc.net)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////////

#include "test.hpp"

namespace
{
	char const * VERT_SHADER_SOURCE_TEXTURE("gl-440/fbo-texture-2d.vert");
	char const * FRAG_SHADER_SOURCE_TEXTURE("gl-440/fbo-texture-2d.frag");
	char const * VERT_SHADER_SOURCE_SPLASH("gl-440/fbo-splash.vert");
	char const * FRAG_SHADER_SOURCE_SPLASH("gl-440/fbo-splash.frag");
	char const * TEXTURE_DIFFUSE("kueken1-dxt1.dds");

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f))
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2, 
		2, 3, 0
	};

	namespace buffer
	{
		enum type
		{
			VERTEX,
			ELEMENT,
			TRANSFORM,
			MAX
		};
	}//namespace buffer

	namespace texture
	{
		enum type
		{
			DIFFUSE,
			COLORBUFFER,
			RENDERBUFFER,
			MAX
		};
	}//namespace texture
	
	namespace pipeline
	{
		enum type
		{
			TEXTURE,
			SPLASH,
			MAX
		};
	}//namespace pipeline
}//namespace

class gl_440_fbo : public test
{
public:
	gl_440_fbo(int argc, char* argv[]) :
		test(argc, argv, "gl-440-fbo", test::CORE, 4, 4, glm::vec2(glm::pi<float>() * 0.2f)),
		UniformPointer(nullptr),
		FramebufferName(0)
	{}

private:
	glm::mat4* UniformPointer;
	GLuint FramebufferName;
	std::array<GLuint, pipeline::MAX> ProgramName;
	std::array<GLuint, pipeline::MAX> VertexArrayName;
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, texture::MAX> TextureName;
	std::array<GLuint, pipeline::MAX> PipelineName;

	bool initProgram()
	{
		bool Validated(true);
	
		glGenProgramPipelines(pipeline::MAX, &PipelineName[0]);

		if(Validated)
		{
			glf::compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, glf::DATA_DIRECTORY + VERT_SHADER_SOURCE_TEXTURE, "--version 440 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, glf::DATA_DIRECTORY + FRAG_SHADER_SOURCE_TEXTURE, "--version 440 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName[pipeline::TEXTURE] = glCreateProgram();
			glProgramParameteri(ProgramName[pipeline::TEXTURE], GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName[pipeline::TEXTURE], VertShaderName);
			glAttachShader(ProgramName[pipeline::TEXTURE], FragShaderName);
			glLinkProgram(ProgramName[pipeline::TEXTURE]);
			glDeleteShader(VertShaderName);
			glDeleteShader(FragShaderName);

			Validated = Validated && glf::checkProgram(ProgramName[pipeline::TEXTURE]);
		}

		if(Validated)
			glUseProgramStages(PipelineName[pipeline::TEXTURE], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[pipeline::TEXTURE]);

		if(Validated)
		{
			glf::compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, glf::DATA_DIRECTORY + VERT_SHADER_SOURCE_SPLASH, "--version 440 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, glf::DATA_DIRECTORY + FRAG_SHADER_SOURCE_SPLASH, "--version 440 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName[pipeline::SPLASH] = glCreateProgram();
			glProgramParameteri(ProgramName[pipeline::SPLASH], GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName[pipeline::SPLASH], VertShaderName);
			glAttachShader(ProgramName[pipeline::SPLASH], FragShaderName);
			glLinkProgram(ProgramName[pipeline::SPLASH]);
			glDeleteShader(VertShaderName);
			glDeleteShader(FragShaderName);

			Validated = Validated && glf::checkProgram(ProgramName[pipeline::SPLASH]);
		}

		if(Validated)
			glUseProgramStages(PipelineName[pipeline::SPLASH], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[pipeline::SPLASH]);

		return Validated;
	}

	bool initBuffer()
	{
		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferStorage(GL_ARRAY_BUFFER, VertexSize, VertexData, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glBufferStorage(GL_UNIFORM_BUFFER, UniformBlockSize, NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return true;
	}

	bool initTexture()
	{
		bool Validated(true);

		gli::texture2D Texture(gli::load_dds((glf::DATA_DIRECTORY + TEXTURE_DIFFUSE).c_str()));
		assert(!Texture.empty());

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(texture::MAX, &TextureName[0]);

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::DIFFUSE]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);

		glTexStorage2D(
			GL_TEXTURE_2D,
			GLint(Texture.levels()),
			gli::internal_format(Texture.format()),
			GLsizei(Texture.dimensions().x),
			GLsizei(Texture.dimensions().y));

		for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
		{
			glCompressedTexSubImage2D(
				GL_TEXTURE_2D,
				GLint(Level),
				0, 0,
				GLsizei(Texture[Level].dimensions().x),
				GLsizei(Texture[Level].dimensions().y),
				gli::external_format(Texture.format()),
				GLsizei(Texture[Level].size()), 
				Texture[Level].data());
		}
	
		glm::ivec2 WindowSize = this->getWindowSize();

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexStorage2D(GL_TEXTURE_2D, GLint(1), GL_RGBA8, GLsizei(WindowSize.x), GLsizei(WindowSize.y));

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::RENDERBUFFER]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexStorage2D(GL_TEXTURE_2D, GLint(1), GL_DEPTH_COMPONENT24, GLsizei(WindowSize.x), GLsizei(WindowSize.y));

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		return Validated;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(pipeline::MAX, &VertexArrayName[0]);
		glBindVertexArray(VertexArrayName[pipeline::TEXTURE]);
			glVertexAttribFormat(glf::semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0);
			glVertexAttribBinding(glf::semantic::attr::POSITION, glf::semantic::buffer::STATIC);
			glEnableVertexAttribArray(glf::semantic::attr::POSITION);

			glVertexAttribFormat(glf::semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2));
			glVertexAttribBinding(glf::semantic::attr::TEXCOORD, glf::semantic::buffer::STATIC);
			glEnableVertexAttribArray(glf::semantic::attr::TEXCOORD);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBindVertexArray(0);

		glBindVertexArray(VertexArrayName[pipeline::SPLASH]);
		glBindVertexArray(0);

		return true;
	}

	bool initFramebuffer()
	{
		bool Validated(true);

		glGenFramebuffers(1, &FramebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureName[texture::COLORBUFFER], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TextureName[texture::RENDERBUFFER], 0);

		if(glf::checkFramebuffer(FramebufferName))
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	bool begin()
	{
		bool Validated(true);

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initFramebuffer();

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		UniformPointer = (glm::mat4*)glMapBufferRange(
			GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

		return Validated;
	}

	bool end()
	{
		glDeleteProgramPipelines(pipeline::MAX, &PipelineName[0]);
		glDeleteProgram(ProgramName[pipeline::SPLASH]);
		glDeleteProgram(ProgramName[pipeline::TEXTURE]);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteFramebuffers(1, &FramebufferName);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteVertexArrays(pipeline::MAX, &VertexArrayName[0]);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		{
			float Aspect = (WindowSize.x * 0.50f) / (WindowSize.y * 0.50f);
			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, Aspect, 0.1f, 100.0f);
			glm::mat4 MVP = Projection * this->view() * glm::mat4(1.0f);

			*UniformPointer = MVP;
		}

		glFlushMappedBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4));

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glViewportIndexedf(0, 0, 0, WindowSize.x, WindowSize.y);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		float Depth(1.0f);
		glClearBufferfv(GL_DEPTH , 0, &Depth);
		glClearTexImage(TextureName[texture::COLORBUFFER], 0, 
			GL_RGBA, GL_UNSIGNED_BYTE, &glm::u8vec4(255, 127,   0, 255));
		glClearTexSubImage(TextureName[texture::COLORBUFFER], 0, 
			64, 64, 0,
			64, 64, 1,
			GL_RGBA, GL_UNSIGNED_BYTE, &glm::u8vec4(  0, 127, 255, 255));
		glClearTexSubImage(TextureName[texture::COLORBUFFER], 0, 
			256, 0, 0,
			128, 128, 1,
			GL_RGBA, GL_UNSIGNED_BYTE, &glm::u8vec4(  0, 127, 255, 255));
		glClearTexSubImage(TextureName[texture::COLORBUFFER], 0, 
			128, 384, 0,
			64, 64, 1,
			GL_RGBA, GL_UNSIGNED_BYTE, &glm::u8vec4(  0, 127, 255, 255));
		glClearTexSubImage(TextureName[texture::COLORBUFFER], 0, 
			512, 256, 0,
			64, 64, 1,
			GL_RGBA, GL_UNSIGNED_BYTE, &glm::u8vec4(  0, 127, 255, 255));


		// Bind rendering objects
		glBindProgramPipeline(PipelineName[pipeline::TEXTURE]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::DIFFUSE]);
		glBindVertexArray(VertexArrayName[pipeline::TEXTURE]);
		glBindVertexBuffer(glf::semantic::buffer::STATIC, BufferName[buffer::VERTEX], 0, GLsizei(sizeof(glf::vertex_v2fv2f)));
		glBindBufferBase(GL_UNIFORM_BUFFER, glf::semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);

		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 2, 0, 0);

		glDisable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindProgramPipeline(PipelineName[pipeline::SPLASH]);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VertexArrayName[pipeline::SPLASH]);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);

		glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);

		return true;
	}
};

int main(int argc, char* argv[])
{
	int Error(0);

	gl_440_fbo Test(argc, argv);
	Error += Test();

	return Error;
}

