#include <imgui.h>
#include <memory>
#include <vector>
#include "state_machine_graph.h"
#include "StateMachine.h"
#include "StateMachineCanvas.h"

using std::vector;

void ShowStateMachineGraph(bool* opened);
void initExampleNodes(StateMachine &sMachine);

void ShowStateMachineGraph(bool* opened)
{
	ImGui::SetNextWindowSize(ImVec2(700, 700), ImGuiSetCond_FirstUseEver);

	/* begin a ImGui window */
	if (!ImGui::Begin("State Machine Graph", opened))
	{
		ImGui::End();
		return;
	}

	static StateMachine sMachine;
	initExampleNodes(sMachine);

	/* params for nodes */
	bool open_context_menu = false;
	StateID state_hovered_in_list = -1;
	static StateID state_hovered_in_scene = -1;
	static StateID state_selected = -1;
	static TransitionID trans_selected = -1;
	
	static StateMachineCanvas canvas;
	canvas.update();

	/* node list: left bar*/
	ImGui::BeginChild("node_list", ImVec2(100, 0));
	{
		ImGui::Text("Nodes");
		ImGui::Separator();
		for (auto node_idx = 0; node_idx < sMachine.states.size(); node_idx++)
		{
			State* node = &sMachine.states[node_idx];
			ImGui::PushID(node->id);
			/* draw and check if selected */
			if (ImGui::Selectable(node->name, node->id == canvas.state_selected)) {
				canvas.state_selected = node->id; canvas.trans_selected = -1;
			}
			/* check if Hovered. */
			if (ImGui::IsItemHovered())
			{
				canvas.state_hovered_in_list = node->id;
				canvas.open_context_menu |= ImGui::IsMouseClicked(1);
			}
			ImGui::PopID();
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();

	/* draw param panel: side bar */
	ImGui::BeginChild("param panel", ImVec2(200, 0));
	{
		ImGui::Text("Test");
		if (canvas.state_selected >= 0) {
			State _state = sMachine.states[canvas.state_selected];
		}
	}
	ImGui::EndChild();

	/* keep the next item in the same line */
	ImGui::SameLine();

	/* draw canvas */
	{
		const int NUM_LAYERS = 4;
		/* get the idx layer, 0 is the front, -1 is the last layer*/
		auto layer = [NUM_LAYERS](int idx)
		{
			if (idx == -1) return 0;
			int _res = NUM_LAYERS - 1 - idx;
			return _res >= 0 ? _res : NUM_LAYERS - 1;
		};

		static ImVec2 scrolling = ImVec2(0.0f, 0.0f);
		ImVec2 canvas_origin = ImGui::GetCursorScreenPos() - scrolling;
		static bool show_grid = true;
		ImGui::BeginGroup();

		ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
		ImGui::SameLine(ImGui::GetWindowWidth() - 100);
		ImGui::Checkbox("Show grid", &show_grid);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImColor(60, 60, 70, 200));

		ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
		ImGui::PushItemWidth(120.0f);

		/* get current window draw list, and split into 2 layers */
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->ChannelsSplit(NUM_LAYERS);

		/* draw grids */
		if (show_grid)
		{
			ImU32 GRID_COLOR = ImColor(200, 200, 200, 40);
			float GRID_SZ = 64.0f;
			ImVec2 win_pos = ImGui::GetCursorScreenPos();
			ImVec2 canvas_sz = ImGui::GetWindowSize();

			for (float x = fmodf(-scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
				draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
			for (float y = fmodf(-scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
				draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
		}

		bool node_debug = true;
		if (node_debug)
		{
			draw_list->ChannelsSetCurrent(layer(0));	/* draw debug on top */
			ImGui::Text("mouse pos: %.2f %.2f", ImGui::GetMousePos().x, ImGui::GetMousePos().y);


			if (canvas.state_selected >= 0) {
				State currState = sMachine.states[canvas.state_selected];
				ImGui::Text("State.name: %s", currState.name);
				for (TransitionID trans_id : currState.transitions)
				{
					auto _trans = sMachine.getTransition(trans_id);
					ImGui::Text("\ttransition from %d to %d", _trans.fromID, _trans.toID);
				}
			}

			if (canvas.trans_selected >= 0)
			{
				Transition currTransition = sMachine.getTransition(canvas.trans_selected);
				ImGui::Text("Transition from %d to %d", currTransition.fromID, currTransition.toID);
			}

			if (ImGui::IsAnyItemHovered())
				ImGui::Text("Hovered");
		}

		/* draw links */
		draw_list->ChannelsSetCurrent(0); /* background */
		float lineThickness = 3.0f;
		sMachine.setLineThickness(lineThickness);
		//		for (State state_from : sMachine.states)
		//		{
		//			auto pos_from = offset + state_from.center();
		//			for (TransitionID trans_id : state_from.transitions)
		//			{
		//				auto trans = sMachine.getTransition(trans_id);
		//				auto pos_to = offset + sMachine.states[trans.toID].center();
		//				draw_list->AddLine(pos_from, pos_to, ImColor(200, 200, 100), lineThickness);
		//				
		//			}
		//		}


		ImGui::Text("offset pos %f, %f", canvas_origin.x, canvas_origin.y);
		bool anyTransSelected = false;
		for (std::pair<TransitionID, Transition> pair : sMachine.transitions)
		{
			auto _trans = pair.second;
			auto from_pos = canvas_origin + sMachine.states[_trans.fromID].center();
			auto to_pos = canvas_origin + sMachine.states[_trans.toID].center();
			draw_list->AddLine(from_pos, to_pos, ImColor(200, 200, 100), lineThickness);
			if (sMachine.onTransitionLine(from_pos, to_pos, ImGui::GetMousePos()) && canvas.state_hovered_in_scene < 0) {
				canvas.trans_selected = pair.first;
				anyTransSelected = true;
				ImGui::Text("hovered trans id %d", pair.first);
			}
		}

		if(!anyTransSelected) {
			canvas.trans_selected = -1;
			ImGui::Text("hovered nothing");
		}


		/* draw nodes */
		const float NODE_SLOT_RADIUS = 4.0f;
		const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);
		for (State &node : sMachine.states)
		{
			ImGui::PushID(node.id);

			draw_list->ChannelsSetCurrent(layer(0)); /* foreground */
			bool old_any_active = ImGui::IsAnyItemActive();

			ImVec2 node_rect_min = canvas_origin + node.pos;
			ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);

			/*draw node content*/
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
				draw_list->ChannelsSetCurrent(0); // Background
				ImGui::SetCursorScreenPos(node_rect_min);
				ImGui::InvisibleButton("node", node.size);		/* invisible button to check if mouse is hovered. */

				/* hovered */
				if (ImGui::IsItemHovered()) {
					canvas.state_hovered_in_scene = node.id;
					canvas.open_context_menu |= ImGui::IsMouseClicked(1);	/* open right click on item context menu */
				}
				else
					canvas.state_hovered_in_scene = -1;

				/* select node item */
				bool node_moving_active = ImGui::IsItemActive();
				if (node_widgets_active || node_moving_active) {
					canvas.state_selected = node.id; canvas.trans_selected = -1;
				}

				/* print node_selected to cmd */
				static int last_node_selected = -1;
				if (canvas.state_selected != last_node_selected)
					printf("active id: %d\n", canvas.state_selected);
				last_node_selected = canvas.state_selected;

				if (node_moving_active && ImGui::IsMouseDragging(0))
					node.pos = node.pos + ImGui::GetIO().MouseDelta;
			}

			draw_list->ChannelsSetCurrent(layer(1));
			/* draw node background */
			{
				ImU32 node_bg_color = (canvas.state_hovered_in_list == node.id || canvas.state_hovered_in_scene == node.id 
					|| (canvas.state_hovered_in_list == -1 && canvas.state_selected == node.id))
					? ImColor(75, 75, 75) : ImColor(60, 60, 60);

				draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
				draw_list->AddRect(node_rect_min, node_rect_max, ImColor(100, 100, 100), 4.0f);
			}

			ImGui::PopID();
		}



		/* check if right click on window context menu */
		if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))
		{
			canvas.state_selected = canvas.state_hovered_in_list = canvas.state_hovered_in_scene = canvas.trans_selected = -1;
			canvas.open_context_menu = true;
		}

		/* open context menu */
		if (canvas.open_context_menu)
		{
			ImGui::OpenPopup("context_menu");
			if (canvas.state_hovered_in_list != -1) {
				canvas.state_selected = canvas.state_hovered_in_list; canvas.trans_selected = -1;
			}
			if (canvas.state_hovered_in_scene != -1) {
				canvas.state_selected = canvas.state_hovered_in_scene; canvas.trans_selected = -1;
			}
		}

		/* Draw unfinished line */
		static bool hasDrawingLine = false;
		static StateID transition_start_id;
		static ImVec2 drawing_line_start, drawing_line_end;
		static ImColor darwing_line_color(200, 50, 50);
		{
			draw_list->ChannelsSetCurrent(layer(2)); /* mid layer */

			/* still drawing */
			if (hasDrawingLine)
			{
				drawing_line_end = ImGui::GetMousePos();
				draw_list->AddLine(drawing_line_start, drawing_line_end, darwing_line_color);
			}

			/* finish drawing */
			if (hasDrawingLine && ImGui::IsMouseClicked(0))
			{
				StateID transition_end_id = canvas.state_hovered_in_scene;
				hasDrawingLine = false;
				sMachine.addTransition(transition_start_id, transition_end_id);
				//				nodes[transition_start_id].addTransition(transition_end_id);
			}
		}

		draw_list->ChannelsMerge();


		/* Draw context menu */
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::BeginPopup("context_menu"))
		{
			State* node = canvas.state_selected != -1 ? &sMachine.states[canvas.state_selected] : NULL;
			ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - canvas_origin;
			if (node)
			{
				ImGui::Text("Node '%s'", node->name);
				ImGui::Separator();
				if (ImGui::MenuItem("New Transition", NULL, false, true))
				{
					hasDrawingLine = true;
					transition_start_id = node->id;
					drawing_line_start = node->center() + canvas_origin;
					darwing_line_color = ImColor(100, 255, 255);
				};
				if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
				if (ImGui::MenuItem("Delete", NULL, false, false)) {}
				if (ImGui::MenuItem("Copy", NULL, false, false)) {}
			}
			else
			{
				//				if (ImGui::MenuItem("Add")) { nodes.push_back(State(nodes.size(), "New node", scene_pos, vector<Transition>())); }
				if (ImGui::MenuItem("Add")) { sMachine.addState("New node", scene_pos, vector<StateID>{}); }
				if (ImGui::MenuItem("Paste", NULL, false, false)) {}
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();

		// Scrolling
		if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
			scrolling = scrolling - ImGui::GetIO().MouseDelta;

		ImGui::PopItemWidth();
		ImGui::EndChild();

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);


		ImGui::EndGroup();
	}

	ImGui::End();
}

void initExampleNodes(StateMachine &sMachine)
{
	if (sMachine.states.size() != 0)
		return;
	printf("check.\n");

	sMachine.addState("MainTex", ImVec2(40, 50), vector<StateID>{1, 2});
	sMachine.addState("BumpMap", ImVec2(40, 150), vector<StateID>{});
	sMachine.addState("Combine", ImVec2(270, 80), vector<StateID>{});
}