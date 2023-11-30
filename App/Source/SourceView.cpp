#include "SourceView.h"

SourceView::SourceView(std::string filePath)
{
    _filePath = filePath;

    SyncFileContent();
}

SourceView::~SourceView()
{

}

void SourceView::Update()
{
    if (ImGui::Begin(_filePath.c_str(), NULL, ImGuiWindowFlags_MenuBar))
    {
        ImGui::TextUnformatted(_fileContent.c_str());

    } ImGui::End();
}

bool SourceView::SyncFileContent()
{
    std::ifstream file(_filePath.c_str());
    if (file.fail())
    {
        Logger::LogMessageConsole(Logger::Severity::Fail, __FILE__, __LINE__, "Something went wrong when opening " + _filePath + " to edit file.\n");
        return false;
    }

    // Read file into content
    _fileContent = std::string(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
    file.close();

    return true;
}

bool SourceView::SetFilePath(std::string filePath)
{
    _filePath = filePath;
    return SyncFileContent();
}
