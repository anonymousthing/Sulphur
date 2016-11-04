#pragma once

#define SCRIPT_API __declspec(dllexport)

class Script {
public:
	//Calls 'Init' in the underlying Script.
	void initialise();

};
