#include "RecourseAllocation.h"
#include <algorithm>

InterferenceGraph makeInterferenceGraph(Instructions& insts, Variables& vars) 
{
	Variables variables = vars;

	int size = variables.size();

	InterferenceGraph graph(variables, size);

	for (auto it = insts.begin(); it != insts.end(); it++) 
	{
		vector<int> interference = findInterference((*it)->getDst(), (*it)->getOut(), (*it)->getIn());
		if (!interference.empty()) 
		{
			int def = interference.back();
			for (int i = 0; i < interference.size() - 1; i++) 
			{
				int inter = interference[i];
				if (inter != -1) 
				{
					graph.matrix[def][inter] = 1;
					graph.matrix[inter][def] = 1;
				}
			}
		}

	}

	return graph;
}


vector<int> findInterference(Variables dst, Variables out, Variables in) 
{
	if (!dst.empty()) 
	{
		Variable* dit = dst.front();
		int defpos = dit->getPos();
		vector<int> positions;


		if (findVar(dit->getName(), out) != NULL) // gledamo da li postoji u out
		{
			for (auto it = out.begin(); it != out.end(); it++) 
			{
				if (dit != *it) 
				{
					positions.push_back((*it)->getPos()); // stavljamo u vektor varijable s kojima je smetnja
				}
			}
		}
		/*
		if (findVar(dit->getName(), in) != NULL) 
		{
			for (auto it = in.begin(); it != in.end(); it++) 
			{
				if (dit != *it) 
				{
					positions.push_back((*it)->getPos());
				}
			}
		}
		*/

		sort(positions.begin(), positions.end());
		positions.erase(unique(positions.begin(), positions.end()), positions.end()); // brisemo duplikate

		positions.push_back(defpos); // stavljamo na kraj vektora varijablu iz dst/def

		return positions;
	}
}

void printInterferenceGraph(InterferenceGraph graph) 
{
	cout << "***INTERFERENCE GRAPH***" << endl << endl;;

	cout << "\t" << "    ";
	for (int i = 0; i < graph.size; i++) cout << "r" << i + 1 << " ";

	cout << endl;

	for (int i = 0; i < graph.size; i++) 
	{
		cout << "\t" << "r" << i + 1 << " ";

		for (int j = 0; j < graph.size; j++) cout << "  " << graph.matrix[i][j];

		cout << endl;
	}

	cout << endl;
}


bool doRecourseAllocation(InterferenceGraph& graph) 
{
	bool regmin[5];
	Variables imaregistar;
	Variables vars = graph.variables;

	while(!vars.empty()) 
	{
		for (int i = 0; i < 5; i++) regmin[i] = false;

		Variable* var = vars.front();
		vars.pop_front();

		int i = var->getPos();
		//int reg = 1;

		for (auto jit = imaregistar.begin(); jit != imaregistar.end(); jit++) //ako ima smetnju sa obojenom varijalom onda reci da ne sme da se zauzme registar taj
			if (graph.matrix[i][(*jit)->getPos()] == 1) regmin[(*jit)->getAss()] = true;

		for (int i = 1; i < 5; i++)
		{
			if (regmin[i] == false)
			{
				var->setAss((Regs)(i));
				break;
			}
			if (i == 4) return false; // ako se desi ovo onda je spill
		}
		imaregistar.push_back(var);
	}

	return true;
}
