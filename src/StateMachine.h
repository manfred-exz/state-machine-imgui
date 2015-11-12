#pragma once
#include <list>
#include "StateMachineLayer.h"

class StateMachine
{
public:
	std::list<StateMachineLayer> layers;
	StateMachineLayer * baseLayer = nullptr;
};