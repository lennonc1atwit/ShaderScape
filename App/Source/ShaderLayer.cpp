#include "ShaderLayer.h"

namespace Scape {
    void ShaderLayer::OnAttach()
    {
        _quad = std::make_unique<Quad>(glm::vec2(1200, 600));
        _shader = _quad->GetShaderProgram();

        // Bind member func as callback
        _shader->AddOnLink(std::bind(&ShaderLayer::OnShaderLink, this));
    }

    void ShaderLayer::OnDetach()
    {

    }

    void ShaderLayer::OnUpdate(double time, double timeStep)
    {
        float dt = timeStep;
        _shader->SetUniformBuffer("iTimeDelta", &dt);
        _shader->SetUniformBuffer("iFrame", &_iFrame);

        _quad->SetTime((float)time);
        if(IsShaderLoaded())
            _quad->Render(true);

        _iFrame++;
    }

    void ShaderLayer::OnFixedUpdate(double fixedTimeStep)
    {
        // Hot reload doesn't need to check every frame 
        if (!IsShaderLoaded() || _lastSaveTime >= GetLastWriteTime())
            return;

        LoadShaderFromPath(_selectedShaderFilePath);
    }

    void ShaderLayer::OnImGuiRender()
    {
        if (ImGui::Begin("Settings")) 
        {
            ShowBuilder();

            if (IsShaderLoaded())
                ShowUniforms();
        }
        ImGui::End();

        if (IsShaderLoaded())
            ShowOutput();

        ShowFileErrorPopup();
    }

    void ShaderLayer::OnEvent(std::shared_ptr<Event> pEvent)
    {
        if (pEvent->GetType() == Event::Type::DropFile)
        {
            DropEvent* pDropEvent = dynamic_cast<DropEvent*>(pEvent.get());
            if (!pDropEvent || !LoadShaderFromPath(pDropEvent->GetPaths()[0]))
                _showError = true;
        }

        else if (pEvent->GetType() == Event::Type::Cursor)
        {
            CursorEvent* pCursorEvent = dynamic_cast<CursorEvent*>(pEvent.get());
            if (!pCursorEvent)
                _showError = true;

            glm::vec4 mouseData = glm::vec4(0);
            
        }
    }

    bool ShaderLayer::LoadShaderFromPath(std::string filePath)
    {
        std::ifstream shaderFile(filePath);
        if (shaderFile.fail())
        {
            _errorMsg = "Something went wrong when opening " + filePath + "The file may not exist or is currently inaccessible.\n";
            Logger::LogMessageConsole(Logger::Severity::Fail, __FILE__, __LINE__, _errorMsg);
            return false;
        }
        // Read file
        std::string source = std::string(std::istreambuf_iterator<char>(shaderFile), (std::istreambuf_iterator<char>()));
        shaderFile.close();

        GLuint newShader = _shader->CreateShader(source, GL_FRAGMENT_SHADER);
        if (newShader == 0xFFFFFFFF) {
            _errorMsg = "Error Compiling Shader.";
            return false;
        }

        _shader->DeleteShader(_curShader);
        _curShader = newShader;
        _shader->AttachShader(_curShader);
        _shader->Link();
        glfwSetTime(0.0f);
        _iFrame = 0;

        // If everything went okay update last write time 
        _selectedShaderFilePath = filePath;
        _selectedShaderFileName = filePath.substr(filePath.find_last_of("/\\") + 1);
        _lastSaveTime = GetLastWriteTime();
        
        return true;
    }

    void ShaderLayer::UnLoadShader()
    {
        _selectedShaderFileName = "None";

        _shader->DetachShader(_curShader);
        _shader->DeleteShader(_curShader);
        _shader->Link();

        _curShader = 0;
    }

    double ShaderLayer::GetLastWriteTime()
    {
        return (double)std::filesystem::last_write_time(_selectedShaderFilePath).time_since_epoch().count();
    }

    void ShaderLayer::OnShaderLink()
    {
        auto activeUniforms = _shader->GetActiveUniforms();

        // Erase old entries
        std::erase_if(_uniformData, [activeUniforms](std::pair<std::string, std::shared_ptr<UniformUiData>> entry) {
            std::string name = entry.first;
            return activeUniforms.find(name) == activeUniforms.end() || entry.second->Type != activeUniforms.at(name)->GetType();
        });

        // Add new ones
        for (auto const& entry : activeUniforms)
        {
            if (_uniformData.find(entry.first) == _uniformData.end()) {
                _uniformData[entry.first] = std::make_unique<UniformUiData>(entry.second->GetType(), entry.second->GetUniformTypeName(), entry.second->GetName());
                switch (entry.second->GetDataType())
                {
                case Uniform::DataType::Float:      *(float*)(_uniformData[entry.first]->Max) = 100.f; break;
                case Uniform::DataType::Double:     *(double*)(_uniformData[entry.first]->Max) = 100.0; break;
                case Uniform::DataType::Signed32:   *(int*)(_uniformData[entry.first]->Max) = 100;   break;
                case Uniform::DataType::UnSigned32: *(unsigned int*)(_uniformData[entry.first]->Max) = 100;   break;
                }
            }
        }
    }

    static std::vector<std::string> FetchAvailableShaders()
    {
         std::vector fragmentShaderFileNames = std::vector<std::string>();
        for (const auto& file : std::filesystem::directory_iterator(Scape::Shader::SHADER_DIR))
        {
            // Extract file name and extension
            std::string filename = file.path().filename().string();
            std::string ext = file.path().extension().string();

            // Make its a fragment shader extension
            if (Scape::Shader::SUPPORTED_EXTENSIONS.count(ext) == 0)
                continue;

            fragmentShaderFileNames.push_back(filename);
        }

        return fragmentShaderFileNames;
    }

    void ShaderLayer::ShowBuilder()
    {
        // Notice Text
        //ImGui::Text("This Program only supports fragment shaders.\nSupported File extensions: .f, .frag, .fragment");
        ImGui::Dummy(ImVec2(25, 25));

        // Shader file selection
        // TODO make this not happen each frame
        std::vector shaderFileNames = FetchAvailableShaders();
        if (ImGui::BeginCombo("Source File", _selectedShaderFileName.c_str()))
        {
            // Case for deselecting shader
            bool none_selected = !IsShaderLoaded();
            if (ImGui::Selectable("None", none_selected) && !none_selected)
                UnLoadShader();
            
            if (none_selected)
                ImGui::SetItemDefaultFocus();

            // Selecting shader
            for (std::string fileName : shaderFileNames)
            {
                bool is_selected = (_selectedShaderFileName == fileName);
                if (ImGui::Selectable(fileName.c_str(), is_selected))
                {
                    if (!LoadShaderFromPath(Shader::SHADER_DIR + fileName))
                        _showError = true;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();
        ImGui::Dummy(ImVec2(25, 25));
    }

    void ShaderLayer::ShowUniforms()
    {
        ImGui::SeparatorText("Active uniforms");
        ImVec2 windowSize = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0, 0.5));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        for (const auto& [name, uniform] : _shader->GetActiveUniforms())
        {
            std::shared_ptr<UniformUiData> data = _uniformData[name];
            
            ShowUniformConfig(name.c_str(), uniform, data, windowSize);
            ImGui::SameLine(0.0);
            ImGui::Text("%s", name.c_str());
            ImGui::SameLine(ImGui::GetWindowWidth() - data->TypeOffset);
            ImGui::Text(data->TypeString);
            
            ImGui::SetNextItemWidth(windowSize.x);

            bool edited = false;
            if (data->IsColor)
            {
                float* bufferData = (_shader->GetUniformBuffer<float>(name));
                edited = uniform->GetType() == GL_FLOAT_VEC3 ? ImGui::ColorEdit3(data->WidgetID.c_str(), bufferData, 0) : ImGui::ColorEdit4(data->WidgetID.c_str(), bufferData, 0);
            }
            else {
                glm::vec2 uniformDim = uniform->GetComponentCount();
                char* bufferData = uniform->GetBuffer<char>(); // char so we can iterate
                size_t stride = uniform->GetBufferSize() / (size_t)uniformDim.y;

                for (int i = 0; i < uniformDim.x; i++) {
                    std::string sliderID = data->WidgetID + std::to_string(i);
                    edited = ImGui::SliderScalarN(sliderID.c_str(), uniform->GetDataType(), &bufferData[stride * i], (int)uniformDim.y, data->Min, data->Max, UNIFORM_FORMAT_MAP[uniform->GetDataType()]);
                }
            }
            
            ImGui::Separator();

            if (edited) _shader->SendUniform(name);
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    void ShaderLayer::ShowUniformConfig(const char* name, std::shared_ptr<Uniform> uniform, std::shared_ptr<UniformUiData> data, ImVec2 windowSize)
    {
        
        if (ImGui::Button(name, ImVec2(windowSize.x, 24)))
            ImGui::OpenPopup(name);

        if (ImGui::BeginPopup(name))
        {
            ImGui::Text(name);
            ImGui::Separator();

            bool couldBeColor = uniform->GetType() == GL_FLOAT_VEC3 || uniform->GetType() == GL_FLOAT_VEC4;
            if (!data->IsColor || !couldBeColor)
            {
                ImGui::InputScalar(data->MinWidgetId.c_str(), uniform->GetDataType(), data->Min);
                ImGui::InputScalar(data->MaxWidgetId.c_str(), uniform->GetDataType(), data->Max);
            }

            if (couldBeColor)
            {
                ImGui::Separator();
                ImGui::Checkbox(data->ColorWidgetId.c_str(), &data->IsColor);
            }

            ImGui::EndPopup();
        }
    }

    void ShaderLayer::ShowOutput()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("Output"))
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
                _shader->SetUniformBuffer("iMouse", glm::value_ptr(glm::vec4(mousePos, 0, 0)));
            }

            if (contentSize.x != _quad->GetSize().x || contentSize.y != _quad->GetSize().y)
                _quad->SetResolution(contentSize.x, contentSize.y);
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void ShaderLayer::ShowFileErrorPopup()
    {
        if (_showError) {
            ImGui::OpenPopup("FileErrorPopup");
            _showError = false;
        }

        if (ImGui::BeginPopup("FileErrorPopup"))
        {
            ImGui::Text("An Error occured with this file");
            ImGui::Text(_errorMsg.c_str());
            ImGui::EndPopup();
        }
    }
}