#include <iostream>
#include <iomanip>

#include "SyntaxAnalysis.h"
#include "Types.h"

using namespace std;


SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis& lex)
	: lexicalAnalysis(lex), errorFound(false), tokenIterator(lexicalAnalysis.getTokenList().begin())
{
}


bool SyntaxAnalysis::Do()
{
	currentToken = getNextToken();
	Q();

	return !errorFound;
}


void SyntaxAnalysis::printSyntaxError(Token token)
{
	cout << "Syntax error! Token: " << token.getValue() << " unexpected" << endl;
}


void SyntaxAnalysis::eat(TokenType t)
{
	if (errorFound == false)
	{
		if (currentToken.getType() == t)
		{
			cout << currentToken.getValue() << endl;
			currentToken = getNextToken();
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
}


Token SyntaxAnalysis::getNextToken()
{
	if (tokenIterator == lexicalAnalysis.getTokenList().end())
		throw runtime_error("End of input file reached");
	return *tokenIterator++;
}


void SyntaxAnalysis::Q()
{
	S();
	eat(T_SEMI_COL);
	L();
}


void SyntaxAnalysis::S()
{	
	switch (currentToken.getType()) 
	{
		case T_MEM:
			eat(T_MEM);
			eat(T_M_ID);
			eat(T_NUM);
			break;
		case T_REG:
			eat(T_REG);
			eat(T_R_ID);
			break;
		case T_FUNC:
			eat(T_FUNC);
			eat(T_ID);
			break;
		case T_ID:
			eat(T_ID);
			eat(T_COL);
			E();
			break;
		case T_COMMENT:
			eat(T_COMMENT);
			S();
		default:
			E();
			break;
	}
}


void SyntaxAnalysis::L()
{
	if (currentToken.getType() != T_END_OF_FILE) Q();
}


void SyntaxAnalysis::E()
{
	switch (currentToken.getType()) 
	{
		case T_ADD:
			eat(T_ADD);
			eat3rid();
			break;
		case T_SUB:
			eat(T_SUB);
			eat3rid();
			break;
		case T_LA:
			eat(T_LA);
			eat(T_R_ID);
			eat(T_COMMA);
			eat(T_M_ID);
			break;
		case T_LW:
			eat(T_LW);
			eat(T_R_ID);
			eat(T_COMMA);
			eat(T_NUM);
			eat(T_L_PARENT);
			eat(T_R_ID);
			eat(T_R_PARENT);
			break;
		case T_B:
			eat(T_B);
			eat(T_ID);
			break;
		case T_BLTZ:
			eat(T_BLTZ);
			eat(T_R_ID);
			eat(T_COMMA);
			eat(T_ID);
			break;
		case T_AND:
			eat(T_AND);
			eat3rid();
			break;
		case T_NOT:
			eat(T_NOT);
			eat(T_R_ID);
			eat(T_COMMA);
			eat(T_R_ID);
			break;
		case T_SRL:
			eat(T_SRL);
			eat2ridnum();
			break;
		case T_SLL:
			eat(T_SLL);
			eat2ridnum();
			break;
		default:
			break;
	}
}

void SyntaxAnalysis::eat3rid() 
{ // funkcija koja pojede tri registar tokena
	eat(T_R_ID);
	eat(T_COMMA);
	eat(T_R_ID);
	eat(T_COMMA);
	eat(T_R_ID);
}

void SyntaxAnalysis::eat2ridnum() 
{ // jedemo 2 registra i broj
	eat(T_R_ID);
	eat(T_COMMA);
	eat(T_R_ID);
	eat(T_COMMA);
	eat(T_NUM);
}