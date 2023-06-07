#pragma once
#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>
#include <vector>
#include <queue>
#include <atomic>
#include <functional>
#include "ParentTask.hpp"
#include "MyTask.h"

class ThreadPool
{
private:
	std::mutex my_mutex;//锁
	std::condition_variable my_cond;//条件变量
	std::vector<std::shared_ptr<std::thread>> my_thread_pool;//线程工人集合
	std::vector <int> thread_working_state;//线程工人工作状态：0为娱乐状态，1为工作状态，-1为缺少状态
	std::queue< std::shared_ptr<ParentTask<std::vector<int>>>> task_queue;//任务队列
	std::shared_ptr<std::thread> manger;//管理者
	std::atomic<bool> isStart;//是否开启工作

protected:
	void manager_thread();//管理者工作
	void start_work(const int &index);//单个线程开始工作，参数：你是第几个线程
	int Get_task_queue_size();
public:
	ThreadPool();
	void Start(bool b);
	void AddTask(std::shared_ptr<ParentTask<std::vector<int>>> task);
	~ThreadPool();
};

