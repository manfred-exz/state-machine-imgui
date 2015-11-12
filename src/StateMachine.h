#pragma once
#include <map>
#include "StateMachineLayer.h"

class StateMachine
{
public:
	std::map<LayerID, StateMachineLayer> layers;
	LayerID baseLayerID = -1;

private:
	LayerID nextLayerID = 0;

public:
	StateMachineLayer& addLayer(const char * name, bool isBaseLayer = false) {
		layers.insert({ nextLayerID, StateMachineLayer(name) });

		if (isBaseLayer)
			baseLayerID = nextLayerID;

		++nextLayerID;
		return layers.at(nextLayerID - 1);
	}
};