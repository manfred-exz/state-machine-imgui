#include <imgui.h>
#include <memory>
#include <vector>
#include "state_machine_graph.h"
#include "StateMachineLayer.h"
#include "StateMachineCanvas.h"
#include "StateMachinePainter.h"

using std::vector;

void ShowStateMachineGraph(bool* opened);
void initExampleNodes(StateMachineLayer &sMachine);

void ShowStateMachineGraph(bool* opened)
{
	ImGui::SetNextWindowSize(ImVec2(700, 700), ImGuiSetCond_FirstUseEver);

	/* begin a ImGui window */
	if (!ImGui::Begin("State Machine Graph", opened))
	{
		ImGui::End();
		return;
	}

	static StateMachineLayer sMachine;
	initExampleNodes(sMachine);

	static StateMachineCanvas canvas;
	canvas.updateFrame();

	static StateMachinePainter painter(&sMachine, &canvas);

	if(ImGui::Button("Save XML"))
	{
		sMachine.XMLsave();
	}

	if(ImGui::Button("Load XML"))
	{
		sMachine.XMLparse("save_file_output.xml");
	}

	painter.drawNodeListBar();

	ImGui::SameLine();

	painter.drawSideBar();

	ImGui::SameLine();

	painter.drawCanvas();

	ImGui::End();
}

void initExampleNodes(StateMachineLayer &sMachine)
{
	if (sMachine.states.size() != 0)
		return;
	printf("check.\n");

	sMachine.addState("MainTex", ImVec2(140, 50), vector<StateID>{1, 2});
	sMachine.addState("BumpMap", ImVec2(140, 150), vector<StateID>{});
	sMachine.addState("Combine", ImVec2(370, 80), vector<StateID>{});
}