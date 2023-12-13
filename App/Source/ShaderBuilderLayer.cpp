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

    _attachedShaderFiles = std::map<GLenum, std::shared_ptr<ShaderFileData>>();

    _attachedShaderFiles[GL_FRAGMENT_SHADER] = std::make_shared<ShaderFileData>(ShaderFileData{ 0xFFFFFFFF, 0, "N/A", "None", 0.0 });
    // _attachedShaderFiles[GL_COMPUTE_SHADER] = std::make_shared<ShaderFileData>(0xFFFFFFFF, "N/A", "None", 0.0);

    BUILTIN_SOURCE = GetShaderSource(SHADER_DIR + BUILTIN_SHADER);
}

void Scape::ShaderBuilderLayer::OnDetach()
{

}

void Scape::ShaderBuilderLayer::OnUpdate(double time, double timeStep)
{

}

void Scape::ShaderBuilderLayer::OnFixedUpdate(double fixedTimeStep)
{
    RefreshFileCache();
    CheckForFileUpdates(_attachedShaderFiles[GL_FRAGMENT_SHADER]);
}

void Scape::ShaderBuilderLayer::OnImGuiRender()
{
	if (ImGui::Begin("Shader Builder"))
	{
        ImGui::SeparatorText("Select File(s).");
        ShowFileDropDown("Fragment Shader", _attachedShaderFiles[GL_FRAGMENT_SHADER]->FileName, _availableFragmentShaders, GL_FRAGMENT_SHADER);
        // ShowFileDropDown("Compute Shader",  _attachedShaderFiles[GL_COMPUTE_SHADER]->FileName,  _availableComputeShaders,  GL_COMPUTE_SHADER);
        ShowFileErrorPopup();
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
            std::shared_ptr<ShaderFileData> data = _attachedShaderFiles[shaderType];
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
                TryLoadShaderFromFile(SHADER_DIR + fileName, shaderType);
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

bool Scape::ShaderBuilderLayer::TryLoadShaderFromFile(const std::string& filePath, GLenum type)
{
    std::string source = BUILTIN_SOURCE + "\"" +filePath+ "\"\n" + GetShaderSource(filePath);
    GLuint newShader = _shaderProgram->CreateShader(source.c_str(), type);
    
    // Check if we have an error
    std::string errorMsg = _shaderProgram->GetShaderLog(newShader);
    if (errorMsg.compare("") != 0)
    {
        _errorMsg = errorMsg;
        _showError = true;
        return false;
    }

    std::shared_ptr<ShaderFileData> data = _attachedShaderFiles[type];

    _shaderProgram->DetachShader(data->ShaderId);
    data->ShaderId = newShader;
    _shaderProgram->AttachShader(data->ShaderId);
    _shaderProgram->Link();
    _shaderProgram->DeleteShader(data->ShaderId); // Marks for cleanup later when not in use

    // Update data
    data->ShaderType = type;
    data->FilePath = filePath;
    data->FileName = filePath.substr(filePath.find_last_of("/\\") + 1);
    data->FileTimeStamp = (double)std::filesystem::last_write_time(filePath).time_since_epoch().count();

    return true;
}

void Scape::ShaderBuilderLayer::CheckForFileUpdates(std::shared_ptr<ShaderFileData> data)
{
    struct stat sb; // Check that file exists
    if (stat(data->FilePath.c_str(), &sb) != 0 || (sb.st_mode & S_IFDIR))
        return;

    double lastWrite = std::filesystem::last_write_time(data->FilePath).time_since_epoch().count();
    if (lastWrite > data->FileTimeStamp)
    {
        TryLoadShaderFromFile(data->FilePath, data->ShaderType);
        data->FileTimeStamp = lastWrite;
    }
}

void Scape::ShaderBuilderLayer::RefreshFileCache()
{
    _availableComputeShaders = FetchFilesWithExtensions(SHADER_DIR, COMPUTE_EXTENSIONS);
    _availableFragmentShaders = FetchFilesWithExtensions(SHADER_DIR, FRAGMENT_EXTENSIONS);
}

void Scape::ShaderBuilderLayer::ShowFileErrorPopup()
{
    if (_showError) {
        ImGui::OpenPopup("FileErrorPopup");
        _showError = false;
    }

    if (ImGui::BeginPopup("FileErrorPopup"))
    {
        ImGui::Text("An Error occured when compiling the shader!");
        ImGui::Separator();
        ImGui::Text(_errorMsg.c_str());
        ImGui::EndPopup();
    }
}
