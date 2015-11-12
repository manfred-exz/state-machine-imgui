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

void StateMachinePainter::drawCanvas()
{
	/* draw canvas */
	udpateCanvasOrigin();
	ImGui::BeginGroup();
	{
		ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
		ImGui::SameLine(ImGui::GetWindowWidth() - 100);
		ImGui::Checkbox("Show grid", &show_grid);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImColor(60, 60, 70, 200));

		ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
		{
			ImGui::PushItemWidth(120.0f);

			/* get current window draw list, and split into several layers */
			ImDrawList& draw_list = *ImGui::GetWindowDrawList();
			draw_list.ChannelsSplit(canvas->NUM_LAYERS);
			{
				/* draw grids */
				if (show_grid) { drawGrid(draw_list); }

				/* debug */
				if (canvas->node_debug)
				{
					draw_list.ChannelsSetCurrent(canvas->layer(0));	/* draw debug on top */
					ImGui::Text("offset pos %f, %f", canvas_origin.x, canvas_origin.y);
					ImGui::Text("mouse pos: %.2f %.2f", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

					for (std::pair<TransitionID, Transition> pair : currentLayer->transitions)
					{
						auto _trans = pair.second;
						auto from_pos = canvas_origin + currentLayer->states[_trans.fromID].center();
						auto to_pos = canvas_origin + currentLayer->states[_trans.toID].center();
						if (currentLayer->onTransitionLine(from_pos, to_pos, ImGui::GetMousePos()) && canvas->getStateHoveredInScene() < 0) {
							//						canvas->selectTrans(pair.first);
							ImGui::Text("hovered trans id %d", pair.first);
						}
						else
						{
							ImGui::Text("hovered nothing");
						}
					}

					if (canvas->getStateHoveredInScene() >= 0)
					{
						ImGui::Text("hoverd State.name: %s", currentLayer->states[canvas->getStateHoveredInScene()].name);
					}

					if (canvas->getStateSelected() >= 0) {
						State currState = currentLayer->states[canvas->getStateSelected()];
						ImGui::Text("State.name: %s", currState.name);
						for (TransitionID trans_id : currState.transitions)
						{
							auto _trans = currentLayer->getTransition(trans_id);
							ImGui::Text("\ttransition from %d to %d", _trans.fromID, _trans.toID);
						}
					}

					if (canvas->getTransSelected() >= 0)
					{
						Transition currTransition = currentLayer->getTransition(canvas->getTransSelected());
						ImGui::Text("Transition from %d to %d", currTransition.fromID, currTransition.toID);
					}

					if (ImGui::IsAnyItemHovered())
						ImGui::Text("Hovered");
				}

				/* draw states */
				for (State &node : currentLayer->states)
					drawStateNode(draw_list, node);

				/* draw links */
				drawLinks(draw_list);

				/* check if right click on window context menu */
				canvas->checkWindowRightClick();

				/* select the corresponding node if context menu is opened on it */
				if (canvas->isContextMenuOpen())
				{
					ImGui::OpenPopup("context_menu");
					if (canvas->getStateHoveredInList() != -1) {
						canvas->selectState(canvas->getStateHoveredInList());
					}

					if (canvas->getStateHoveredInScene() != -1) {
						canvas->selectState(canvas->getStateHoveredInScene());
					}
				}

				/* Draw unfinished line */
				drawUnfinishedLine(draw_list);
			}
			draw_list.ChannelsMerge();

			/* Draw context menu */
			drawContextMenu();

			// Scrolling
			if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
				scrolling = scrolling - ImGui::GetIO().MouseDelta;

			ImGui::PopItemWidth();
		}
		ImGui::EndChild();

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

	}
	ImGui::EndGroup();
}

void StateMachinePainter::drawStateNode(ImDrawList& draw_list, State& node) const {
	ImGui::PushID(node.id);

	draw_list.ChannelsSetCurrent(canvas->layer(0)); /* foreground */


	//		ImVec2 node_rect_min = canvas_origin + node.pos;
	ImVec2 node_rect_min = node.rectMin(canvas_origin);
	ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);

	/*draw node content*/
	bool old_any_active = ImGui::IsAnyItemActive();
	{
		ImGui::BeginGroup(); // Lock horizontal position
		ImGui::Text("%s", node.name);
		ImGui::ColorEdit3("##color", &node.color.x);
		//				ImGui::Text("NodeSize: [%.2f, %.2f]", node.Size.x, node.Size.y);
		ImGui::EndGroup();
	}
	/* if the above widgets group (text, colorEdit, ...) is clicked, the whole node should also be active. */
	bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());

	/* update node size & pos */
	node.size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
	ImVec2 node_rect_max = node_rect_min + node.size;

	/* draw an invisible button to handle mouse input */
	{
		draw_list.ChannelsSetCurrent(0); // Background
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node.size);		/* invisible button to check if mouse is hovered. */

		/* hovered, don't ask me why "|| node_widgets_active..." */
		if (ImGui::IsItemHovered() || node_widgets_active) {
			canvas->hoverStateScene(node.id);
			canvas->setContextMenu(); /* open right click on item context menu */
		}

		/* select node item */
		bool node_moving_active = ImGui::IsItemActive();
		bool right_click_select = ImGui::IsItemHovered() && ImGui::IsMouseClicked(1);	/* if you hover over the invisible button and right click, it's also select */

		if (node_widgets_active || node_moving_active || right_click_select)
			canvas->selectState(node.id);

		/* print node_selected to cmd */
		static int last_node_selected = -1;
		if (canvas->getStateSelected() != last_node_selected)
			printf("active id: %d\n", canvas->getStateSelected());
		last_node_selected = canvas->getStateSelected();


		if (node_moving_active && ImGui::IsMouseDragging(0))
			node.pos = node.pos + ImGui::GetIO().MouseDelta;
	}

	draw_list.ChannelsSetCurrent(canvas->layer(1));
	/* draw node background */
	{
		ImU32 node_bg_color = (canvas->getStateHoveredInList() == node.id || canvas->getStateHoveredInScene() == node.id
			|| (canvas->getStateHoveredInList() == -1 && canvas->getStateSelected() == node.id))
			? ImColor(75, 75, 75) : ImColor(60, 60, 60);

		draw_list.AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		draw_list.AddRect(node_rect_min, node_rect_max, ImColor(100, 100, 100), 4.0f);
	}

	ImGui::PopID();
}

void StateMachinePainter::drawGrid(ImDrawList& draw_list) const
{
	ImU32 GRID_COLOR = ImColor(200, 200, 200, 40);
	float GRID_SZ = 64.0f;
	ImVec2 win_pos = ImGui::GetCursorScreenPos();
	ImVec2 canvas_sz = ImGui::GetWindowSize();

	for (float x = fmodf(-scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
		draw_list.AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
	for (float y = fmodf(-scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
		draw_list.AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
}

void StateMachinePainter::drawLinks(ImDrawList& draw_list) const {
	draw_list.ChannelsSetCurrent(0); /* background */
	float lineThickness = 3.0f;
	currentLayer->setLineThickness(lineThickness);

	for (std::pair<TransitionID, Transition> pair : currentLayer->transitions)
	{
		auto _trans = pair.second;
		auto from_pos = canvas_origin + currentLayer->states[_trans.fromID].center();
		auto to_pos = canvas_origin + currentLayer->states[_trans.toID].center();
		draw_list.AddLine(from_pos, to_pos, ImColor(200, 200, 100), lineThickness);
		if (!canvas->hasDrawingLine && ImGui::IsMouseClicked(0) && currentLayer->onTransitionLine(from_pos, to_pos, ImGui::GetMousePos()) && canvas->getStateHoveredInScene() < 0) {
			canvas->selectTrans(pair.first);
			printf("selected trans id %d\n", pair.first);

			/* todo: this line doesn't work right now */
			//canvas->drawTriangleOnLine(draw_list, from_pos, to_pos, ImColor(255, 255, 255));
		}
	}
}

void StateMachinePainter::drawUnfinishedLine(ImDrawList& draw_list) const {
	draw_list.ChannelsSetCurrent(canvas->layer(2)); /* mid layer */

	/* still drawing */
	if (canvas->hasDrawingLine)
	{
		canvas->drawing_line_end = ImGui::GetMousePos();
		draw_list.AddLine(canvas->drawing_line_start, canvas->drawing_line_end, canvas->darwing_line_color);
	}

	/* finish drawing */
	if (canvas->hasDrawingLine && ImGui::IsMouseClicked(0))
	{
		StateID transition_end_id = canvas->getStateHoveredInScene();
		canvas->hasDrawingLine = false;
		/* if you didn't click on any state */
		if (canvas->getStateHoveredInScene() != -1) {
			currentLayer->addTransitionAndUpdateState(canvas->transition_start_id, transition_end_id);
		}
	}
}

void StateMachinePainter::drawContextMenu() const {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("context_menu"))
	{
		State* node = canvas->getStateSelected() != -1 ? &currentLayer->states[canvas->getStateSelected()] : NULL;
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - canvas_origin;
		if (node)
		{
			ImGui::Text("Node '%s'", node->name);
			ImGui::Separator();
			if (ImGui::MenuItem("New Transition", nullptr, false, true))
			{
				canvas->hasDrawingLine = true;
				canvas->transition_start_id = node->id;
				canvas->drawing_line_start = node->center() + canvas_origin;
				canvas->darwing_line_color = ImColor(100, 255, 255);
				printf("start form node id %d with node name %s\n", node->id, node->name);
			};
			if (ImGui::MenuItem("Rename..", nullptr, false, false)) {}
			if (ImGui::MenuItem("Delete", nullptr, false, false)) {}
			if (ImGui::MenuItem("Copy", nullptr, false, false)) {}
		}
		else
		{
			if (ImGui::MenuItem("Add")) { currentLayer->addState("New node", scene_pos, std::vector<StateID>{}); }
			if (ImGui::MenuItem("Paste", nullptr, false, false)) {}
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}
