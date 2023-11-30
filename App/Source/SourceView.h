#pragma once
#include <Logging.h>
#include <imgui.h>

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

class SourceView
{
public:
	SourceView(std::string filePath);
   ~SourceView();

   void Update();
   bool SyncFileContent();
   bool SetFilePath(std::string);

private:
	std::string _filePath;
	std::string _fileContent;
};