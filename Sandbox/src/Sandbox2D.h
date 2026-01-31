#pragma once

#include "GameEngine.h"
#include "Panels/SceneHierarchyPanel.h"
#include "GameEngine/Renderer/EditorCamera.h"

class Sandbox2D : public GameEngine::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	void OnUpdate(GameEngine::Timestep ts);
	virtual void OnImGuiRender() override;
	void OnEvent(GameEngine::Event& e) override;
private:
	bool OnKeyPressed(GameEngine::KeyPressedEvent& e);

	void NewScene();
	void OpenScene();
	void SaveSceneAs();
private:
	GameEngine::OrthographicCameraController m_CameraController;


	GameEngine::Ref<GameEngine::VertexArray> m_SquareVA;
	GameEngine::Ref<GameEngine::Shader> m_FlatShader;
	GameEngine::Ref<GameEngine::Texture2D> m_Texture;
	GameEngine::Ref<GameEngine::Texture2D> m_SpriteSheet;
	GameEngine::Ref<GameEngine::SubTexture2D> m_TextureStairs;
	GameEngine::Ref<GameEngine::Framebuffer> m_Framebuffer;

	GameEngine::Ref<GameEngine::Scene> m_ActiveScene;

	GameEngine::EditorCamera m_EditorCamera;

	bool m_ViewportFocused = false, m_ViewportHovered = false;
	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	glm::vec2 m_ViewportBounds[2];


	glm::vec4 m_Color = { 0.8f, 0.3f, 0.2f, 1.0f };

	int m_GizmoType = -1;

	uint32_t m_MapWidth, m_MapHeight;
	std::unordered_map<char, GameEngine::Ref<GameEngine::SubTexture2D>> s_TextureMap;

	GameEngine::SceneHierarchyPanel m_SceneHierarchyPanel;
};