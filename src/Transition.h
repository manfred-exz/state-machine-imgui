#pragma once
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
};
