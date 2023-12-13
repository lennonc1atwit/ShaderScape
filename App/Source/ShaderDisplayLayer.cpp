#include "ShaderDisplayLayer.h"

Scape::ShaderDisplayLayer::ShaderDisplayLayer(std::shared_ptr<Shader> shader): Layer("ShaderDisplayLayer")
{
    _shaderProgram = shader;
    OnAttach();
}

void Scape::ShaderDisplayLayer::OnAttach()
{
    _shaderProgram->AddOnLink(std::bind(&ShaderDisplayLayer::OnShaderLink, this));
    _quad = std::make_unique<Quad>(glm::vec2(100, 100), _shaderProgram);

    _iTime = 0;
    _iFrame = 0;
}

void Scape::ShaderDisplayLayer::OnUpdate(double time, double timeStep) 
{ 
    _iTime += (float)timeStep;
    _shaderProgram->SendUniform("iTime", GL_FLOAT, &_iTime);

    float iTimeDelta = (float)timeStep;
    _shaderProgram->SendUniform("iTimeDelta", GL_FLOAT, &iTimeDelta);

    _quad->Render(true); 
}

void Scape::ShaderDisplayLayer::OnImGuiRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("Output", NULL, ImGuiWindowFlags_NoTitleBar) && !_shaderProgram->Empty())
    {
        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImGui::Image((ImTextureID)_quad->GetTextureId(), contentSize, ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::IsWindowHovered())
        {
            glm::vec2 mousePos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
            glm::vec2 vMin(ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowContentRegionMin().y);
            vMin += glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);

            // Transform window to image coords
            mousePos -= vMin;
            mousePos.y = contentSize.y - mousePos.y;
            _shaderProgram->SendUniform("iMouse", GL_FLOAT_VEC4, (void*)glm::value_ptr(glm::vec4(mousePos, 0, 0)));
        }

        if (contentSize.x != _quad->GetSize().x || contentSize.y != _quad->GetSize().y) {
            _quad->SetResolution(contentSize.x, contentSize.y);
            _shaderProgram->SendUniform("iResolution", GL_FLOAT_VEC3, (void*)glm::value_ptr(_quad->GetSize()));
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void Scape::ShaderDisplayLayer::OnShaderLink()
{
    _iTime = 0;
    _iFrame = 0;
    
    _shaderProgram->SendUniform("iTime", GL_FLOAT, &_iTime);
    _shaderProgram->SendUniform("iResolution", GL_FLOAT_VEC3, (void*)glm::value_ptr(_quad->GetSize()));
}
