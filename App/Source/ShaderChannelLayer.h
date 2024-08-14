#pragma once
#include <Layer.h>
#include <Shader.h>

#include <imgui.h>

namespace Scape
{
	class ShaderChannelsLayer : public Layer
	{
	public:
		ShaderChannelsLayer(std::shared_ptr<Shader> shader);
		~ShaderChannelsLayer() override { OnDetach(); }

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(double time, double timeStep)  override;
		void OnFixedUpdate(double fixedTimeStep) override;
		void OnImGuiRender() override;
		void OnEvent(std::shared_ptr<Event> event) override;
	private:
		enum ChannelType
		{
			None =0,
			BufferA = 1,
			BufferB,
			BufferC,
			BufferD,
			Texture,
			COUNT
		};

		inline static const char* ChannelTypeNames[ChannelType::COUNT] =
		{
			"None",
			"BufferA",
			"BufferB",
			"BufferC",
			"BufferD",
			"Texture"
		};

		struct Channel
		{
			ChannelType Type;
			const char* Name;
			const char* TypeName;
			
		};

		void ShowChannelSelector(std::shared_ptr<Channel> channel);

		std::shared_ptr<Shader> _shaderProgram;

		std::shared_ptr<Channel> _channel1;
		std::shared_ptr<Channel> _channel2;
		std::shared_ptr<Channel> _channel3;
		std::shared_ptr<Channel> _channel4;
	};
}