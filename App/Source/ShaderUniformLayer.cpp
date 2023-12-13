#include "ShaderUniformLayer.h"

Scape::ShaderUniformLayer::ShaderUniformLayer(std::shared_ptr<Shader> shader) : Layer("ShaderUniformLayer")
{
    _shaderProgram = shader;
    OnAttach();
}

void Scape::ShaderUniformLayer::OnAttach()
{
    _uniformData = std::map<std::string, std::shared_ptr<UniformUIData>>();
    _shaderProgram->AddOnLink(std::bind(&ShaderUniformLayer::OnShaderProgramLink, this));
}

void Scape::ShaderUniformLayer::OnImGuiRender()
{
    if(ImGui::Begin("Settings") ) {
        ImGui::SeparatorText("Active uniforms");
        ImGui::PushStyleVar(  ImGuiStyleVar_ButtonTextAlign, ImVec2{ 0.0f, 0.5f });
        ImGui::PushStyleColor(ImGuiCol_Button,               ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,        ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,         ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

        bool edited = false;
        float lineWidth = ImGui::GetContentRegionAvail().x;
        for (const auto& [name, data] : _uniformData)
        {
            std::shared_ptr<UniformUIData> uniformData = _uniformData[name];

            ShowUniformConfig(name.c_str(), uniformData, lineWidth);

            ImGui::SameLine(0.0);
            ImGui::Text("%s", name.c_str());
            ImGui::SameLine(lineWidth - uniformData->TypeStringOffset);
            ImGui::Text(uniformData->TypeString);

            ImGui::SetNextItemWidth(lineWidth);
            if (uniformData->IsColor)
            {
                float* color = reinterpret_cast<float*>(uniformData->Buffer);
                edited = uniformData->Dim.x == 3 ? ImGui::ColorEdit3(uniformData->WidgetID, color, ImGuiColorEditFlags_AlphaPreviewHalf) : ImGui::ColorEdit4(uniformData->WidgetID, color, ImGuiColorEditFlags_AlphaPreviewHalf);
            }
            else
            {
                char* buffer = reinterpret_cast<char*>(uniformData->Buffer);// cast char so we can iterate byte wise (TODO: double check that a char is 1 byte on system)
                for (int i = 0; i < uniformData->Dim.x; i++) 
                {
                    std::string sliderID = uniformData->WidgetID + std::to_string(i);
                    edited |= ImGui::SliderScalarN(sliderID.c_str(), uniformData->DataType, &buffer[uniformData->SliderStride * i], (int)uniformData->Dim.y, uniformData->SliderMin, uniformData->SliderMax, uniformData->DisplayFormat, uniformData->SliderFlags);
                }
            }

            ImGui::Separator();
            if (edited)
                _shaderProgram->SendUniform(uniformData->Location, uniformData->Type, uniformData->Buffer);
            edited = false; // Reset for next uniform
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
    ImGui::End();
}

void Scape::ShaderUniformLayer::ShowUniformConfig(const char* name, std::shared_ptr<UniformUIData> data, float lineWidth)
{

    if (ImGui::Button(name, ImVec2(lineWidth, 24)))
        ImGui::OpenPopup(name);

    if (ImGui::BeginPopup(name))
    {
        ImGui::Text(name);
        char* bufferData = reinterpret_cast<char*>(data->Buffer); // char so we can iterate
        bool edited = false;
        for (int i = 0; i < data->Dim.x; i++)
        {
            std::string sliderID = data->WidgetID + std::to_string(i);
            edited |= ImGui::InputScalarN(sliderID.c_str(), data->DataType, &bufferData[data->SliderStride * i], (int)data->Dim.y, 0, 0, data->DisplayFormat, 0);
        }
        if (edited)
            _shaderProgram->SendUniform(data->Location, data->Type, data->Buffer);

        ImGui::Separator();
        if (!data->IsColor)
        {
            ImGui::InputScalar(data->MinWidgetId, data->DataType, data->SliderMin, 0, 0, data->DisplayFormat, 0);
            ImGui::InputScalar(data->MaxWidgetId, data->DataType, data->SliderMax, 0, 0, data->DisplayFormat, 0);
        }

        ImGui::CheckboxFlags("Use Log Scale", &data->SliderFlags, ImGuiSliderFlags_Logarithmic);

        if (data->CouldBeColor)
            ImGui::Checkbox(data->ColorWidgetId, &data->IsColor);

        ImGui::EndPopup();
    }
}

void Scape::ShaderUniformLayer::OnShaderProgramLink()
{
    auto uniforms = _shaderProgram->GetActiveUniforms();

    std::erase_if(_uniformData, [uniforms](std::pair<std::string, std::shared_ptr<UniformUIData>> entry) {
        std::string name = entry.first;
        if (uniforms.find(name) == uniforms.end() || entry.second->Type != uniforms.find(name)->second.Type)
        {
            free(entry.second->Buffer);
            free(entry.second->SliderMax);
            free(entry.second->SliderMin);
            return true;
        }
        return false;
    });

    for (auto &[name, info] : uniforms)
    {
        if (_uniformData.find(name) != _uniformData.end())
            UpdateUniformLocation(name);
        if (!IsBuiltInUniform(name, info.Type))
            AddNewUniform(name.c_str(), info);
    }
}

void Scape::ShaderUniformLayer::UpdateUniformLocation(std::string name)
{
    if (_uniformData.find(name) == _uniformData.end())
        return;
    _uniformData[name]->Location = _shaderProgram->GetUniformLocation(name.c_str());
}

void Scape::ShaderUniformLayer::AddNewUniform(const char* name, Shader::UniformInfo info)
{
    auto data = std::make_shared<UniformUIData>();

    data->Type = info.Type;
    data->Location = info.Location;
    
    data->TypeString = GL_TYPE_INFO_MAP[data->Type].Name;
    data->Dim = GL_TYPE_INFO_MAP[data->Type].Dim;
    data->DataType = GL_TYPE_INFO_MAP[data->Type].ComponentDataType;
    data->BufferSize = GL_TYPE_INFO_MAP[data->Type].Size;
    data->Buffer = malloc(data->BufferSize);
    // Load any defaults from the shader program
    _shaderProgram->RetreiveUniform(data->Location, data->Type, data->Buffer);

    data->TypeStringOffset = ImGui::CalcTextSize(data->TypeString).x;
    data->DisplayFormat = UNIFORM_FORMAT_MAP[data->DataType];

    // Setup our ui labels
    snprintf(data->WidgetID,      sizeof(char) * 16, "##%s",      name);
    snprintf(data->MinWidgetId,   sizeof(char) * 16, "Min##%s",   name);
    snprintf(data->MaxWidgetId,   sizeof(char) * 16, "Max##%s",   name);
    snprintf(data->ColorWidgetId, sizeof(char) * 16, "Color##%s", name);

    data->SliderMin = calloc(1, sizeof(double));
    data->SliderMax = calloc(1, sizeof(double));
    data->SliderFlags = ImGuiSliderFlags_None;

    // This is how many bytes we jump for each new imgui slider we draw
    // Each new slider is a row of an NxMmatrix and needs to be offset by M members from the last sliders data
    data->SliderStride = data->BufferSize / (size_t)data->Dim.x;

    data->IsColor = false;
    data->CouldBeColor = data->Type == GL_FLOAT_VEC3 || data->Type == GL_FLOAT_VEC4;
    data->LogScale = false;
    
    switch (data->DataType)
    {
    case DataType::Float:             *(float*)(data->SliderMax) = *reinterpret_cast<float*>(data->Buffer);        break;
    case DataType::Double:           *(double*)(data->SliderMax) = *reinterpret_cast<double*>(data->Buffer);       break;
    case DataType::Signed32:            *(int*)(data->SliderMax) = *reinterpret_cast<int*>(data->Buffer);          break;
    case DataType::UnSigned32: *(unsigned int*)(data->SliderMax) = *reinterpret_cast<unsigned int*>(data->Buffer); break;
    }

    _uniformData[name] = data;
}

bool Scape::ShaderUniformLayer::IsBuiltInUniform(std::string name, GLenum type)
{
    // Check for name match
    if (_builtInUniforms.find(name) == _builtInUniforms.end())
        return false;
    // If types match its a valid builtin
    return _builtInUniforms[name] == type;
}


