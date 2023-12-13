#include "ShaderBuilderLayer.h"

static std::string GetShaderSource(std::string filePath)
{

    std::ifstream shaderFile(filePath);
    if (shaderFile.fail())
    {
        Logger::LogMessageConsole(Logger::Severity::Fail, __FILE__, __LINE__, "_errorMsg");
        return "false";
    }
    // Read file
    std::string source = std::string(std::istreambuf_iterator<char>(shaderFile), (std::istreambuf_iterator<char>()));
    shaderFile.close();

    /*Scape::GlslTokenizer tokenizer(source.c_str(), source.length());
    Scape::GlslTokenizer::Token token{.Type = Scape::GlslTokenizer::Type::Invalid };
    while (token.Type != Scape::GlslTokenizer::Type::Eof)
    {
        token = tokenizer.Next();
        printf("%.*s %d\n", (int)token.TextLen, token.Text, token.Type);
    }*/

    return source;
}

Scape::ShaderBuilderLayer::ShaderBuilderLayer(std::shared_ptr<Shader> shader) : Layer("ShaderBuilderLayer")
{
    _shaderProgram = shader;
    OnAttach();
}

void Scape::ShaderBuilderLayer::OnAttach()
{
    _availableFragmentShaders = std::vector<std::string>();
    _availableComputeShaders = std::vector<std::string>();

    _attachedShaderFiles = std::map<GLenum, ShaderFileData*>();

    _attachedShaderFiles[GL_FRAGMENT_SHADER] = new ShaderFileData{ 0xFFFFFFFF, "N/A", "None", 0.0 };
    _attachedShaderFiles[GL_COMPUTE_SHADER] = new ShaderFileData{ 0xFFFFFFFF, "N/A", "None", 0.0 };

    BUILTIN_SOURCE = GetShaderSource(SHADER_DIR + BUILTIN_SHADER);
}

void Scape::ShaderBuilderLayer::OnDetach()
{
    delete _attachedShaderFiles[GL_FRAGMENT_SHADER];
    delete _attachedShaderFiles[GL_COMPUTE_SHADER];
}

void Scape::ShaderBuilderLayer::OnUpdate(double time, double timeStep)
{

}

void Scape::ShaderBuilderLayer::OnFixedUpdate(double fixedTimeStep)
{
    RefreshFileCache();
}

void Scape::ShaderBuilderLayer::OnImGuiRender()
{
	if (ImGui::Begin("Shader Builder"))
	{
        ImGui::SeparatorText("Select File(s).");
        ShowFileDropDown("Fragment Shader", _attachedShaderFiles[GL_FRAGMENT_SHADER]->FileName, _availableFragmentShaders, GL_FRAGMENT_SHADER);
        ShowFileDropDown("Compute Shader",  _attachedShaderFiles[GL_COMPUTE_SHADER]->FileName,  _availableComputeShaders,  GL_COMPUTE_SHADER);
	}
	ImGui::End();
}

void Scape::ShaderBuilderLayer::OnEvent(std::shared_ptr<Event> event)
{

}

std::vector<std::string> FetchFilesWithExtensions(std::string dir, std::set<std::string> extensions)
{
    std::vector fragmentShaderFileNames = std::vector<std::string>();
    for (const auto& file : std::filesystem::directory_iterator(dir))
    {
        // Extract file name and extension
        std::string filename = file.path().filename().string();
        std::string ext = file.path().extension().string();

        // Make its a fragment shader extension
        if (extensions.count(ext) == 0)
            continue;

        fragmentShaderFileNames.push_back(filename);
    }

    return fragmentShaderFileNames;
}

void Scape::ShaderBuilderLayer::ShowFileDropDown(std::string title, std::string& selectedItem, std::vector<std::string> options, GLenum shaderType)
{
    ImGui::Text(title.c_str());
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::BeginCombo(title.c_str(), selectedItem.c_str()))
    {
        // Case for deselecting file
        bool none_selected = (selectedItem.compare("None") == 0);
        if (ImGui::Selectable("None", none_selected) && !none_selected)
        {
            ShaderFileData* data = _attachedShaderFiles[shaderType];
            _shaderProgram->DetachShader(data->ShaderId);
            _shaderProgram->Link();
            data->ShaderId = 0;
        }

        if (none_selected)
            ImGui::SetItemDefaultFocus();

        // Selecting file
        for (std::string fileName : options)
        {
            bool is_selected = (selectedItem.compare(fileName) == 0);
            if (ImGui::Selectable(fileName.c_str(), is_selected))
            {
                selectedItem = fileName;
                if (LoadShaderFromFile(SHADER_DIR + fileName, shaderType))
                    printf("loaded\n");
                else
                    printf("error\n");
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

bool Scape::ShaderBuilderLayer::LoadShaderFromFile(const std::string& filePath, GLenum type)
{
    GLuint newShader = _shaderProgram->CreateShader(BUILTIN_SOURCE + GetShaderSource(filePath), type);
    
    if (newShader == 0xFFFFFFFF) {
        _errorMsg = "Error Compiling Shader.";
        return false;
    }

    ShaderFileData* data = _attachedShaderFiles[type];

    _shaderProgram->DetachShader(data->ShaderId);
    data->ShaderId = newShader;
    _shaderProgram->AttachShader(data->ShaderId);
    _shaderProgram->Link();
    _shaderProgram->DeleteShader(data->ShaderId); // Marks for cleanup later when not in use

    // Update data
    data->FilePath = filePath;
    data->FileName = filePath.substr(filePath.find_last_of("/\\") + 1);
    data->FileTimeStamp = (double)std::filesystem::last_write_time(filePath).time_since_epoch().count();

    return true;
}

void Scape::ShaderBuilderLayer::RefreshFileCache()
{
    _availableComputeShaders = FetchFilesWithExtensions(SHADER_DIR, COMPUTE_EXTENSIONS);
    _availableFragmentShaders = FetchFilesWithExtensions(SHADER_DIR, FRAGMENT_EXTENSIONS);
}
