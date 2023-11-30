#include "ShaderLayer.h"
namespace Scape {
    void ShaderLayer::OnAttach()
    {
        _quad = std::make_unique<Quad>(glm::vec2(1200, 600));
        std::function<void()> callback = std::bind(&ShaderLayer::OnShaderLink, this);
        _quad->GetShaderProgram()->AddOnLink(callback);
    }

    void ShaderLayer::OnDetach()
    {
    }

    void ShaderLayer::OnUpdate(double time, double timeStep)
    {
        _quad->SetTime(time);
        _quad->Render(true);
    }

    void ShaderLayer::OnFixedUpdate(double fixedTimeStep)
    {
    }

    void ShaderLayer::OnImGuiRender()
    {
        ImGui::Begin("Settings");

        ShowBuilder();

        if (_quad->GetShaderProgram()->GetUniformCount() > 0)
            ShowUniforms();

        ImGui::End();

        if (!ImGui::Begin("Shader") || !_quad->WindowedRender)
        {
            ImGui::End();
            return;
        }

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImGui::Image(reinterpret_cast<ImTextureID>(_quad->GetTextureId()), contentSize, ImVec2(0, 1), ImVec2(1, 0));

        if (contentSize.x != _quad->GetSize().x || contentSize.y != _quad->GetSize().y)
            _quad->SetResolution(contentSize.x, contentSize.y);


        ImGui::End();
    }

    void ShaderLayer::OnEvent()
    {
    }

    void ShaderLayer::OnShaderLink()
    {
        auto activeUniforms = _quad->GetShaderProgram()->GetActiveUniforms();

        // Erase old entries
        for (auto curEntry = _uniformData.begin(), last = _uniformData.end(); curEntry != last;)
        {
            if (activeUniforms.find(curEntry->first) == activeUniforms.end() || curEntry->second->Type != activeUniforms[curEntry->first]->GetType())
                curEntry = _uniformData.erase(curEntry);
            else
                ++curEntry;
        }

        // Add new ones
        for (auto const& entry : activeUniforms)
        {
            if (_uniformData.find(entry.first) == _uniformData.end()) {
                _uniformData[entry.first] = std::make_unique<UniformUiData>(entry.second->GetType());
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
        for (const auto& file : std::filesystem::directory_iterator(SS::Shader::SHADER_DIR))
        {
            // Extract file name and extension
            std::string filename = file.path().filename().string();
            std::string ext = file.path().extension().string();

            // Make its a fragment shader extension
            if (SS::Shader::SUPPORTED_EXTENSIONS.count(ext) == 0)
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
            bool none_selected = (_selectedShaderFileName == "None");
            if (ImGui::Selectable("None", none_selected) && !none_selected)
            {
                _selectedShaderFileName = "None";
            }
            if (none_selected)
                ImGui::SetItemDefaultFocus();

            // Selecting shader
            for (int n = 0; n < shaderFileNames.size(); n++)
            {
                bool is_selected = (_selectedShaderFileName == shaderFileNames[n]);
                if (ImGui::Selectable(shaderFileNames[n].c_str(), is_selected))
                {
                    _selectedShaderFileName = shaderFileNames[n];
                    std::string filePath = SS::Shader::SHADER_DIR + _selectedShaderFileName;
                    std::ifstream shaderFile(filePath);
                    if (shaderFile.fail())
                    {
                        Logger::LogMessageConsole(Logger::Severity::Fail, __FILE__, __LINE__,
                            "Something went wrong when opening " + filePath + "The file may not exist or is currently inaccessible.\n");
                        break;
                    }
                    // Read file
                    std::string source = std::string(std::istreambuf_iterator<char>(shaderFile), (std::istreambuf_iterator<char>()));
                    shaderFile.close();

                    _quad->GetShaderProgram()->DeleteShader(_curShader);
                    _curShader = _quad->GetShaderProgram()->CreateShader(source, GL_FRAGMENT_SHADER);
                    _quad->GetShaderProgram()->AttachShader(_curShader);
                    _quad->GetShaderProgram()->Link();
                }
                if (!none_selected && is_selected)
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

        for (const auto& [name, uniform] : _quad->GetShaderProgram()->GetActiveUniforms())
        {
            std::shared_ptr<UniformUiData> data = _uniformData[name];

            std::string widgetID = "##" + name;
            std::string minWidgetId = "Min" + widgetID;
            std::string maxWidgetId = "Max" + widgetID;
            std::string colorWidgetId = "Color" + widgetID;

            ImGui::Text("%s", name.c_str());

            char typeString[24];
            sprintf(typeString, "(%s)", uniform->GetUniformTypeName());
            ImVec2 textSize = ImGui::CalcTextSize(typeString);

            ImGui::SameLine(ImGui::GetWindowWidth() - (textSize.x + 8));

            ImGui::Text(typeString);
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right, false) && ImGui::IsItemHovered())
                ImGui::OpenPopup(name.c_str());

            if (ImGui::BeginPopup(name.c_str()))
            {
                ImGui::Text(name.c_str());
                ImGui::Separator();

                bool couldBeColor = uniform->GetType() == GL_FLOAT_VEC3 || uniform->GetType() == GL_FLOAT_VEC4;
                if (!data->IsColor || !couldBeColor) {
                    ImGui::InputScalar(minWidgetId.c_str(), uniform->GetDataType(), data->Min);
                    ImGui::InputScalar(maxWidgetId.c_str(), uniform->GetDataType(), data->Max);
                }

                if (couldBeColor)
                {
                    ImGui::Separator();
                    ImGui::Checkbox(colorWidgetId.c_str(), &data->IsColor);
                }

                ImGui::EndPopup();
            }

            ImGui::SetNextItemWidth(windowSize.x);

            bool edited = false;
            if (data->IsColor)
            {
                float* bufferData = (_quad->GetShaderProgram()->GetUniformBuffer<float>(name));
                edited = uniform->GetType() == GL_FLOAT_VEC3 ? ImGui::ColorEdit3(widgetID.c_str(), bufferData, 0) : ImGui::ColorEdit4(widgetID.c_str(), bufferData, 0);
            }
            else {
                glm::vec2 uniformDim = uniform->GetComponentCount();
                char* bufferData = uniform->GetBuffer<char>(); // char so we can iterate
                size_t stride = uniform->GetBufferSize() / uniformDim.y;

                for (int i = 0; i < uniformDim.x; i++) {
                    std::string sliderID = widgetID + std::to_string(i);
                    edited = ImGui::SliderScalarN(sliderID.c_str(), uniform->GetDataType(), &bufferData[stride * i], uniformDim.y, data->Min, data->Max, UNIFORM_FORMAT_MAP[uniform->GetDataType()]);
                }
            }

            ImGui::Separator();

            if (edited) _quad->GetShaderProgram()->SendUniform(name);
        }
    }
}