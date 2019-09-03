#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
using namespace std;
#define KW_COUNT 6
#define SC_COUNT 9
#define MAX_SYMBOLS 100
#define MAX_TOK_SIZE 256
class LexicalParser {
private:
	static const string keywords[];
	static const char specialCharacters[];
	enum tokenClass { KEYWORD, IDENTIFIER, INTEGER, REAL, SPECIAL };
	struct symbol {
		string token;
		int count;
		tokenClass tc;
	};
	symbol symbolTable[MAX_SYMBOLS];
	int symbolCount;
	char nextToken;
	string error;
public:
	LexicalParser() {
		symbolCount = 0;
		error = "";
		nextToken = 0;
	}
	bool parse(char* fn) {
		ifstream fin;
		string s;
		fin.open(fn);
		if (fin.bad()) {
			error = "Cannot open the file";
			fin.close();
			return false;
		}
		nextToken = fin.get();
		while (!fin.eof()) {
			readToken(fin, s);
			if (isKeyword(s))
				addSymbol(s, KEYWORD);
			else if (isInteger(s)) {
				if (nextToken == '.') {
					string s2;
					nextToken = fin.get();
					readToken(fin, s2);
					s = s + "." + s2;
					addSymbol(s, REAL);
				}
				else
					addSymbol(s, INTEGER);
			}
			else if (isSpecialChar(s))
				addSymbol(s, SPECIAL);
			else if (isIdentifier(s))
				addSymbol(s, IDENTIFIER);
			else {
				error = "Error occurred. Unknown token \"" + s + "\"";
				return false;
			}
		}
		fin.close();
		return true;
	}
	string getError(void) {
		return error;
	}
	void readToken(ifstream& fin, string& s) {
		char scanstring[] = "\n\t ()[]+-=,;.\0";
		char spacestring[] = "\n\t ";
		char specialstring[] = "()[]+-=,;";
		char token[MAX_TOK_SIZE];
		int i = 0;
		if (strchr(specialstring, nextToken)) {
			token[i++] = nextToken;
			nextToken = fin.get();
			while (strchr(spacestring, nextToken) && fin.good()) {
				nextToken = fin.get();
			}
			token[i] = '\0';
			s = string(token);
			return;
		}
		while (fin.good()) {
			if (strchr(scanstring, nextToken)) {
				while (strchr(spacestring, nextToken) && fin.good()) {
					nextToken = fin.get();
				}
				token[i] = '\0';
				s = string(token);
				return;
			}
			else
				token[i++] = nextToken;
			nextToken = fin.get();
		}
	}
	bool isKeyword(string& s) {
		for (int i = 0; i < KW_COUNT; i++)
			if (!s.compare(keywords[i]))
				return true;
		return false;
	}
	bool isSpecialChar(string& s) {
		if (s.length() != 1)
			return false;
		char sc = s.at(0);
		for (int i = 0; i < SC_COUNT; i++)
			if (sc == specialCharacters[i])
				return true;
		return false;
	}
	bool isIdentifier(string& s) {
		for (int i = 0; i < s.length(); i++)
			if (!((s.at(i) >= 'a' && s.at(i) <= 'z') ||
				(s.at(i) >= 'A' && s.at(i) <= 'Z')))
				return false;
		return true;
	}
	bool isInteger(string& s) {
		for (int i = 0; i < s.length(); i++)
			if (s.at(i) < '0' || s.at(i) > '9')
				return false;
		return true;
	}
	void addSymbol(string& tok, tokenClass tc) {
		for (int i = 0; i < symbolCount; i++)
			if (!tok.compare(symbolTable[i].token)) {
				symbolTable[i].count++;
				return;
			}
		symbolTable[symbolCount].token = tok;
		symbolTable[symbolCount].count = 1;
		symbolTable[symbolCount++].tc = tc;
		return;
	}
	void printTokenList() {
		cout << "Symbol \t Count \t TokenType\n\n";
		for (int i = 0; i < symbolCount; i++) {
			cout << symbolTable[i].token << "\t" << symbolTable[i].count << "\t";
			if (symbolTable[i].tc == INTEGER)
				cout << "INTEGER" << endl;
			else if (symbolTable[i].tc == REAL)
				cout << "REAL" << endl;
			else if (symbolTable[i].tc == IDENTIFIER)
				cout << "IDENTIFIER" << endl;
			else if (symbolTable[i].tc == KEYWORD)
				cout << "KEYWORD" << endl;
			else
				cout << "SPECIAL" << endl;
		}
	}
};
const string LexicalParser::keywords[] = { "if", "then", "else", "begin", "end","return" };
const char LexicalParser::specialCharacters[] = { '(', ')', '[', ']', '+', '-', '=', ',', ';' };
int main(int argc, char* argv[])
{
	LexicalParser parser;
	if (!parser.parse(argv[1])) {
		cout << "Parser returned error: " << parser.getError();
		exit(0);
	}
	parser.printTokenList();
	return 0;
}