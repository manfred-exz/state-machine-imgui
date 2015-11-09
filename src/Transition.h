#pragma once
#include <pugixml.hpp>
typedef int StateID;
typedef int TransitionID;

/* In the Graph, multi-edge, self-loop are all allowed. */
class Transition
{
public:
	StateID fromID, toID;
//	Condition[] conditions;
	bool solo, mute, hasExit;

	Transition( const StateID& from, const StateID& to, bool solo = false, bool mute = false, bool has_exit = false)
		: fromID(from),
		  toID(to),
		  solo(solo),
		  mute(mute),
		  hasExit(has_exit)
	{}

	void gen_xml_node(pugi::xml_node root, TransitionID id)
	{
		auto _trans = root.append_child("transition");

		_trans.append_attribute("id") = id;

		_trans.append_attribute("fromID") = fromID;
		_trans.append_attribute("toID") = toID;

		_trans.append_attribute("solo") = solo;
		_trans.append_attribute("mute") = mute;
		_trans.append_attribute("hasExit") = hasExit;

	}
};
