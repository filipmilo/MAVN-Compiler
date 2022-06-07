#pragma once

#include "IR.h"

InterferenceGraph makeInterferenceGraph(Instructions& insts, Variables& vars);

vector<int> findInterference(Variables dst, Variables out, Variables in);

void printInterferenceGraph(InterferenceGraph graph);

bool doRecourseAllocation(InterferenceGraph& graph);