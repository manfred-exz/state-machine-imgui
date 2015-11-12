#include <imgui.h>
#include <memory>
#include <vector>
#include "state_machine_graph.h"
#include "StateMachineLayer.h"
#include "StateMachineCanvas.h"
#include "StateMachinePainter.h"
#include "StateMachine.h"

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

	static StateMachine sMachine;
	
//	static StateMachineLayer baseLayer("Base Layer");
	static StateMachineLayer baseLayer = sMachine.addLayer("Base Layer", true);

	initExampleNodes(baseLayer);

	static StateMachineCanvas canvas;
	canvas.updateFrame();

	static StateMachinePainter painter(&baseLayer, &canvas);

	ImGui::BeginGroup();
	{
		if (ImGui::Button("Save XML"))
		{
			baseLayer.XMLsave();
		}

		if (ImGui::Button("Load XML"))
		{
			baseLayer.XMLparse("save_file_output.xml");
		}
	}
	ImGui::EndGroup();

	ImGui::SameLine();

	painter.drawLayerPanel();

	ImGui::SameLine();

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