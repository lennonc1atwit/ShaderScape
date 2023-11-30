-- premake5.lua
workspace "ShaderScape"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "ShaderScape"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"
LibDir = "../Libs"

group "Dependencies"
   include "Vendor/ImGui"
   include "Vendor/ImPlot"
   include "Vendor/Glad"
   include "Vendor/GLFW"
group ""


group "Core"
	include "Core/Build-Core.lua"
group ""

include "App/Build-App.lua"