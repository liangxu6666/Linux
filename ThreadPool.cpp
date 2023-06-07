#include "ThreadPool.h"

void ThreadPool::manager_thread()
{
	while (1)//等待开始
	{
		if (isStart == false)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));//摸鱼0.1s
		else
		{
			printf("线程池管理者开始工作\n");
			break;
		}
	}
	while (1) 
	{
		this->my_mutex.lock();
		int working_num = std::count(thread_working_state.begin(), thread_working_state.end(), 1);//在工作的个数
		int gameing_num = std::count(thread_working_state.begin(), thread_working_state.end(), 0);//在娱乐的个数
		if (gameing_num>5)//闲人太多,辞退一个
		{
			//从头遍历，找第一个闲人辞退
			std::vector <int> ::iterator it_state = thread_working_state.begin();
			std::vector<std::shared_ptr<std::thread>>::iterator it_thread = my_thread_pool.begin();
			while (it_thread!= my_thread_pool.end()) 
			{
				if (*it_state == 0)//找到闲人
				{
					*it_state=-1;//清理信息
					this->my_mutex.unlock();
					printf("辞退一个\n");
					break;
				}
				it_thread++;
				it_state++;
			}

			(*it_thread)->join();
			(*it_thread).reset();//辞退
			*it_thread = nullptr;
		}
		else if (gameing_num < 5)//预备人员太少,去new一个
		{
			if (this->Get_task_queue_size() > 10)//如果任务很多
			{
				//从头遍历，找到-1或nullptr，new一个员工
				int index = 0;
				std::vector <int> ::iterator it_state = thread_working_state.begin();
				std::vector<std::shared_ptr<std::thread>>::iterator it_thread = my_thread_pool.begin();
				while (index < 20)
				{
					if ((*it_state)==-1)//空职位
					{
						break;
					}
					it_thread++;
					it_state++;
					index++;
					
				}
				if (index >= 20)
				{
					printf("满员：不能再招工人了！\n");
					this->my_mutex.unlock();
					std::this_thread::sleep_for(std::chrono::seconds(1));//摸鱼1s
					continue;
				}
				else
				{
					*it_thread = std::make_shared<std::thread>(std::bind(&ThreadPool::start_work, this, index));
					*it_state = 0;//状态暂时没有活干
					printf("入职一个\n");
				}

				

			}
		}
		else
		{
			printf("工作状态：");
			for(int i=0;i< thread_working_state.size();++i)
				printf("%d", thread_working_state[i]);
			printf("\n");
			this->my_mutex.unlock();
			std::this_thread::sleep_for(std::chrono::seconds(1));//摸鱼1s
			continue;
		}
		this->my_mutex.unlock();
	}
	
}


void ThreadPool::start_work(const int &index)
{
	
	while (isStart)
	{
		std::shared_ptr<ParentTask<std::vector<int>>> task;
		//改变状态，获取任务实例
		{
			std::unique_lock<std::mutex> guard(my_mutex);
			if (this->thread_working_state[index] == -1 || this->thread_working_state[index] == 1)
			{
				printf("不能干活\n");
				return;//已经被辞了
			}
			while (task_queue.empty()) {   // 检查共享资源是否满足条件
				this->my_cond.wait(guard);   // 进入等待状态，并自动释放锁
			}
				task = task_queue.front();
				task_queue.pop();
				this->thread_working_state[index] = 1;
			printf("%d号工人接收任务并开始干活\n", index);
		}

		//终于开始干活了
		if (task == nullptr)
		{
			printf("拿了个空活\n");
			continue;
		}
		else
			task->Run();

		//输出结果
		{
			std::unique_lock<std::mutex> guard(my_mutex);
			printf("%d号工人任务结束\n", index);
			//task->printfResult();
			this->thread_working_state[index] = 0;
		}

	}
}

ThreadPool::ThreadPool()
{
	for (int i = 0; i < 20; i++)//最多20个线程
	{
		this->thread_working_state.push_back(-1);//-1表示职位空闲没有工人，即nullptr
		this->my_thread_pool.push_back(nullptr);
	}
	manger = std::make_shared<std::thread>(&ThreadPool::manager_thread,this);
}

void ThreadPool::Start(bool b)
{
	
	this->isStart = b;
	if(isStart)
		printf("线程池开启");
	else
		printf("线程池暂停");
}

void ThreadPool::AddTask(std::shared_ptr<ParentTask<std::vector<int>>> task)
{
	{
		std::unique_lock<std::mutex> guard(my_mutex);
		//if (Get_task_queue_size() >= 200)
		//	std::this_thread::sleep_for(std::chrono::seconds(1));//摸鱼1s
		task_queue.push(task);
		printf("任务队列添加一个任务\n");
		//唤醒工人
		if (!task_queue.empty())
			this->my_cond.notify_one();
	}
	
}

int ThreadPool::Get_task_queue_size()
{
	return this->task_queue.size();
}

ThreadPool::~ThreadPool()
{
	this->Start(false);
	printf("线程池销毁\n");
	while (!task_queue.empty())
	{
		task_queue.pop();
	}
	while (!thread_working_state.empty())
	{
		if (thread_working_state.back() == 0) 
		{
			thread_working_state.pop_back();
			my_thread_pool.pop_back();
		}
		
	}

}
