#include "StateMachinePainter.h"

void StateMachinePainter::drawNodeListBar() const
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

void StateMachinePainter::drawSideBar() const
{
	/* draw param panel: side bar */
	ImGui::BeginChild("param panel", ImVec2(200, 0));
	{
		if (canvas->getStateSelected() >= 0) {
			ImGui::Text("State");
			State& _state = currentLayer->states[canvas->getStateSelected()];
			{
				ImGui::TextWrapped("Name: ");
				ImGui::SameLine();
				ImGui::InputText("", _state.name, sizeof(_state.name) / sizeof(_state.name[0]));

				ImGui::TextWrapped("Type: %s", _state.typeStr());
			}

			/* todo: should add multi transition support */
			if (ImGui::CollapsingHeader("Transitions", nullptr, true, true))
			{
				for (TransitionID _transID : _state.transitions)
				{
					auto _trans = currentLayer->getTransition(_transID);
					ImGui::Text("%s -> %s", currentLayer->states[_trans.fromID].name, currentLayer->states[_trans.toID].name);
				}
				if (_state.transitions.size() == 0)
					ImGui::Text("Empty");
			}
		}

		if (canvas->getTransSelected() >= 0)
		{
			ImGui::Text("Transition");
			Transition& _trans = currentLayer->getTransition(canvas->getTransSelected());
			ImGui::Text("%s -> %s", currentLayer->states[_trans.fromID].name, currentLayer->states[_trans.toID].name);
			ImGui::Checkbox("solo:", &_trans.solo);
			ImGui::Checkbox("mute:", &_trans.mute);
			ImGui::Checkbox("hasExit:", &_trans.hasExit);
		}
	}
	ImGui::EndChild();

}