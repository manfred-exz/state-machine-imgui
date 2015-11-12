#pragma once
#include <map>
#include "pugixml.hpp"
#include "state_machine_graph.h"
#include "State.h"
#include "Transition.h"
#include <iostream>

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
//		states[from].transitions.push_back(nextTransitionID);	/* update transition id in state */

		++nextTransitionID;

		return nextTransitionID - 1;
	}

	TransitionID addTransitionAndUpdateState(const StateID& from, const StateID& to)
	{
		Transition _trans(from, to);
		transitions.insert({ nextTransitionID, _trans });
		states[from].transitions.push_back(nextTransitionID);	/* update transition id in state */

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

		states.push_back(State(nextStateID, name, pos, trans, StateType::NORMAL, ImColor(100, 100, 100)));

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

	void XMLsave()
	{
		pugi::xml_document doc;
		doc.load_string("");

		//		pugi::xml_node root = doc.append_child("root");

		gen_xml_node(doc);

		std::cout << "Saving result: " << doc.save_file("save_file_output.xml") << std::endl;
		//		doc.save(std::cout);
	}

	void XMLparse(char* filePath)
	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(filePath);

		std::cout << "Load result: " << result.description() << std::endl;

		//		std::cout << "test: " << doc.child("sm_1").child("states").name();

		auto sMachine_xml_node = doc.child("sm_1");

		parse_xml_node(sMachine_xml_node);

		//		for (auto _element : doc)
		//		{
		//			std::cout << _element.name() << std::endl;
		//			for (auto _sub_element : _element)
		//			{
		//				std::cout << "    " << _sub_element.name() << std::endl;
		//			}
		//		}
	}

	/* if any structure failed to parse, return false.
	parse failure lead to memory data inconsistent. */
	bool parse_xml_node(pugi::xml_node &node)
	{
		if (node.empty())
			return false;

		nextStateID = node.attribute("nextStateID").as_int();
		nextTransitionID = node.attribute("nextTransitionID").as_int();
		lineThickness = node.attribute("lineThickness").as_double();

		/* states */
		{
			auto _states_node = node.child("states");
			if (_states_node.empty())
				return false;

			states.clear();
			for (auto state = _states_node.first_child(); state; state = state.next_sibling())
			{
				std::vector<TransitionID> _transitionIDs;
				auto _trans_node = state.child("transitions");
				for (auto id_attr = _trans_node.first_attribute(); id_attr; id_attr = id_attr.next_attribute())
					_transitionIDs.push_back(id_attr.as_int());

				StateID _id = state.attribute("id").as_int();
				std::string _name = state.attribute("name").as_string();
				StateType _type = (StateType)state.attribute("type").as_int();
				bool _isDefulat = state.attribute("isDefault").as_bool();

#pragma warning(disable: 4244)
				ImVec2 _pos;
				_pos.x = state.child("pos").attribute("x").as_double();
				_pos.y = state.child("pos").attribute("y").as_double();

				ImVec2 _size;
				_size.x = state.child("size").attribute("width").as_double();
				_size.y = state.child("size").attribute("height").as_double();

				ImVec4 _color;
				_color.x = state.child("color").attribute("r").as_double();
				_color.y = state.child("color").attribute("g").as_double();
				_color.z = state.child("color").attribute("b").as_double();
				_color.w = state.child("color").attribute("a").as_double();
#pragma warning(default: 4244)
				states.push_back(State(_id, _name.c_str(), _pos, _size, _color, _type, _transitionIDs, _isDefulat));
			}
		}

		/* transitions */
		{
			auto _transitions_node = node.child("transitions");
			if (_transitions_node.empty())
				return false;

			transitions.clear();
			for (auto _trans_node = _transitions_node.first_child(); _trans_node; _trans_node = _trans_node.next_sibling())
			{
				TransitionID _id = _trans_node.attribute("id").as_int();
				StateID _fromID = _trans_node.attribute("fromID").as_int();
				StateID _toID = _trans_node.attribute("toID").as_int();

				bool _solo = _trans_node.attribute("solo").as_bool();
				bool _mute = _trans_node.attribute("mute").as_bool();
				bool _hasExit = _trans_node.attribute("hasExit").as_bool();

				transitions.insert({ _id, Transition(_fromID, _toID, _solo, _mute, _hasExit) });
			}
		}

		return true;
	}

	void gen_xml_node(pugi::xml_node &root)
	{
		pugi::xml_node _node = root.append_child("stateMachine");
		_node.set_name("sm_1");

		/* states */
		{
			pugi::xml_node states_node = _node.append_child("states");
			_node.append_attribute("nextStateID") = nextStateID;

			for (State _state : states)
			{
				_state.gen_xml_node(states_node);
			}
		}

		/* transitions */
		{
			pugi::xml_node transitions_node = _node.append_child("transitions");
			_node.append_attribute("nextTransitionID") = nextTransitionID;
			for (auto _pair : transitions)
			{
				TransitionID _id = _pair.first;
				Transition _trans = _pair.second;
				_trans.gen_xml_node(transitions_node, _id);
			}
		}

		/* lineThickness */
		{
			_node.append_attribute("lineThickness") = lineThickness;
		}
	}
};
