#include "MyTask.h"

MyTask::MyTask()
{
	srand((unsigned)time(NULL));
	for (int i = 0; i < 1000; i++)
		this->input.push_back(rand()%1000);
}

bool MyTask::Run()
{
	if (this->input.empty())
		return false;

	std::vector<int>::iterator maxIt;
	for (int i = 0; i < input.size(); ++i) {
		maxIt = std::min_element(input.begin(), input.end());
		int val = *maxIt;
		this->input.erase(maxIt);
		this->result.push_back(val);
	}
	return true;
}

void MyTask::printfResult()
{
	if (result.empty())
	{
		printf("ÔÝÎ´ÓÐ½á¹û");
		return;
	}
	std::vector<int>::iterator it;
	int i = 0;
	for (it = result.begin(); it != result.end(); it++) 
	{
		printf("%d ",*it);
		if (0 == ++i % 100)
			printf("\n");
	}
}
