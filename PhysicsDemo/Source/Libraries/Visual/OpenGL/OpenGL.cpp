#include "OpenGL.h"

#include "VisualDebug.h"

#define GLE(ret, name, ...) name##Procedure * gl##name;
GL_LIST
GL_LIST_WIN32
#undef GLE

PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

#include "Platform/OSWindow.h"

#include "MeshData.h"

namespace jm::OpenGL
{
	typedef PROC WINAPI wglGetProcAddressproc(LPCSTR lpszProc);
	wglGetProcAddressproc* wglGetProcAddress;

	HINSTANCE LoadDLL(cstring dllName)
	{
		HINSTANCE dll = LoadLibraryA(dllName);
		JM_VISUAL_ASSERT(dll, "%s not found!", dllName);
		return dll;
	}

	template <typename T>
	T GetProcedureAddress(HINSTANCE dll, cstring procedureName)
	{
		T procedureAddress = (T)GetProcAddress(dll, procedureName);
		JM_VISUAL_ASSERT(procedureAddress, "Cound not find procedure %s!", procedureName);
		return procedureAddress;
	}

	template <typename T>
	T GetGLProcedureAddress(LPCSTR procedureName)
	{
		T procedureAddress = (T)wglGetProcAddress(procedureName);
		JM_VISUAL_ASSERT(procedureAddress, "Cound not find procedure %s!", procedureName);
		return procedureAddress;
	}

	bool LoadFunctions()
	{
		HINSTANCE dll = LoadDLL("opengl32.dll");
		wglGetProcAddress = GetProcedureAddress<wglGetProcAddressproc*>(dll, "wglGetProcAddress");

#define GLE(ret, name, ...) \
	gl##name = GetGLProcedureAddress<name##Procedure*>("gl" #name);

		GL_LIST
		GL_LIST_WIN32
#undef GLE

		wglSwapIntervalEXT = GetGLProcedureAddress<PFNWGLSWAPINTERVALEXTPROC>("wglSwapIntervalEXT");

		return true;
	}

#if JM_DEBUG

	enum class GLError
	{
		None = GL_NO_ERROR,
		InvalidEnum = GL_INVALID_ENUM,
		InvalidValue = GL_INVALID_VALUE,
		InvalidOperation = GL_INVALID_OPERATION,
		InvalidFrameBufferOperation = GL_INVALID_FRAMEBUFFER_OPERATION,
		OutOfMemory = GL_OUT_OF_MEMORY,
		StackUnderflow = GL_STACK_UNDERFLOW,
		StackOverflow = GL_STACK_OVERFLOW
	};

	cstring GetErrorString(GLError error)
	{
		switch (error)
		{
		case GLError::InvalidEnum:
			return "Invalid Enum";
		case GLError::InvalidValue:
			return "Invalid Value";
		case GLError::InvalidOperation:
			return "Invalid Operation";
		case GLError::InvalidFrameBufferOperation:
			return "Invalid FrameBuffer Operation";
		case GLError::OutOfMemory:
			return "Out Of Memory";
		case GLError::StackUnderflow:
			return "Stack Underflow";
		case GLError::StackOverflow:
			return "Stack Overflow";
		default: //None
			return "";
		}
	}

	void CheckError()
	{
		GLError error = static_cast<GLError>(glGetError());
		JM_VISUAL_ASSERT(error == GLError::None, "Error detected! %s", GetErrorString(error));
	}

	void CheckFrameBuffer()
	{
		GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		switch (error)
		{
		case GL_FRAMEBUFFER_COMPLETE:
			return;

		case GL_FRAMEBUFFER_UNDEFINED:
			JM_VISUAL_HALT("Framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist!");
			return;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			JM_VISUAL_HALT("One or more framebuffer attachment points are framebuffer incomplete!");
			return;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			JM_VISUAL_HALT("The framebuffer does not have at least one image attached to it!");
			return;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			JM_VISUAL_HALT("GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for some color attachment point(s) \
							named by GL_DRAW_BUFFERi!");
			return;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			JM_VISUAL_HALT("GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE \
							is GL_NONE for the color attachment point named by GL_READ_BUFFER!");
			return;

		case GL_FRAMEBUFFER_UNSUPPORTED:
			JM_VISUAL_HALT("Internal formats of the attached images violates an implementation-dependent set of restrictions!");
			return;

		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			JM_VISUAL_HALT("One of the following hold: --GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers\n \
							--GL_TEXTURE_SAMPLES is the not same for all attached textures\n \
							--if the attached images are a mix of renderbuffers and textures, GL_RENDERBUFFER_SAMPLES does \n \
							not match the value of GL_TEXTURE_SAMPLES \n \
							--GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures \n \
							--if the attached images are a mix of renderbuffers and textures, GL_TEXTURE_FIXED_SAMPLE_LOCATIONS \n \
							is not GL_TRUE for all attached textures");
			return;

		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			JM_VISUAL_HALT("some framebuffer attachment is layered, and some populated attachment is not layered OR \n \
							all populated color attachments are not from textures of the same target");
			return;

		default:
			JM_VISUAL_HALT("Unknown error detected! code = %d", error);
			return;
		}
	}

#else
	void CheckError() {}
	void CheckFrameBuffer() {}

#endif

	bool FormatPixelBuffer(HDC deviceContext)
	{
		//set pixel format
		PIXELFORMATDESCRIPTOR pfd = { 0 };  // create the pfd,

		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);		// just its size
		pfd.nVersion = 1;								// always 1

		pfd.dwFlags = PFD_SUPPORT_OPENGL |  // OpenGL support - not DirectDraw
			PFD_DOUBLEBUFFER |				// double buffering support
			PFD_DRAW_TO_WINDOW;				// draw to the app window, not to a bitmap image

		pfd.iPixelType = PFD_TYPE_RGBA;		// red, green, blue, alpha for each pixel
		pfd.cColorBits = 24;                // 24 bit == 8 bits for red, 8 for green, 8 for blue.
											// This count of color bits EXCLUDES alpha.

		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;

		int chosenPixelFormat = ChoosePixelFormat(deviceContext, &pfd);

		if (chosenPixelFormat == NULL)
		{
			FatalAppExit(NULL, TEXT("ChoosePixelFormat() failed!"));
			return false;
		}

		JM_VISUAL_LOG("You got ID# %d as your pixelformat!", chosenPixelFormat);

		return SetPixelFormat(deviceContext, chosenPixelFormat, &pfd);
	}

#if JM_ON_WINDOWS
	Rasterizer::Rasterizer(HWND nativeWindowHandle, Platform::PixelArea area)
		: GLDeviceContext(GetDC(nativeWindowHandle))
		, GLRenderingContext(NULL)
		, WindowHandle(nativeWindowHandle)
#else
#error "UNSUPPORTED OS!"
#endif // JM_WINDOWS
	{
		JM_VISUAL_VERIFY(FormatPixelBuffer(GLDeviceContext));

		//create render context
		GLRenderingContext = wglCreateContext(GLDeviceContext);

		JM_VISUAL_VERIFY(wglMakeCurrent(GLDeviceContext, GLRenderingContext));
		JM_VISUAL_VERIFY(OpenGL::LoadFunctions());
		JM_VISUAL_VERIFY(wglSwapIntervalEXT(0));

		glViewport(0, 0, static_cast<GLsizei>(area.Width), static_cast<GLsizei>(area.Height));

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}

	Rasterizer::~Rasterizer()
	{
		wglMakeCurrent(NULL, NULL);

		wglDeleteContext(GLRenderingContext);

#if JM_ON_WINDOWS
		ReleaseDC(WindowHandle, GLDeviceContext);
#else
#error UNSUPPORTED OS!
#endif
	}

	void Rasterizer::PrepareRenderBuffer(math::vector3_f32 const& colour)
	{
		glClearColor(colour.x, colour.y, colour.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Rasterizer::UpdateRenderBuffer()
	{
		SwapBuffers(GLDeviceContext);
	}

	InputLayoutHandle Memory::createInputLayout(const Visual::InputLayout& inputLayout)
	{
		GLuint VAO{};
		glGenVertexArrays(1, &VAO);
		InputLayoutHandle newHandle{ VAO };
		JM_VISUAL_ASSERT(std::cmp_equal(VAO, newHandle)); //check casting safety

		LayoutState& layoutState = VAOStates[VAO];
		layoutState.layout = inputLayout;

		return newHandle;
	}

	void Memory::destroyInputLayout(InputLayoutHandle& inputLayoutHandle)
	{
		GLuint VAO = static_cast<GLuint>(inputLayoutHandle);
		JM_VISUAL_ASSERT(std::cmp_equal(VAO, inputLayoutHandle)); //check casting safety
		inputLayoutHandle = 0;

		JM_VISUAL_ASSERT(VAOStates.contains(VAO));
		LayoutState& layoutState = VAOStates[VAO];
		if (!layoutState.VBO.empty())
		{
			glDeleteBuffers((GLsizei)layoutState.VBO.size(), layoutState.VBO.data());
		}
		VAOStates.erase(VAO);

		glDeleteVertexArrays(1, &VAO);
	}

	InputBufferHandle Memory::createInputBuffer(
		InputLayoutHandle inputLayoutHandle,
		const byte_list& inputData)
	{
		GLuint VAO = static_cast<GLuint>(inputLayoutHandle);
		JM_VISUAL_ASSERT(std::cmp_equal(VAO, inputLayoutHandle)); //check casting safety
		JM_VISUAL_ASSERT(VAOStates.contains(VAO));

		GLuint VBO{};
		glGenBuffers(1, &VBO);
		InputBufferHandle newHandle{ VBO };
		JM_VISUAL_ASSERT(std::cmp_equal(VBO, newHandle)); //check casting safety

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		OpenGL::CheckError();

		VAOStates[VAO].VBO.push_back(VBO);
		Visual::InputLayout& inputLayout = VAOStates[VAO].layout;

		GLsizei stride = (GLsizei)(inputLayout.elementSize * sizeof(float));

		for (u32 a = 0; a < inputLayout.attributes.size(); ++a)
		{
			auto attribute = inputLayout.attributes[a];
			JM_VISUAL_ASSERT(1 <= attribute.size && attribute.size <= 4);
			void* ptrOffset = (void*)(attribute.offset * sizeof(float));
			glVertexAttribPointer(a, attribute.size, GL_FLOAT, GL_FALSE, stride, ptrOffset);
			glEnableVertexAttribArray(a);
			OpenGL::CheckError();
		}


		glBufferData(GL_ARRAY_BUFFER, inputData.size(), inputData.data(), GL_STATIC_DRAW);

		OpenGL::CheckError();
		return newHandle;
	}

	void Memory::destroyInputBuffer(InputLayoutHandle inputLayoutHandle, InputBufferHandle& bufferHandle)
	{
		GLuint VAO = static_cast<GLuint>(inputLayoutHandle);
		JM_VISUAL_ASSERT(std::cmp_equal(VAO, inputLayoutHandle)); //check casting safety
		JM_VISUAL_ASSERT(VAOStates.contains(VAO));

		GLuint VBO = static_cast<GLuint>(bufferHandle);
		JM_VISUAL_ASSERT(std::cmp_equal(VBO, bufferHandle)); //check casting safety

		auto& vbos = VAOStates[VAO].VBO;
		auto findResult = std::find(vbos.begin(), vbos.end(), VBO);
		JM_VISUAL_ASSERT(findResult != vbos.end());

		bufferHandle = 0;
		*findResult = NULL;
		vbos.erase(findResult);

		glDeleteBuffers(1, &VBO);
	}
}
