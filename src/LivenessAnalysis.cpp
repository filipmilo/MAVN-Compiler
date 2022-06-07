/* Autor: Filip Milošević Datum: 29.05.2021. */
#include "LivenessAnalysis.h"

void makeSuccPred(Instructions& insts, Labels& labels) 
{

	Instruction* helper1 = NULL;
	Instruction* helper2 = NULL;

	Instructions::iterator it;

	for (it = insts.begin(); it != insts.end(); it++) 
	{

		if ((*it)->getType() != I_B && (*it)->getType() != I_BLTZ) 
		{ //ako je nesto osim skokova 
			helper1 = findInst((*it)->getPos() - 1, insts);
			helper2 = findInst((*it)->getPos() + 1, insts);
		}
		else if ((*it)->getType() == I_B) 
		{ // ako je b 
			Label* lab = findLabel((*it)->getLabel(), labels);
			helper1 = findInst((*it)->getPos() - 1, insts);
			helper2 = findInst(lab->getInst(), insts);
			helper2->pushBackPred(*it);
		}
		else 
		{ // ako je bltz
			Label* lab = findLabel((*it)->getLabel(), labels);
			helper1 = findInst((*it)->getPos() - 1, insts);
			helper2 = findInst(lab->getInst(), insts);
			if (helper2 != NULL) (*it)->pushBackSucc(helper2);
			helper2->pushBackPred(*it);
			helper2 = findInst((*it)->getPos() + 1, insts);
		}

		if (helper1 != NULL && helper1->getType() != I_B) (*it)->pushBackPred(helper1);
		if (helper2 != NULL) (*it)->pushBackSucc(helper2);
	}

	it--;
	(*it)->setIn((*it)->getSrc()); // stavljamo da je in = src od poslednje instrukcije
}

void livenessAnalysis(Instructions& insts) 
{
	bool done = true;
	for (auto instruction: insts) 
	{
		//instruction->clearInOut();
		for (auto it = insts.rbegin(); it != insts.rend(); it++) 
		{
			(*it)->doLivenessAnalysis(done);
			(*it)->uniqueInOut();
		}

		//if (done) break;
	}
}