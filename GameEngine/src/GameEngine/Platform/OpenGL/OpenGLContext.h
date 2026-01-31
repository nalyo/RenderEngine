#pragma once

#include "GameEngine/Renderer/GraphicsContext.h"
#include "GameEngine/Core/Core.h"
#include <GameEngine.h>

struct GLFWwindow;

namespace GameEngine
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_windowHandle;
	};
}