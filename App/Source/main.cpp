// Local headers
#include "program.h"
#include "ShaderLayer.h"

// Standard headers
#include <cstdlib>
#include <ProfilerLayer.h>


int main(int argc, char* argv[])
{
    std::unique_ptr<Application> app = std::make_unique<Application>();
    app->InstanceLayer<Scape::ShaderLayer>();
    app->InstanceLayer<Scape::ProfilerLayer>();

    app->Run();

    return EXIT_SUCCESS;
}
