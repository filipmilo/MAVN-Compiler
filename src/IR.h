/* Autor: Filip Milošević Datum: 29.05.2021. */
#pragma once

#include "Types.h"
#include "LexicalAnalysis.h"


/**
 * This class represents one variable from program code.
 */

enum VariableType
{
	MEM_VAR,
	REG_VAR,
	NO_TYPE
};

class Variable
{
public:

	Variable() : m_type(NO_TYPE), m_name(""), m_position(-1), m_assignment(no_assign) {}
	Variable(std::string name, int pos) : m_type(NO_TYPE), m_name(name), m_position(pos), m_assignment(no_assign) {}
	Variable(std::string name, int pos, string type, std::string val = "") : m_name(name), m_position(pos), m_assignment(no_assign), value(val) {
		if (type == "mem") m_type = MEM_VAR;
		else if (type == "reg") m_type = REG_VAR;
		else m_type = NO_TYPE;
	}

	void ToString();
	std::string getName();
	VariableType getType();
	int getPos();
	void setAss(Regs reg);
	Regs getAss();
	std::string getValue();

private:
	VariableType m_type;
	std::string m_name;
	int m_position;
	Regs m_assignment;
	std::string value;
};


/**
 * This type represents list of variables from program code.
 */
typedef std::list<Variable*> Variables;

/**
 * This class represents one instruction in program code.
 */
class Instruction
{
public:
	Instruction() : m_position(0), m_type(I_NO_TYPE) {}
	Instruction(int pos, InstructionType type, Variables& dst, Variables& src, std::string shp, std::string l = "") :
		m_position(pos), m_type(type), m_dst(dst), m_src(src), shape(shp), label(l) {
	}

	std::string getShape();
	int getPos();
	std::string getLabel();
	InstructionType getType();
	Variables getDst();
	Variables getSrc();
	Variables getIn();
	Variables getOut();
	void setIn(Variables newIn);

	void pushBackSucc(Instruction* inst);
	void pushBackPred(Instruction* inst);
	void printSuccPred();
	void doLivenessAnalysis(bool& done);
	bool varExists(Variable* var, Variables vars);
	void clearInOut();
	void printFinal();
	void uniqueInOut();

private:
	std::string shape;
	int m_position;
	InstructionType m_type;
	std::string label;

	Variables m_dst;
	Variables m_src;

	Variables m_use; 
	Variables m_def;
	Variables m_in;
	Variables m_out;
	std::list<Instruction*> m_succ;
	std::list<Instruction*> m_pred;
};

/**
 * This type represents list of instructions from program code.
 */
typedef std::list<Instruction*> Instructions;

class Label
{
private:
	std::string name;
	int nextinst;
public:
	Label(std::string n, int next) : name(n), nextinst(next) {}

	std::string getName();
	int getInst();
	void toString();
};

typedef std::list<Label*> Labels;

struct InterferenceGraph 
{
	Variables variables;
	vector<vector<int>> matrix;
	int size;

	InterferenceGraph(Variables vars, int size) {
		variables = vars;
		this->size = size;
		
		for (int i = 0; i < size; i++) {
			vector<int> row(size);
			matrix.push_back(row);
		}
	}
};

// funkicija koja pravi instrukcije sa tri registra
void make3RegInst(Variables& dst, Variables& src, InstructionType type, int& instpos, TokenList::iterator& it, std::string shape, Variables& vars, Instructions& insts);
void MakeVarInstLabs(TokenList tokens, Variables& vars, Instructions& insts, Labels& labels, Variables& varsm);

// funkcije za nalazenje
Variable* findVar(std::string varname, Variables& vars);
Instruction* findInst(int id, Instructions& insts);
Label* findLabel(std::string name, Labels& labels);

// printovanje instrukcija i varijabli
void printInstructions(Instructions& insts);
void printVariables(Variables vars);