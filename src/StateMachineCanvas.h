#pragma once
#include <imgui.h>
#include "Transition.h"
inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs);

class StateMachineCanvas
{
public:
	ImVec2 windowSize = ImVec2(700, 700);

	/* user operation data */
private:
	StateID state_hovered_in_list = -1;
	StateID state_hovered_in_scene = -1;
	StateID state_widget_hovered = -1;

	StateID state_selected = -1;
	TransitionID trans_selected = -1;
	bool open_context_menu = false;

public:

	ImVec2 scrolling = ImVec2(0.0f, 0.0f);	/* canvas_pos + scrolling == window_pos */
	ImVec2 canvas_origin = ImVec2(0.0f, 0.0f);
	bool show_grid = true;

	bool node_debug = true;

	const int NUM_LAYERS = 4;

	bool hasDrawingLine = false;
	StateID transition_start_id = -1;
	ImVec2 drawing_line_start, drawing_line_end;
	ImColor darwing_line_color = ImColor(200, 50, 50);


	StateMachineCanvas(){}

	StateMachineCanvas operator=(const StateMachineCanvas in) {
		return *this;
	}

	/* get the idx layer, 0 is the front, -1 is the last layer*/
	int layer(const int& idx){
		if (idx == -1) return 0;
		int _res = NUM_LAYERS - 1 - idx;
		return _res >= 0 ? _res : NUM_LAYERS - 1;
	}

	void updateFrame(){
		open_context_menu = false;
		state_hovered_in_list = state_hovered_in_scene = state_widget_hovered  = -1;
	}

	void udpateCanvasOrigin()
	{
		canvas_origin = ImGui::GetCursorScreenPos() - scrolling;
	}

	void selectState(StateID id){
		state_selected = id;
		trans_selected = -1;
	}

	void hoverStateList(StateID id){state_hovered_in_list = id;}

	void hoverStateScene(StateID id){state_hovered_in_scene = id;}

	void selectTrans(TransitionID id){
		trans_selected = id;
		state_selected = -1;
	}

	void cancelSelectTrans(){trans_selected = -1;}


	StateID getStateHoveredInList() const
	{
		return state_hovered_in_list;
	}

	StateID getStateHoveredInScene() const
	{
		return state_hovered_in_scene;
	}

	StateID getStateSelected() const
	{
		return state_selected;
	}

	TransitionID getTransSelected() const
	{
		return trans_selected;
	}

	/* if right clicked set it open */
	void setContextMenu()
	{
		if (ImGui::IsMouseClicked(1))
			this->open_context_menu = true;
	}

	bool isContextMenuOpen() const
	{
		return open_context_menu;
	}


	StateID getStateWidgetHovered() const
	{
		return state_widget_hovered;
	}

	void setStateWidgetHovered(StateID state_widget_hovered)
	{
		this->state_widget_hovered = state_widget_hovered;
	}

	void checkWindowRightClick()
	{
		if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))
		{
			state_selected = state_hovered_in_list = trans_selected = -1;
			open_context_menu = true;
		}
	}
};
