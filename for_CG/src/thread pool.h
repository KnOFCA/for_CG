#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>

class ThreadPool
{
public:
	ThreadPool();
	ThreadPool(int num);
	~ThreadPool();

	void CreateThread(void);

	template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args)
		-> std::future<decltype(f(args...))>;
private:
	std::vector <std::thread> workers; //thread array

	std::queue<std::function<void()>>tasks; //task queue

	std::mutex queue_mutex;
	std::condition_variable cond;
	bool stop;
};

inline ThreadPool::ThreadPool()
{
}

inline void ThreadPool::CreateThread(void)
{
	for (;;)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(this->queue_mutex);
			this->cond.wait(lock, [this] {
				return this->stop || !this->tasks.empty();//stop ����������в�Ϊ��ʱ���ѡ�
				}
			);

			if (stop && tasks.empty())
				return;
			task = std::move(tasks.front());
			tasks.pop();//�е�����bfs��˼·
		}

		task();
	}
}

ThreadPool::ThreadPool(int num) :stop(false)
{
	for (size_t i = 0; i < num; i++)
	{
		workers.emplace_back(std::bind(&ThreadPool::CreateThread, this));
	}
}

inline ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}
	cond.notify_all();

	for (auto& worker : workers)
	{
		worker.join();
	}
}


template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
->std::future<decltype(f(args...))>
{
	using ret_type = std::future< decltype(f(args...))>; 
	std::function< decltype(f(args...))()> func =
		std::bind(std::forward<F>(f), std::forward<Args>(args)...); 

	auto task = std::make_shared< std::packaged_task< decltype(f(args...))()> >(
		func
	);

	std::function<void()> warpper_func = [task]() {(*task)(); };
	ret_type res = task->get_future();
	if (stop)
		throw std::runtime_error("enqueue on stopped ThreadPool");
	std::unique_lock<std::mutex> lock(queue_mutex);
	tasks.emplace(warpper_func);
	cond.notify_one();
	return res;
}