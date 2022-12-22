#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <atomic>

#include "base.h"

namespace _STD_BUILD {
	template <typename Func>
	class ThreadPool {
	public:
		ThreadPool() = default;
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		void operator=(const ThreadPool&) = delete;
		void operator=(ThreadPool&&) = delete;

		~ThreadPool() {
			m_quit = true;

			m_cv.notify_all();

			for(auto& thread : m_threads) {
				thread.join();
			}
		}

		void init(uint16_t thread_count) {
			if(m_initialized == false) {
				// If a user doesn't specify a number of threads, then try to use the highest amount possible.
				const auto supported_thread_count = std::thread::hardware_concurrency();
				if(thread_count == 0) {
					if(supported_thread_count == 0) {
						thread_count = 1;
					} else {
						thread_count = supported_thread_count;
					}
				} else {
					if(supported_thread_count != 0 && thread_count >= supported_thread_count) {
						thread_count = supported_thread_count;
					}
				}

				for(auto i = 0; i < thread_count; ++i) {
					m_threads.push_back(std::thread(&ThreadPool::dispatchThread, this));
				}

				m_initialized = true;
			}
		}

		void addTask(Func&& func) {
			if(m_initialized) {
				std::unique_lock<std::mutex> l(m_lock);
				m_tasks.push(func);
				l.unlock();
				m_cv.notify_one();
			}
		}

		void wait() {
			if(m_initialized) {
				while(true) {
					std::unique_lock<std::mutex> l(m_lock);
					if(m_waiting_tasks == 0) {
						break;
					}
				}
			}
		}

		bool tasks_complete() {
			std::unique_lock<std::mutex> l(m_lock);
			return (m_initialized && m_waiting_tasks == 0);
		}

		bool initialized() const noexcept { return m_initialized; }

	protected:
		void dispatchThread() {
			while(true) {
				Func func;
				{
					std::unique_lock<std::mutex> l(m_lock);
					if(m_waiting_tasks > 0) { m_waiting_tasks--; }
					m_cv.wait(l, [this] { return (!m_tasks.empty() || m_quit); });

					if(m_quit && m_tasks.empty()) {
						return;
					}

					func = std::move(m_tasks.front());
					m_waiting_tasks++;
					m_tasks.pop();
				}
				func();
			}
		}

	private:
		bool m_quit{ false };
		bool m_initialized{ false };
		std::mutex m_lock;
		std::condition_variable m_cv;
		std::vector<std::thread> m_threads;
		std::queue<Func> m_tasks;
		uint32_t m_waiting_tasks{ 0 };
	};
} // namespace _STD_BUILD