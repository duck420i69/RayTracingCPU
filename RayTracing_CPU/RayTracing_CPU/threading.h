#pragma once

#include <thread>
#include <mutex>
#include "Camera.h"
#include <iostream>

class ThreadPool {
public:
	ThreadPool() {};
	void startWork(const Camera& cam, const Object& scene, std::vector<vec4>& buffer);

	int getWork() {
		std::unique_lock<std::mutex> lg_lock(mux_work);
		return current_work++;
	}

	bool isDone() { 
		std::unique_lock<std::mutex> lg_lock(mux_work);
		
		// Pls delete this later
		if (current_work >= total_work) {
			current_work = 0;
			++samplePerPixel;
		}
		return current_work >= total_work; 
	}

	int getSamplePerPixel() { return samplePerPixel; }

private:
	std::vector<std::thread> m_threadPool;
	std::mutex mux_work;
	std::condition_variable cv_should_stop;
	int current_work, total_work;
	int samplePerPixel = 1;
};

void threadWork(ThreadPool& threadPool, const Camera& cam, const Object& scene, std::vector<vec4>& buffer);