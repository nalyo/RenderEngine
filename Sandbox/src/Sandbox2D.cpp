#include "Sandbox2D.h"
#include "imgui/imgui.h"
#include "GameEngine/Platform/OpenGL/OpenGLShader.h"


#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GameEngine/Scene/SceneSerializer.h"
#include "GameEngine/Utils/PlatformUtils.h"

#include "ImGuizmo/ImGuizmo.h"

#include "GameEngine/Math/Math.h"

static const uint32_t s_MapWidth = 25;
static const char* s_MapTiles = 
"WWWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWDDDDDDDDDDDDDWWWWWW"
"WWDDDDDWWWDDDDDDDDDDDDWWW"
"WWDDDDDWWWDDDWWWDDDDDDWWW"
"WWWWDDDDDDDDWWWDDDDWWWWWW"
"WWWWWDDDDDDDDDDDDWWWWWWWW"
"WWWWWWWDDDDDDDDDWWWWWWWWW"
"WWWWWWWWDDDDDDDWWWWWWWWWW"
"WWWWWWWWWDDDDWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWWW"
;

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{

}

void Sandbox2D::OnAttach()
{
	GE_PROFILE_FUNCTION();

	m_Texture = GameEngine::Texture2D::Create("assets/textures/achievement_pure_completion.png");
	m_SpriteSheet = GameEngine::Texture2D::Create("assets/game/textures/roguelikeSheet_transparent.png");
	
	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / s_MapWidth;
	s_TextureMap['D'] = GameEngine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 0, 5 }, { 16, 16 }, { 1, 1 });
	s_TextureMap['W'] = GameEngine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 0, 32 }, { 16, 16 }, {1, 1});

    GameEngine::FramebufferSpecification fbSpec;
    fbSpec.Attachments = { GameEngine::FramebufferTextureFormat::RGBA8, GameEngine::FramebufferTextureFormat::RED_INTEGER, GameEngine::FramebufferTextureFormat::Depth };
    fbSpec.Width = 1280;
    fbSpec.Height = 720;
    m_Framebuffer = GameEngine::Framebuffer::Create(fbSpec);

    m_ActiveScene = GameEngine::CreateRef<GameEngine::Scene>();

    m_EditorCamera = GameEngine::EditorCamera();

#if 0
    m_SquareEntity = m_ActiveScene->CreateEntity("Square");
    m_SquareEntity.AddComponent<GameEngine::SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

    m_CameraEntity = m_ActiveScene->CreateEntity("Camera");
    m_CameraEntity.AddComponent<GameEngine::CameraComponent>();

    class CameraController : public GameEngine::ScriptableEntity
    {
    public:
        void OnCreate()
        {
        }
        void OnDestroy()
        {
        }
        void OnUpdate(GameEngine::Timestep ts)
        {
            auto& translation = GetComponent<GameEngine::TransformComponent>().Translation;
            float m_CameraTranslationSpeed = 5.0f;

            if (GameEngine::Input::IsKeyPressed(GE_KEY_A))
                translation.x -= m_CameraTranslationSpeed * ts;
            else if (GameEngine::Input::IsKeyPressed(GE_KEY_D))
                translation.x += m_CameraTranslationSpeed * ts;
            if (GameEngine::Input::IsKeyPressed(GE_KEY_W))
                translation.y += m_CameraTranslationSpeed * ts;
            else if (GameEngine::Input::IsKeyPressed(GE_KEY_S))
                translation.y -= m_CameraTranslationSpeed * ts;
        }
    };
    m_CameraEntity.AddComponent<GameEngine::NativeScriptComponent>().Bind<CameraController>();
#endif
    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void Sandbox2D::OnDetach()
{
	GE_PROFILE_FUNCTION();

}

void Sandbox2D::OnUpdate(GameEngine::Timestep ts)
{
	GE_PROFILE_FUNCTION();

    if (GameEngine::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
        m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
        (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
    {
        m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
        m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    }

    if(m_ViewportFocused)
    {
        m_CameraController.OnUpdate(ts);
        m_EditorCamera.OnUpdate(ts);
    }


	GameEngine::Renderer2D::ResetStats();
    m_Framebuffer->Bind();
	GameEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	GameEngine::RenderCommand::Clear();

	{
		//static float rotation = 0.0f;
  //      rotation += glm::radians(ts * 50.0f);

		GE_PROFILE_SCOPE("Renderer Draw");
        // Update Scene
        m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);

        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y - my;
        int mouseX = (int)mx;
        int mouseY = (int)my;

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
        {
            int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
            GE_CORE_WARN("pixelData = {0}", pixelData);
        }

          

        m_Framebuffer->Unbind();
	}


#if 0
	GameEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
	for (uint32_t y = 0; y < m_MapHeight; y++)
	{
		for (uint32_t x = 0; x < m_MapWidth; x++)
		{
			char tileType = s_MapTiles[x + y * m_MapWidth];
			GameEngine::Ref<GameEngine::SubTexture2D> texture;
			if (s_TextureMap.find(tileType) != s_TextureMap.end())
			{
				texture = s_TextureMap[tileType];
			}
			else
			{
				texture = s_TextureMap['W'];
			}
			
			GameEngine::Renderer2D::DrawQuad({ x - m_MapWidth /2.0f, m_MapHeight - y - m_MapHeight / 2.0f, 0.0f }, { 1.0f, 1.0f }, texture);
		}
	}
	GameEngine::Renderer2D::EndScene();

#endif

}

void Sandbox2D::OnImGuiRender()
{
	GE_PROFILE_FUNCTION();

    static bool dockspaceOpen = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    style.WindowMinSize.x = minWinSizeX;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
                NewScene();
            }

            if (ImGui::MenuItem("Open...", "Ctrl+O"))
            {
                OpenScene();
            }

            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
            {
                SaveSceneAs();
            }

            if (ImGui::MenuItem("Exit")) GameEngine::Application::Get().Close();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    m_SceneHierarchyPanel.OnImGuiRender();

    ImGui::Begin("Settings");

    auto stats = GameEngine::Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    //if(m_SquareEntity)
    //{
    //    ImGui::Separator();
    //    auto& tag = m_SquareEntity.GetComponent<GameEngine::TagComponent>().Tag;
    //    ImGui::Text("%s", tag.c_str());

    //    auto& suqareColor = m_SquareEntity.GetComponent<GameEngine::SpriteRendererComponent>().Color;
    //    ImGui::ColorEdit4("Square Color", glm::value_ptr(suqareColor));
    //}

    //{
    //    auto& camera = m_CameraEntity.GetComponent<GameEngine::CameraComponent>().Camera;
    //    float orthoSize = camera.GetOrthographicSize();
    //    if (ImGui::DragFloat("Orhto Size", &orthoSize))
    //        camera.SetOrthographicSize(orthoSize);
    //}

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0 ,0 });
    ImGui::Begin("Viewport");
    auto viewportOffset = ImGui::GetCursorPos();

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();
    GameEngine::Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
    {
        //m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        //m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);
    }
    uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
    ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    
    auto windowSize = ImGui::GetWindowSize();
    ImVec2 minBound = ImGui::GetWindowPos();
    minBound.x += viewportOffset.x;
    minBound.y += viewportOffset.y;

    ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
    m_ViewportBounds[0] = { minBound.x, minBound.y };
    m_ViewportBounds[1] = { maxBound.x, maxBound.y };

    // Gizmos
    GameEngine::Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity(); 
    if (selectedEntity && m_GizmoType != -1)
    {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        ImGuizmo::SetRect(
            windowPos.x,
            windowPos.y,
            windowSize.x,
            windowSize.y
        );

        // Camera
        //auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
        //const auto& camera = cameraEntity.GetComponent<GameEngine::CameraComponent>().Camera;
        //const glm::mat4& cameraProjection = camera.GetProjection();
        //glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<GameEngine::TransformComponent>().GetTransform());

        // Editor Camera
        const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
        glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

        // Entity transform
        auto& tc = selectedEntity.GetComponent<GameEngine::TransformComponent>();
        glm::mat4 transform = tc.GetTransform();

        bool snap = GameEngine::Input::IsKeyPressed(GE_KEY_LEFT_CONTROL);
        float snapValue = 0.5f;
        if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
            snapValue = 45.0f;

        float snapValues[3] = { snapValue, snapValue, snapValue };

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
            (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
            nullptr, snap ? snapValues : nullptr);
    
        if (ImGuizmo::IsUsing())
        {
            glm::vec3 translation, rotation, scale;
            GameEngine::Math::DecomposeTransform(transform, translation, rotation, scale);

            glm::vec3 deltaRotation = rotation - tc.Rotation;
            tc.Translation = translation;
            tc.Rotation += deltaRotation;
            tc.Scale = scale;
        }
    }   
   
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::End();
}

void Sandbox2D::OnEvent(GameEngine::Event& e)
{
	m_CameraController.OnEvent(e);
    m_EditorCamera.OnEvent(e);

    GameEngine::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<GameEngine::KeyPressedEvent>(GE_BIND_EVENT_FN(Sandbox2D::OnKeyPressed));
}

bool Sandbox2D::OnKeyPressed(GameEngine::KeyPressedEvent& e)
{
    if (e.GetRepeatCount() > 0)
        return false;

    bool control = GameEngine::Input::IsKeyPressed(GE_KEY_LEFT_CONTROL) || GameEngine::Input::IsKeyPressed(GE_KEY_RIGHT_CONTROL);
    bool shift = GameEngine::Input::IsKeyPressed(GE_KEY_LEFT_SHIFT) || GameEngine::Input::IsKeyPressed(GE_KEY_RIGHT_SHIFT);
    switch (e.GetKeyCode())
    {
        case GE_KEY_N:
        {
            if (control)
            {
                NewScene();
            }
            break;
        }
        case GE_KEY_O:
        {
            if (control)
            {
                OpenScene();
            }
            break;
        }
        case GE_KEY_S:
        {
            if (control && shift)
            {
                SaveSceneAs();
            }
            else {
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
            }
            break;
        }

        // Gizmos

        case GE_KEY_Q:
            m_GizmoType = -1;
            break;
        case GE_KEY_G:
            m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
            break;
        case GE_KEY_R:
            m_GizmoType = ImGuizmo::OPERATION::ROTATE;
            break;
    }
}

void Sandbox2D::NewScene()
{
    m_ActiveScene = GameEngine::CreateRef<GameEngine::Scene>();
    m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void Sandbox2D::OpenScene()
{
    std::string filepath = GameEngine::FileDialogs::OpenFile("GameEngine Scene (*.ge)\0*.ge\0");
    if (!filepath.empty())
    {
        m_ActiveScene = GameEngine::CreateRef<GameEngine::Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

        GameEngine::SceneSerializer serializer(m_ActiveScene);
        serializer.Deserialize(filepath);
    }
}

void Sandbox2D::SaveSceneAs()
{
    std::string filepath = GameEngine::FileDialogs::SaveFile("GameEngine Scene (*.ge)\0*.ge\0");
    if (!filepath.empty())
    {
        GameEngine::SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(filepath);
    }
}
