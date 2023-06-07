#include "ThreadPool.h"

void ThreadPool::manager_thread()
{
	while (1)//�ȴ���ʼ
	{
		if (isStart == false)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));//����0.1s
		else
		{
			printf("�̳߳ع����߿�ʼ����\n");
			break;
		}
	}
	while (1) 
	{
		this->my_mutex.lock();
		int working_num = std::count(thread_working_state.begin(), thread_working_state.end(), 1);//�ڹ����ĸ���
		int gameing_num = std::count(thread_working_state.begin(), thread_working_state.end(), 0);//�����ֵĸ���
		if (gameing_num>5)//����̫��,����һ��
		{
			//��ͷ�������ҵ�һ�����˴���
			std::vector <int> ::iterator it_state = thread_working_state.begin();
			std::vector<std::shared_ptr<std::thread>>::iterator it_thread = my_thread_pool.begin();
			while (it_thread!= my_thread_pool.end()) 
			{
				if (*it_state == 0)//�ҵ�����
				{
					*it_state=-1;//������Ϣ
					this->my_mutex.unlock();
					printf("����һ��\n");
					break;
				}
				it_thread++;
				it_state++;
			}

			(*it_thread)->join();
			(*it_thread).reset();//����
			*it_thread = nullptr;
		}
		else if (gameing_num < 5)//Ԥ����Ա̫��,ȥnewһ��
		{
			if (this->Get_task_queue_size() > 10)//�������ܶ�
			{
				//��ͷ�������ҵ�-1��nullptr��newһ��Ա��
				int index = 0;
				std::vector <int> ::iterator it_state = thread_working_state.begin();
				std::vector<std::shared_ptr<std::thread>>::iterator it_thread = my_thread_pool.begin();
				while (index < 20)
				{
					if ((*it_state)==-1)//��ְλ
					{
						break;
					}
					it_thread++;
					it_state++;
					index++;
					
				}
				if (index >= 20)
				{
					printf("��Ա���������й����ˣ�\n");
					this->my_mutex.unlock();
					std::this_thread::sleep_for(std::chrono::seconds(1));//����1s
					continue;
				}
				else
				{
					*it_thread = std::make_shared<std::thread>(std::bind(&ThreadPool::start_work, this, index));
					*it_state = 0;//״̬��ʱû�л��
					printf("��ְһ��\n");
				}

				

			}
		}
		else
		{
			printf("����״̬��");
			for(int i=0;i< thread_working_state.size();++i)
				printf("%d", thread_working_state[i]);
			printf("\n");
			this->my_mutex.unlock();
			std::this_thread::sleep_for(std::chrono::seconds(1));//����1s
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
		//�ı�״̬����ȡ����ʵ��
		{
			std::unique_lock<std::mutex> guard(my_mutex);
			if (this->thread_working_state[index] == -1 || this->thread_working_state[index] == 1)
			{
				printf("���ܸɻ�\n");
				return;//�Ѿ�������
			}
			while (task_queue.empty()) {   // ��鹲����Դ�Ƿ���������
				this->my_cond.wait(guard);   // ����ȴ�״̬�����Զ��ͷ���
			}
				task = task_queue.front();
				task_queue.pop();
				this->thread_working_state[index] = 1;
			printf("%d�Ź��˽������񲢿�ʼ�ɻ�\n", index);
		}

		//���ڿ�ʼ�ɻ���
		if (task == nullptr)
		{
			printf("���˸��ջ�\n");
			continue;
		}
		else
			task->Run();

		//������
		{
			std::unique_lock<std::mutex> guard(my_mutex);
			printf("%d�Ź����������\n", index);
			//task->printfResult();
			this->thread_working_state[index] = 0;
		}

	}
}

ThreadPool::ThreadPool()
{
	for (int i = 0; i < 20; i++)//���20���߳�
	{
		this->thread_working_state.push_back(-1);//-1��ʾְλ����û�й��ˣ���nullptr
		this->my_thread_pool.push_back(nullptr);
	}
	manger = std::make_shared<std::thread>(&ThreadPool::manager_thread,this);
}

void ThreadPool::Start(bool b)
{
	
	this->isStart = b;
	if(isStart)
		printf("�̳߳ؿ���");
	else
		printf("�̳߳���ͣ");
}

void ThreadPool::AddTask(std::shared_ptr<ParentTask<std::vector<int>>> task)
{
	{
		std::unique_lock<std::mutex> guard(my_mutex);
		//if (Get_task_queue_size() >= 200)
		//	std::this_thread::sleep_for(std::chrono::seconds(1));//����1s
		task_queue.push(task);
		printf("����������һ������\n");
		//���ѹ���
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
	printf("�̳߳�����\n");
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
