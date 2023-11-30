// Local headers
#include <program.h>
#include <iostream>
Application::Application()
{
    _instance = this;
    _layers = std::vector<std::shared_ptr<Scape::Layer>>();

    // Create Window and our UI layer
    _window = std::make_shared<Window>("ShaderScape", 1200, 600);
    _imGuiLayer = std::make_unique<Scape::ImGuiLayer>(_window);
}

Application::~Application()
{
    // Layers detach in deconstructor
    //for (auto layer : _layers)
    //    layer->OnDetach();

    //_imGuiLayer->OnDetach();
}

void Application::Run()
{
    while (!_window->ShouldClose())
    {
        // See how much time the previous frame took
        _currentTime = glfwGetTime();
        _deltaTime = _currentTime - _lastTime;
        _lastTime = _currentTime;

        // Update fixed timesteps depending on how much time has passed since last tick and how many ticks should have passed
        _accumulator += _deltaTime;
        while (_accumulator >= _fixedDeltaTime)
        {
            for (auto layer : _layers)
                layer->OnFixedUpdate(_fixedDeltaTime);
            _accumulator -= _fixedDeltaTime;
        }

        //TODO profile sections: Event, Render, Update, Fixed Update

        // Handle events
        //TODO event system
        glfwPollEvents();
        for (auto layer : _layers)
            layer->OnEvent();

        // Begin Render
        _window->BeginFrame();
        
            // Raw Drawing
            for (auto layer : _layers)
                layer->OnUpdate(_currentTime, _deltaTime);

            // Gui
            _imGuiLayer->Begin();
            
                for (auto layer : _layers)
                    layer->OnImGuiRender();
            
            _imGuiLayer->End();
        
        // Finish Render
        _window->EndFrame();
    }
}

void Application::OnEvent()
{
    
}

