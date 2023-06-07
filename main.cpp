// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "MyTask.h"
#include "ThreadPool.h"
#include "ParentTask.hpp"

int main()
{
	ThreadPool myPool;
	myPool.Start(true);
	while (1) 
	{
		std::shared_ptr<ParentTask<std::vector<int>>> task(new MyTask);
		myPool.AddTask(move(task));
	}

	
	
}

