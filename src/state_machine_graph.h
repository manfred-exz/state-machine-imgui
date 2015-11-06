#pragma once
#include <vector>
#include "State.h"
inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
inline ImVec2 operator*(const ImVec2& lhs, const double& factor) { return ImVec2(lhs.x * factor, lhs.y * factor); }
inline ImVec2 operator*(const double& factor, const ImVec2& rhs) { return ImVec2(rhs.x * factor, rhs.y * factor); }

void ShowStateMachineGraph(bool* opened);