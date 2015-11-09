#pragma once
#include <imgui.h>
#include <vector>
#include "Transition.h"
#include <pugixml.hpp>

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


	State(StateID id,const char* name, const ImVec2& pos, const ImVec2& size, const ImVec4& color, StateType type, const std::vector<TransitionID>& transitions, bool is_default)
		: id(id),
		  pos(pos),
		  size(size),
		  color(color),
		  type(type),
		  transitions(transitions),
		  isDefault(is_default)
	{
		strncpy_s(this->name, name, 31);
		this->name[31] = 0;
	}

	char * typeStr() const
	{
		char *_str[] = { "normal", "entry", "any", "exit" };

		return _str[int(type)];
	}

	ImVec2 drawPos() const
	{
		return center();
	}

	/* todo: you have to make sure the field.size is updated. */
	ImVec2 center() const
	{
		return pos + ImVec2(size.x / 2, size.y / 2);
	}

	void gen_xml_node(pugi::xml_node &root)
	{
		auto _state_node = root.append_child("state");

		/* attributes */
		_state_node.append_attribute("id") = id;
		if (name)
			_state_node.append_attribute("name") = name;
		else
			_state_node.append_attribute("name") = "";
		_state_node.append_attribute("type") = unsigned int(type);

		/* positions */
		{
			auto _pos = _state_node.append_child("pos");
			_pos.append_attribute("x") = pos.x;
			_pos.append_attribute("y") = pos.y;
		}

		/* size */
		{
			auto _size = _state_node.append_child("size");
			_size.append_attribute("width") = size.x;
			_size.append_attribute("height") = size.y;
		}

		/* color */
		{
			auto _color = _state_node.append_child("color");
			_color.append_attribute("r") = color.x;
			_color.append_attribute("g") = color.y;
			_color.append_attribute("b") = color.z;
			_color.append_attribute("a") = color.w;
		}

		_state_node.append_attribute("isDefault") = isDefault;

		/* transitions */
		{
			auto _transitions = _state_node.append_child("transitions");
			for (TransitionID transition_id : transitions)
			{
				_transitions.append_attribute("id") = transition_id;
			}
		}
	}
	
	//	static char* toString(float data, unsigned int precision = 4)
	//	{
	//		char format[50];
	//		sprintf(format, "%%.%df", precision);
	//		char res[50];
	//		sprintf(res, format, data);
	//
	//		return res;
	//	}

	//	static char* toXMLStr(ImVec2 vec)
	//	{
	//		char buf[100];
	//		sprintf(buf, "(%.6f, %.6f)", vec.x, vec.y);
	//		return buf;
	//	}
};