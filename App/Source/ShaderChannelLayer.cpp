#include "ShaderChannelLayer.h"

namespace Scape 
{
	ShaderChannelsLayer::ShaderChannelsLayer(std::shared_ptr<Shader> shader): Layer("ShaderChannelsLayer")
	{
		_shaderProgram = shader;
		OnAttach();
	}

	void ShaderChannelsLayer::OnAttach()
	{
		_channel1 = std::make_shared<Channel>(Channel{ None, "iChannel0", "None" });
		_channel2 = std::make_shared<Channel>(Channel{ None, "iChannel1", "None" });
		_channel3 = std::make_shared<Channel>(Channel{ None, "iChannel2", "None" });
		_channel4 = std::make_shared<Channel>(Channel{ None, "iChannel3", "None" });
	}

	void ShaderChannelsLayer::OnDetach()
	{

	}

	void ShaderChannelsLayer::OnUpdate(double time, double timeStep)
	{

	}

	void ShaderChannelsLayer::OnFixedUpdate(double fixedTimeStep)
	{

	}

	void ShaderChannelsLayer::OnImGuiRender()
	{
		if (ImGui::Begin("Channels"))
		{
			ShowChannelSelector(_channel1);
			ImGui::Separator();
			ShowChannelSelector(_channel2);
			ImGui::Separator();
			ShowChannelSelector(_channel3);
			ImGui::Separator();
			ShowChannelSelector(_channel4);
		}
		ImGui::End();
	}

	void ShaderChannelsLayer::OnEvent(std::shared_ptr<Event> event)
	{

	}

	void ShaderChannelsLayer::ShowChannelSelector(std::shared_ptr<Channel> channel)
	{
		if (ImGui::BeginCombo(channel->Name, channel->TypeName))
		{
			for (int i = 0; i < ChannelType::COUNT; i++) 
			{
				bool isSelected = channel->TypeName == ChannelTypeNames[i];
				if (ImGui::Selectable(ChannelTypeNames[i], &isSelected, 0))
					channel->TypeName = ChannelTypeNames[i];
			}
			ImGui::EndCombo();
		}
	}
}
