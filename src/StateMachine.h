#pragma once
#include <algorithm>
#include <list>
#include "StateMachineLayer.h"


class StateMachine
{
public:
	std::list<StateMachineLayer> layers;
	StateMachineLayer* baseLayer;

private:
	LayerID nextLayerID;

public:


	StateMachine() {
		baseLayer = nullptr;
		nextLayerID = 0;
	}

	StateMachineLayer& addLayer(const char * name, bool isBaseLayer = false) {
		layers.push_back(StateMachineLayer(name, nextLayerID));

		if (isBaseLayer)
			baseLayer = &layers.back();

		++nextLayerID;
		return layers.back();
	}
};
