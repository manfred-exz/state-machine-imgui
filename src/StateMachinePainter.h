#pragma once
#include "state_machine_graph.h"
#include "StateMachineLayer.h"
#include "StateMachineCanvas.h"

class StateMachinePainter
{
	StateMachineLayer *currentLayer = nullptr;
	StateMachineCanvas *canvas = nullptr;

	ImVec2 windowSize = ImVec2(700, 700);
	ImVec2 scrolling = ImVec2(0.0f, 0.0f);	/* canvas_pos + scrolling == window_pos */
	ImVec2 canvas_origin = ImVec2(0.0f, 0.0f);
	bool show_grid = true;

	const ImVec2 NODE_WINDOW_PADDING = ImVec2(8.0f, 8.0f);

public:
	StateMachinePainter(StateMachineLayer* current_layer, StateMachineCanvas* canvas)
		: currentLayer(current_layer),
		  canvas(canvas)
	{}

	/* this method is called when you start drawing your canvas, it won't work in other places. */
	void udpateCanvasOrigin()
	{
		canvas_origin = ImGui::GetCursorScreenPos() - scrolling;
	}

	void switchLayer(StateMachineLayer *newLayer)
	{
		currentLayer = newLayer;
	}

	void switchCanvas(StateMachineCanvas *newCanvas)
	{
		canvas = newCanvas;
	}

	void drawNodeListBar() const;

	void drawSideBar() const;

	void drawCanvas();

	void drawStateNode(ImDrawList& draw_list, State& node) const;

	void drawGrid(ImDrawList& draw_list) const;

	void drawLinks(ImDrawList& draw_list) const;

	void drawUnfinishedLine(ImDrawList& draw_list) const;

	void drawContextMenu() const;
};