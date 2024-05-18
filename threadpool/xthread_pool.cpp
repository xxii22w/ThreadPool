#include "xthread_pool.h"
#include <iostream>
using namespace std;

void XThread_Pool::Init(int num)
{
	unique_lock<mutex> lock(mux_);
	this->thread_num_ = num;
	cout << "Thread pool Init" << num << endl;
}

void XThread_Pool::Start()
{
	unique_lock<mutex> lock(mux_);
	if (thread_num_ <= 0)
	{
		cerr << "Please Init XThreadPool" << endl;
		return;
	}
	if (!threads_.empty())
	{
		cerr << "Thread pool has start!" << endl;
		return;
	}
	// 启动线程
	for (int i = 0; i < thread_num_; i++)
	{
		//auto th = new thread(&XThreadPool::Run, this);
		auto th = make_shared<thread>(&XThread_Pool::Run, this);
		threads_.push_back(th);
	}
}

void XThread_Pool::Stop()
{
	is_exit_ = true;
	cv_.notify_all();
	for (auto& th : threads_)
	{
		th->join();
	}
	unique_lock<mutex> lock(mux_);
	threads_.clear();
}


void XThread_Pool::Run()
{
	cout << "begin XThreadPool Run " << this_thread::get_id() << endl;
	while (!is_exit())
	{
		auto task = GetTask();
		if (!task)continue;
		++task_run_count_;
		try {
			auto re = task->Run();
			task->SetValue(re);
		}
		catch (...)
		{

		}
		--task_run_count_;
	}
	cout << "end XThreadPool Run " << this_thread::get_id() << endl;
}

void XThread_Pool::AddTask(std::shared_ptr<XTask> task)
{
	unique_lock<mutex> lock(mux_);
	tasks_.push_back(task);
	task->is_exit = [this] {return is_exit(); };
	lock.unlock();
	cv_.notify_one();
}



std::shared_ptr<XTask> XThread_Pool::GetTask()
{
	unique_lock<mutex> lock(mux_);
	if (tasks_.empty())
	{
		cv_.wait(lock);
	}
	if (is_exit())
		return nullptr;
	if (tasks_.empty())
		return nullptr;
	auto task = tasks_.front();
	tasks_.pop_front();
	return task;
}

