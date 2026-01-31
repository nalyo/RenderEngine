#include <GameEngine.h>
#include <glm/ext/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "GameEngine/Platform/OpenGL/OpenGLShader.h"

#include "Sandbox2D.h"

class ExampleLayer : public GameEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f, true)
	{
		m_VertexArray = GameEngine::VertexArray::Create();

		float vertices[7 * 3] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
		};

		GameEngine::Ref<GameEngine::VertexBuffer> m_VertexBuffer;
		m_VertexBuffer = GameEngine::VertexBuffer::Create(vertices, sizeof(vertices));

		GameEngine::BufferLayout layout = {
			{ GameEngine::ShaderDataType::Float3, "a_Position"},
			{ GameEngine::ShaderDataType::Float4, "a_Color"}
		};

		m_VertexBuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		GameEngine::Ref<GameEngine::IndexBuffer> m_IndexBuffer;
		m_IndexBuffer = GameEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);



		m_SquareVA = GameEngine::VertexArray::Create();

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			 -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		GameEngine::Ref<GameEngine::VertexBuffer> squareVB;
		squareVB = GameEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		GameEngine::BufferLayout squareVBLayout = {
			{ GameEngine::ShaderDataType::Float3, "a_Position"},
			{ GameEngine::ShaderDataType::Float2, "a_TexCoord"}
		};

		squareVB->SetLayout(squareVBLayout);
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		GameEngine::Ref<GameEngine::IndexBuffer> squareIB;
		squareIB = GameEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec4 v_Position;

			void main() 
			{
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
				v_Position = a_Color;
			}
		)";
		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;
			in vec4 v_Position;

			void main()
			{
				color = vec4((v_Position.xyz + 0.5) * 0.5, 1.0);
			}
		)";
		m_Shader = GameEngine::Shader::Create("Shader", vertexSrc, fragmentSrc);

		std::string flatShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
				v_Position = a_Position;
			}
		)";
		std::string flatShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;
			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";
		m_FlatShader = GameEngine::Shader::Create("FlatShader", flatShaderVertexSrc, flatShaderFragmentSrc);

		
		auto m_TextureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = GameEngine::Texture2D::Create("assets/textures/achievement_pure_completion.png");
		m_Texture2 = GameEngine::Texture2D::Create("assets/textures/_0007_shadow_scream.png");

		std::dynamic_pointer_cast<GameEngine::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<GameEngine::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
}

	void OnUpdate(GameEngine::Timestep ts) override
	{
		m_CameraController.OnUpdate(ts);

		GameEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		GameEngine::RenderCommand::Clear();

		GameEngine::Renderer::BeginScene(m_CameraController.GetCamera());

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<GameEngine::OpenGLShader>(m_FlatShader)->Bind();
		std::dynamic_pointer_cast<GameEngine::OpenGLShader>(m_FlatShader)->UploadUniformFloat3("u_Color", m_Color);

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				glm::vec3 pos(i * 0.11f, j * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				GameEngine::Renderer::Submit(m_FlatShader, m_SquareVA, transform);
			}
		}
		auto m_TextureShader = m_ShaderLibrary.Get("Texture");
		m_Texture->Bind();
		GameEngine::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_Texture2->Bind();
		GameEngine::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		// Triangle
		// GameEngine::Renderer::Submit(m_Shader, m_VertexArray);

		GameEngine::Renderer::EndScene();

	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_Color));
		ImGui::End();
	}

	void OnEvent(GameEngine::Event& e) override
	{
		m_CameraController.OnEvent(e);

	}

private:
	GameEngine::ShaderLibrary m_ShaderLibrary;
	GameEngine::Ref<GameEngine::Shader> m_Shader;
	GameEngine::Ref<GameEngine::VertexArray> m_VertexArray;

	GameEngine::Ref<GameEngine::Shader> m_FlatShader;
	GameEngine::Ref<GameEngine::VertexArray> m_SquareVA;

	GameEngine::Ref<GameEngine::Texture2D> m_Texture;
	GameEngine::Ref<GameEngine::Texture2D> m_Texture2;

	GameEngine::OrthographicCameraController m_CameraController;

	glm::vec3 m_Color = { 0.8f, 0.3f, 0.2f };

};

class Sandbox : public GameEngine::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{

	}
};

GameEngine::Application* GameEngine::CreateApplication()
{
	return new Sandbox();
}


int main(int argc, char** argv)
{
	GameEngine::Log::Init();

	GE_PROFILE_BEGIN_SESSION("Startup", "GameEngineProfile-Startup.json");
	auto app = GameEngine::CreateApplication();
	GE_PROFILE_END_SESSION();

	GE_PROFILE_BEGIN_SESSION("Runtime", "GameEngineProfile-Runtime.json");
	app->Run();
	GE_PROFILE_END_SESSION();

	GE_PROFILE_BEGIN_SESSION("Shutdown", "GameEngineProfile-Shutdown.json");
	delete app;
	GE_PROFILE_END_SESSION();
}
