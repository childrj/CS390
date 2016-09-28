#include "stdafx.h"
#include "Command.h"
#include <sstream>

Command::Command()
{
	list<string> commandList;
}


Command::~Command()
{
	commandList.clear();
}

void Command::addCommand(string Command) {
	commandList.push_back(Command);
	
}

string Command::removeCommand(string Command) {
	stringstream ss;
	ss << commandList.pop_back();

}

void undo() {

}

void redo() {

}