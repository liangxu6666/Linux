#pragma once
#include <future>

template<typename T>
class ParentTask
{
public:
	virtual bool Run()=0;
	virtual void printfResult() = 0;
	void setInputl(T val)
	{
		this->input=val;
	}
	T getResult()
	{
		return result;
	}
private:
	T input;
	T result;
};
