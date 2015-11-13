#pragma once
#include "state_machine_graph.h"
#include "StateMachine.h"
#include "StateMachineLayer.h"
#include "StateMachineInteraction.h"

/* since in ImGui drawing and interaction are together, this class also handle all the interaction */
class StateMachinePainter
{
	StateMachineLayer *currentLayer;
	StateMachineInteraction *interaction;

	ImVec2 windowSize;
	ImVec2 scrolling;	/* canvas_pos + scrolling == window_pos */
	ImVec2 canvas_origin;
	bool show_grid;

	double lineThickness;
	ImVec2 NODE_WINDOW_PADDING;

private:
	ImVec4 drawing_line_color;

public:
	StateMachinePainter(StateMachineLayer* current_layer, StateMachineInteraction* interaction)
		: currentLayer(current_layer),
		interaction(interaction)
	{
		windowSize = ImVec2(700, 700);
		scrolling = ImVec2(0.0f, 0.0f);	/* canvas_pos + scrolling == window_pos */
		canvas_origin = ImVec2(0.0f, 0.0f);
		show_grid = true;

		drawing_line_color = ImColor(100, 233, 233);
		lineThickness = 3.0f;
		NODE_WINDOW_PADDING = ImVec2(8.0f, 8.0f);
	}

	ImColor getDrawLineColor() const {
		return ImColor(drawing_line_color);
	}

	/* this method is called when you start drawing your canvas, it won't work in other places. */
	void udpateCanvasOrigin()
	{
		canvas_origin = ImGui::GetCursorScreenPos() - scrolling;
	}

	void setLineThickness(const double thickness) {
		lineThickness = thickness;
	}



	void switchLayer(StateMachineLayer *newLayer)
	{
		currentLayer = newLayer;
	}

	void switchCanvas(StateMachineInteraction *newCanvas)
	{
		interaction = newCanvas;
	}

	void drawNodeListBar() const;

	void drawSideBar() const;

	void drawCanvas();

	void drawStateNode(ImDrawList& draw_list, State& node) const;

	void drawGrid(ImDrawList& draw_list) const;

	void drawLinks(ImDrawList& draw_list) const;

	void drawUnfinishedLine(ImDrawList& draw_list) const;

	void drawContextMenu() const;

	void drawLayerPanel(StateMachine& sMachine);

	bool onTransitionLine(StateID from, StateID to, ImVec2 mousePos) const;
	bool onTransitionLine(ImVec2 from_pos, ImVec2 to_pos, ImVec2 mousePos) const;
	static bool onTransitionTriangle(ImVec2 anchor_pos, ImVec2 mousePos);
	static bool isPointInTriangle(ImVec2 pt, ImVec2 t1, ImVec2 t2, ImVec2 t3);
	static void drawTriangleOnLine(ImDrawList& draw_list, ImVec2 from_pos, ImVec2 to_pos, ImColor color, const double arrow_width = 6);
	static void darwSingleTriangle(ImDrawList& draw_list, const ImVec2 anchor_pos, const ImColor color);
};
