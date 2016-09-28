#pragma once
#include <string>
#include <list>
#include <vector>

using namespace std;

class Command
{
private:
	
public:
	list<string> commandList;
	Command();
	~Command();
	void addCommand(string Command);
	string removeCommand(string Command);
	void undo();
	void redo();
};

