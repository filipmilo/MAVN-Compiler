#pragma once

#include "IR.h"

void makeSuccPred(Instructions& insts, Labels& labels);

void livenessAnalysis(Instructions& insts);