#pragma once
#include <map>
#include "state_machine_graph.h"
#include "State.h"
#include "Transition.h"



/* transitions are actually directly store in StateMachine, their reference will be store in State as well. Remember to */
class StateMachine
{
public:
	std::vector<State> states;
	//	std::vector<Transition> transitions;
	std::map<TransitionID, Transition> transitions;

private:
	/* this value only increment in runtime */
	StateID nextStateID;
	TransitionID nextTransitionID;

	double lineThickness = 3.0f;

public:
	StateMachine()
	{
		nextTransitionID = 0;
		nextStateID = 0;
	}

	TransitionID addTransition(const StateID& from, const StateID& to)
	{
		Transition _trans(from, to);
		transitions.insert({ nextTransitionID, _trans });
		++nextTransitionID;

		return nextTransitionID - 1;
	}

	TransitionID addTransition(const Transition& _trans)
	{
		transitions.insert({ nextTransitionID, _trans });
		++nextTransitionID;

		return nextTransitionID - 1;
	}

	Transition& getTransition(const TransitionID& id)
	{
		return transitions.at(id);
	}

	StateID addState(const char* name, const ImVec2& pos, const std::vector<StateID>& transitionTo)
	{
		std::vector<TransitionID> trans;
		for (StateID _to : transitionTo)
		{
			TransitionID _id = addTransition(nextStateID, _to);
			trans.push_back(_id);
		}

		states.push_back(State(nextStateID, name, pos, trans,StateType::NORMAL, ImColor(100, 100, 100)));

		++nextStateID;
		return nextStateID - 1;
	}

	void setLineThickness(const double& thickness)
	{
		lineThickness = thickness;
	}

	bool onTransitionLine(StateID from, StateID to, ImVec2 mousePos) const 
	{
		auto from_pos = states[from].center();
		auto to_pos = states[to].center();

		auto A = to_pos.y - from_pos.y;
		auto B = to_pos.x - from_pos.x;
		auto C = to_pos.x * from_pos.y - from_pos.x * to_pos.y;
		double numerator = A * mousePos.x - B* mousePos.y + C;
		double denominator = A * A + B * B;
		double squre_dist = numerator / denominator;

		return squre_dist <= lineThickness * lineThickness ? true : false;
	}

	bool onTransitionLine(ImVec2 from_pos, ImVec2 to_pos, ImVec2 mousePos) const
	{
		/* test if mouse go out of rect_range(from_pos, to_pos) */
		ImVec2 _test = (mousePos - from_pos) * (mousePos - to_pos);
		if (_test.x > 0 || _test.y > 0)
			return false;

		double A = to_pos.y - from_pos.y;
		double B = -(to_pos.x - from_pos.x);
		double C = to_pos.x * from_pos.y - from_pos.x * to_pos.y;
		double numerator = A * mousePos.x + B* mousePos.y + C;
		double denominator = A * A + B * B;
		double squre_dist = fabs(numerator * numerator / denominator);

		/* use thickness/2 + 1 to better fit with visual effect */
		return (squre_dist <= (lineThickness / 2 + 1) * (lineThickness / 2 + 1)) ? true : false;
	}
};
