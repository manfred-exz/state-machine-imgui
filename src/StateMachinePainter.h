#pragma once
#include "state_machine_graph.h"
#include "StateMachineLayer.h"
#include "StateMachineCanvas.h"

class StateMachinePainter
{
	StateMachineLayer *currentLayer = nullptr;
	StateMachineCanvas *canvas = nullptr;

public:
	StateMachinePainter(StateMachineLayer* current_layer, StateMachineCanvas* canvas)
		: currentLayer(current_layer),
		  canvas(canvas)
	{}

	void switchLayer(StateMachineLayer *newLayer)
	{
		currentLayer = newLayer;
	}

	void switchCanvas(StateMachineCanvas *newCanvas)
	{
		canvas = newCanvas;
	}

	void drawNodeListBar() const
	{
		if (currentLayer == nullptr || canvas == nullptr)
			return;

		/* node list: left bar*/
		ImGui::BeginChild("node_list", ImVec2(100, 0));
		{
			ImGui::Text("Nodes");
			ImGui::Separator();
			for (auto node_idx = 0; node_idx < currentLayer->states.size(); node_idx++)
			{
				State* node = &currentLayer->states[node_idx];
				ImGui::PushID(node->id);
				/* draw and check if selected */
				if (ImGui::Selectable(node->name, node->id == canvas->getStateSelected()))
					canvas->selectState(node->id);
				/* check if Hovered. */
				if (ImGui::IsItemHovered())
				{
					canvas->hoverStateList(node->id);
					canvas->setContextMenu();
				}
				ImGui::PopID();
			}
		}
		ImGui::EndChild();
	}
};