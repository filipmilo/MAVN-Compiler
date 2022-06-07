/* Autor: Filip Milošević Datum: 30.05.2021. */
#include "IR.h"

// Variable functions
void Variable::ToString() 
{
	std::cout << m_name << " pos: " << m_position << endl;
}

std::string Variable::getName() 
{
	return m_name;
}

VariableType Variable::getType() 
{
	return m_type;
}

int Variable::getPos() 
{
	return m_position;
}

void Variable::setAss(Regs reg) 
{
	m_assignment = reg;
}

Regs Variable::getAss() 
{
	return m_assignment;
}

std::string Variable::getValue() 
{
	return value;
}

// Instruction functions
std::string Instruction::getShape() 
{
	return shape;
}

int Instruction::getPos() 
{
	return m_position;
}

std::string Instruction::getLabel() 
{
	return label;
}

InstructionType Instruction::getType() 
{
	return m_type;
}

void Instruction::setIn(Variables newIn) 
{
	m_in = newIn;
}

void Instruction::pushBackSucc(Instruction* inst)
{
	m_succ.push_back(inst);
}

void Instruction::pushBackPred(Instruction* inst) 
{
	m_pred.push_back(inst);
}

void Instruction::printSuccPred() 
{

	cout << "\tPRED: ";
	for (auto it = m_pred.begin(); it != m_pred.end(); it++) cout << (*it)->getPos() << " ";

	cout << endl << "\tSUCC: ";
	for (auto it = m_succ.begin(); it != m_succ.end(); it++) cout << (*it)->getPos() << " ";

	cout << endl << "\tIN: ";
	for (auto it = m_in.begin(); it != m_in.end(); it++) cout << (*it)->getName() << " ";

	cout << endl << "\tOUT: ";
	for (auto it = m_out.begin(); it != m_out.end(); it++) cout << (*it)->getName() << " ";
}

void Instruction::doLivenessAnalysis(bool& done) 
{

	Variables ini;
	Variables oui;
	//Variables copyin = m_in;
	//Variables copyout = m_out;

	for (auto sit = m_succ.begin(); sit != m_succ.end(); sit++)
		for (auto init = (*sit)->m_in.begin(); init != (*sit)->m_in.end(); init++)
			if (!varExists(*init, m_out)) oui.push_back(*init);

	//m_out = oui;
	for (auto it = oui.begin(); it != oui.end(); it++) m_out.push_back(*it);

	for (auto oit = m_out.begin(); oit != m_out.end(); oit++)
		if (!varExists(*oit, m_dst)) ini.push_back(*oit);

	for (auto sit = m_src.begin(); sit != m_src.end(); sit++)
		if (!varExists(*sit, m_in)) ini.push_back(*sit);

	//m_in = ini;
	for (auto it = ini.begin(); it != ini.end(); it++) m_in.push_back(*it);

	//if (copyin != m_in || copyout != m_out) done = false;
}

bool Instruction::varExists(Variable* var, Variables vars)
{
	for (auto it = vars.begin(); it != vars.end(); it++)
		if (*it == var) return true;

	return false;
}

void Instruction::clearInOut() 
{
	m_in.clear();
	m_out.clear();
}

Variables Instruction::getDst() 
{
	return m_dst;
}
Variables Instruction::getSrc() 
{
	return m_src;
}
Variables Instruction::getIn() 
{
	return m_in;
}
Variables Instruction::getOut()
{
	return m_out;
}

void Instruction::printFinal() 
{

	char regs[5] = {'e', '0', '1', '2', '3' };
	Variables src = m_src;
	int counter = 0;

	for (auto &ch : shape) 
	{
		Variable* var;
		Regs reg;

		if (counter > 4) 
		{ // counter ceka do 4 da bi izbegli zamenu karaktera u imenu instrukcija
			switch (ch) 
			{
			case '`':
				ch = 't';
				break;
			case 'd':
				if (!m_dst.empty()) 
				{
					var = m_dst.front();
					reg = var->getAss();
					ch = regs[(int)reg];
				}
				break;
			case 's':
				if (!src.empty()) 
				{
					var = src.front();
					src.pop_front();
					reg = var->getAss();
					ch = regs[(int)reg];
				}
				break;
			default:
				break;
			}
		}
		counter++;
	}

	cout << shape << endl;
}

void Instruction::uniqueInOut() 
{ //funckija unique
	Variables in;
	Variables out;

	for (auto it = m_in.begin(); it != m_in.end(); it++) 
	{
		if (findVar((*it)->getName(), in) == NULL) in.push_back(*it);
	}

	m_in = in;

	for (auto it = m_out.begin(); it != m_out.end(); it++) 
	{
		if (findVar((*it)->getName(), out) == NULL) out.push_back(*it);
	}

	m_out = out;
}

// Label functions
std::string Label::getName()
{
	return name;
}

int Label::getInst() 
{
	return nextinst;
}

void Label::toString() 
{
	cout << name << " next instruction: " << nextinst << endl;
}



void MakeVarInstLabs(TokenList tokens, Variables& vars, Instructions& insts, Labels& labels, Variables& varsm) 
{
	int varpos = 0;
	int instpos = 0;

	TokenList::iterator it;
	for (it = tokens.begin(); it != tokens.end(); it++) 
	{
		std::string memval;
		std::string name;

		if (it->getValue() == "_mem") 
		{ // pravimo mem_var
			it++;
			name = it->getValue();
			it++;
			memval = it->getValue();
			if (findVar(name, varsm) == NULL) varsm.push_back(new Variable(name, -1, "mem", memval));
			else throw runtime_error("VARIABLE THAT ALREADY EXISTS TRIED TO BE REDEFINED!");
			//varpos++;
		}
		else if (it->getValue() == "_reg") 
		{ // pravimo reg_var
			it++;
			if (findVar(it->getValue(), vars) == NULL) vars.push_back(new Variable(it->getValue(), varpos, "reg"));
			else throw runtime_error("VARIABLE THAT ALREADY EXISTS TRIED TO BE REDEFINED!");
			varpos++;
		}
		else if (it->getValue() == "_func") 
		{ // kada dodjemo do _func pravimo main labelu
			it++;
			labels.push_back(new Label(it->getValue(), instpos));
			it++;
			break;
		}
	}

	for (; it != tokens.end(); it++) 
	{
		Variables dst;
		Variables src;
		std::string shp;
		Variable* hlp;

		switch (it->getType()) //pravimo instrukcije
		{
		case T_ADD: // add r1, r2, r2;
			make3RegInst(dst, src, I_ADD, instpos, it, "\tadd\t\t $`d, $`s, $`s", vars, insts);
			break;
		case T_SUB:
			make3RegInst(dst, src, I_SUB, instpos, it, "\tsub\t\t $`d, $`s, $`s", vars, insts);
			break;
		case T_LA: // la rid, mid; // ubaciti error 
			it++;

			hlp = findVar(it->getValue(), vars);
			if (hlp != NULL) 
				dst.push_back(hlp);
			else 
				throw runtime_error("VARIABLE NOT DEFINED");
			it++;
			it++;

			hlp = findVar(it->getValue(), varsm);
			if(hlp == NULL) throw runtime_error("VARIABLE NOT DEFINED");
	
			shp = "\tla\t\t $`d, " + it->getValue();
			insts.push_back(new Instruction(instpos, I_LA, dst, src, shp));
			instpos++;
			break;
		case T_LW: //  lw rid, num(rid);
			it++;
			hlp = findVar(it->getValue(), vars);
			if (hlp != NULL) 
				dst.push_back(hlp);
			else 
				throw runtime_error("VARIABLE NOT DEFINED");
			it++;
			it++;
			shp = "\tlw\t\t $`d, " + it->getValue() + "($`s)";
			it++;
			it++;
			hlp = findVar(it->getValue(), vars);
			if (hlp != NULL) 
				src.push_back(hlp);
			else 
				throw runtime_error("VARIABLE NOT DEFINED");
			insts.push_back(new Instruction(instpos, I_LW, dst, src, shp));
			instpos++;
			break;
		case T_B: // b id;
			it++;
			shp = "\tb\t\t " + it->getValue();
			insts.push_back(new Instruction(instpos, I_B, dst, src, shp, it->getValue()));
			instpos++;
			break;
		case T_BLTZ: // bltz rid, id;
			it++;
			hlp = findVar(it->getValue(), vars);
			if (hlp != NULL) 
				src.push_back(hlp);
			else 
				throw runtime_error("VARIABLE NOT DEFINED");
			it++;
			it++;
			shp = "\tbltz\t\t $`s, " + it->getValue();
			insts.push_back(new Instruction(instpos, I_BLTZ, dst, src, shp, it->getValue()));
			instpos++;
			break;
		case T_AND:
			make3RegInst(dst, src, I_AND, instpos, it, "\tand\t\t $`d, $`s, $`s", vars, insts);
			break;
		case T_NOT:
			it++;
			hlp = findVar(it->getValue(), vars);
			if (hlp != NULL)
				dst.push_back(hlp);
			else
				throw runtime_error("VARIABLE NOT DEFINED");
			it++;
			it++;
			hlp = findVar(it->getValue(), vars);
			if (hlp != NULL)
				src.push_back(hlp);
			else
				throw runtime_error("VARIABLE NOT DEFINED");
			insts.push_back(new Instruction(instpos, I_NOT, dst, src, "\tnot\t\t $`d, $`s"));
			instpos++;
			break;
		case T_SRL: // srl rid, rid, num;
			it++;
			hlp = findVar(it->getValue(), vars);
			if (hlp != NULL)
				dst.push_back(hlp);
			else
				throw runtime_error("VARIABLE NOT DEFINED");
			it++;
			it++;

			hlp = findVar(it->getValue(), vars);
			if (hlp != NULL)
				src.push_back(hlp);
			else
				throw runtime_error("VARIABLE NOT DEFINED");
			it++;
			it++;

			shp = "\tsrl\t\t $`d, $`s, " + it->getValue();

			insts.push_back(new Instruction(instpos, I_SRL, dst, src, shp));
			instpos++;
			break;
		case T_SLL:
			it++;
			hlp = findVar(it->getValue(), vars);
			if (hlp != NULL)
				dst.push_back(hlp);
			else
				throw runtime_error("VARIABLE NOT DEFINED");
			it++;
			it++;

			hlp = findVar(it->getValue(), vars);
			if (hlp != NULL)
				src.push_back(hlp);
			else
				throw runtime_error("VARIABLE NOT DEFINED");
			it++;
			it++;

			shp = "\tsll\t\t $`d, $`s, " + it->getValue();

			insts.push_back(new Instruction(instpos, I_SRL, dst, src, shp));
			instpos++;
			break;
		case T_ID: //label:
			labels.push_back(new Label(it->getValue(), instpos));
		default:
			break;
		}
	}

}


void make3RegInst(Variables& dst, Variables& src, InstructionType type, int& instpos, TokenList::iterator& it, std::string shape, Variables& vars, Instructions& insts) 
{
	it++;

	Variable* hlp;

	hlp = findVar(it->getValue(), vars);
	if (hlp != NULL)
		dst.push_back(hlp);
	else
		throw runtime_error("VARIABLE NOT DEFINED");

	for (int i = 0; i < 2; i++)
	{
		it++;
		it++;

		hlp = findVar(it->getValue(), vars);
		if (hlp != NULL)
			src.push_back(hlp);
		else
			throw runtime_error("VARIABLE NOT DEFINED");
	}

	insts.push_back(new Instruction(instpos, type, dst, src, shape));
	instpos++;
}

Variable* findVar(std::string varname, Variables& vars)
{
	Variables::iterator it;
	for (it = vars.begin(); it != vars.end(); it++) 
	{
		if ((*it)->getName() == varname) return *it;
	}

	return NULL;
}

Instruction* findInst(int id, Instructions& insts) 
{
	for (auto it = insts.begin(); it != insts.end(); it++) 
	{
		if ((*it)->getPos() == id) return *it;
	}

	return NULL;
}

Label* findLabel(std::string name, Labels& labels) 
{
	for (auto it = labels.begin(); it != labels.end(); it++) 
	{
		if ((*it)->getName() == name) return *it;
	}

	return NULL;
}


void printInstructions(Instructions& insts) 
{
	for (auto it = insts.begin(); it != insts.end(); it++) 
	{
		cout << "\tINSTRUCTION " << (*it)->getPos() << endl;
		cout << (*it)->getShape() << endl;
		(*it)->printSuccPred();
		cout << endl << endl;
	}
}


void printVariables(Variables vars) 
{
	for (auto it = vars.begin(); it != vars.end(); it++) 
	{
		cout << (*it)->getPos();
	}
}