#pragma once
#include <imgui.h>
#include <vector>
#include "Transition.h"

inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs);
inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs);

typedef int StateID;

enum class StateType
{
	NORMAL,
	ENTRY,
	ANY,
	EXIT,
};

/* define node data structure. */
class State
{
public:
	/* id */
	StateID     id;

	/* appearance */
	ImVec2  pos, size;
	ImVec4  color;

	/* state */
	char    name[32];
	StateType type;
	std::vector<TransitionID> transitions; /* transitions from this node to other nodes */
	bool isDefault;

	State(StateID id, const char* name, const ImVec2& pos, const std::vector<TransitionID>& transitions, const StateType& type = StateType::NORMAL,
		const ImVec4& color = ImColor(0.3f, 0.3f, 0.8f))
	{
		this->id = id;
		strncpy_s(this->name, name, 31);
		this->name[31] = 0;
		this->pos = pos;
		this->transitions = transitions;

		this->type = type;
		this->color = color;

		this->isDefault = false;
	}

	char * typeStr() const
	{
		char *_str[] = { "normal", "entry", "any", "exit" };

		return _str[int(type)];
	}

	ImVec2 drawPos() const
	{ return center(); }

	/* todo: you have to make sure the field.size is updated. */
	ImVec2 center() const
	{ return pos + ImVec2(size.x / 2, size.y / 2); }

};