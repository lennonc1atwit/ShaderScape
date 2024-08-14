// Local headers
#include "program.h"
#include <ShaderBuilderLayer.h>
#include <ShaderDisplayLayer.h>
#include <ShaderUniformLayer.h>
#include <ShaderChannelLayer.h>
#include <ProfilerLayer.h>

// Standard headers
#include <cstdlib>


int main(int argc, char* argv[])
{
    std::unique_ptr<Application> app = std::make_unique<Application>();

    std::shared_ptr<Scape::Shader> shaderProgram = std::make_shared<Scape::Shader>();
    // Give shader to each layer that needs it
    app->PushLayer(std::make_shared<Scape::ShaderBuilderLayer>(shaderProgram));
    //app->PushLayer(std::make_shared<Scape::ShaderChannelsLayer>(shaderProgram));
    app->PushLayer(std::make_shared<Scape::ShaderDisplayLayer>(shaderProgram));
    app->PushLayer(std::make_shared<Scape::ShaderUniformLayer>(shaderProgram));
    app->PushLayer(std::make_shared<Scape::ProfilerLayer>());

    app->Run();

    return EXIT_SUCCESS;
}
