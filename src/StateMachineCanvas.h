#pragma once
#include <imgui.h>
#include "Transition.h"

class StateMachineCanvas
{
public:
	ImVec2 windowSize = ImVec2(700, 700);

	/* user operation data */
	bool open_context_menu = false;
	StateID state_hovered_in_list = -1;
	StateID state_hovered_in_scene = -1;
	StateID state_selected = -1;
	TransitionID trans_selected = -1;

	ImVec2 scrolling = ImVec2(0.0f, 0.0f);	/* canvas_pos + scrolling == window_pos */
	ImVec2 canvas_origin = ImVec2(0.0f, 0.0f);
	bool show_grid = true;

	bool node_debug = true;

	const int NUM_LAYERS = 4;


	StateMachineCanvas()
	{
	}

	StateMachineCanvas operator=(const StateMachineCanvas in) {
		return *this;
	}

	int layer(const int& idx)
	{
		if (idx == -1) return 0;
		int _res = NUM_LAYERS - 1 - idx;
		return _res >= 0 ? _res : NUM_LAYERS - 1;
	}

	void update()
	{
		open_context_menu = false;
		state_hovered_in_list = state_hovered_in_scene = -1;
	}
};
