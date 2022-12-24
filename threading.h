#pragma once

#include <thread>
#include <mutex>
#include "Camera.h"
#include <iostream>

class ThreadPool {
public:
	ThreadPool() {};
	void startWork(const Camera& cam, const Scene& scene, std::vector<vec4>& buffer);

	long getWork() {
		std::unique_lock<std::mutex> lg_lock(mux_work);
		if (current_work >= total_work) {
			current_work = 0;
			samplePerPixel++;
			end = std::chrono::system_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			start = end;
			std::cout << "This frame took: " << duration.count() << "ms\n";
		}
		return current_work++;
	}

	bool isDone() { 
		std::unique_lock<std::mutex> lg_lock(mux_work);
		return samplePerPixel > 1000;
	}

	int getSamplePerPixel() { return samplePerPixel; }

	void setDebugPosition(long pos) {
		debug_position = pos;

	}

	long getDebugPosition() {
		return debug_position;
	}

private:
	std::vector<std::thread> m_threadPool;
	std::mutex mux_work;
	std::condition_variable cv_should_stop;
	long debug_position;
	long current_work, total_work;
	int samplePerPixel = 0;
	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;
};

void threadWork(ThreadPool& threadPool, const Camera& cam, const Scene& object, std::vector<vec4>& buffer);