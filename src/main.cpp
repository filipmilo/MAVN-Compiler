/* Autor: Filip Milošević Datum: 29.05.2021. */

#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "IR.h"
#include "LivenessAnalysis.h"
#include "RecourseAllocation.h"

#include <iostream>
#include <fstream>
#include <exception>

using namespace std;

void main()
{
	try
	{
		std::string fileName = ".\\..\\examples\\simple.mavn";
		bool retVal = false;

		LexicalAnalysis lex;
		
		if (!lex.readInputFile(fileName))
			throw runtime_error("\nException! Failed to open input file!\n");
		
		lex.initialize();

		retVal = lex.Do();

		if (retVal)
		{
			cout << "Lexical analysis finished successfully!" << endl;
			lex.printTokens();
		}
		else
		{
			lex.printLexError();
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}

		SyntaxAnalysis syntax(lex);

		if (syntax.Do()) { 
			cout << "Syntax analysis finished successfully!" << endl;
		}
		else {
			cout << "Syntax analysis failed!" << endl;
		}


		Variables vars; // lista reg var
		Variables varsm; // lista mem var
		Instructions insts; // lista svih instrukcija
		Labels labels; // lista svih labela

		MakeVarInstLabs(lex.getTokenList(), vars, insts, labels, varsm); // pravimo varijable funkcije i instrukcije

		makeSuccPred(insts, labels); // pravimo graf toka programa

		cout << "*************************************" << endl;
		cout << "INSTRUCTIONS BEFORE LIVENESS ANALYSIS" << endl;
		cout << "*************************************" << endl;
		printInstructions(insts);

		livenessAnalysis(insts); // analiza zivotnog veka

		cout << "*************************************" << endl;
		cout << "INSTRUCTIONS AFTER LIVENESS ANALYSIS!" << endl;
		cout << "*************************************" << endl;
		printInstructions(insts);

		
		InterferenceGraph graph = makeInterferenceGraph(insts, vars); // pravimo graf smetnji
		cout << endl;
		printInterferenceGraph(graph);
		
		
		if (doRecourseAllocation(graph)) cout << "***RECOURSE ALLOCATION DONE***" << endl << endl; // radimo dodelu registara
		else throw runtime_error("RECOURSE ALLOCATION FAILED! RECOURCE SPILLING OCURRED!");

		for (auto it = insts.begin(); it != insts.end(); it++) (*it)->printFinal();

		//printing to file
		ofstream file("output.s");
		file << ".globl main" << endl << endl << ".data" << endl;
		for (auto it = varsm.begin(); it != varsm.end(); it++) file << (*it)->getName() << ":\t\t.word " << (*it)->getValue() << endl;

		file << endl << ".text" << endl;
		for (auto it = insts.begin(); it != insts.end(); it++) 
		{
			for (auto lit = labels.begin(); lit != labels.end(); lit++)
				if ((*lit)->getInst() == (*it)->getPos()) file << (*lit)->getName() << ":" << endl;
			file << (*it)->getShape() << endl;
		}
		
		file.close();
	}
	catch (runtime_error e)
	{
		cout << e.what() << endl;
	}
}