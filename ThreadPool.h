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
	std::mutex my_mutex;//��
	std::condition_variable my_cond;//��������
	std::vector<std::shared_ptr<std::thread>> my_thread_pool;//�̹߳��˼���
	std::vector <int> thread_working_state;//�̹߳��˹���״̬��0Ϊ����״̬��1Ϊ����״̬��-1Ϊȱ��״̬
	std::queue< std::shared_ptr<ParentTask<std::vector<int>>>> task_queue;//�������
	std::shared_ptr<std::thread> manger;//������
	std::atomic<bool> isStart;//�Ƿ�������

protected:
	void manager_thread();//�����߹���
	void start_work(const int &index);//�����߳̿�ʼ���������������ǵڼ����߳�
	int Get_task_queue_size();
public:
	ThreadPool();
	void Start(bool b);
	void AddTask(std::shared_ptr<ParentTask<std::vector<int>>> task);
	~ThreadPool();
};

