#pragma once
#include "ParentTask.hpp"
#include <vector>
#include <stdlib.h>
#include <time.h> 
#include <algorithm>

class MyTask : virtual public ParentTask<std::vector<int>>
{
public:
	MyTask();
	virtual bool Run();
	void printfResult();

private:
	std::vector<int> input;
	std::vector<int> result;
};

