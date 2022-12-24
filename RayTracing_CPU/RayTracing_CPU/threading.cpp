#include "threading.h"
#include <chrono>

constexpr int DEPTH = 3;


void threadWork(ThreadPool& threadPool, const Camera& cam, const Scene& object, std::vector<vec4>& buffer) {
	while (!threadPool.isDone()) {
		long current = threadPool.getWork();
		int samples = threadPool.getSamplePerPixel();
		bool debug = current == threadPool.getDebugPosition();
		//bool debug = false;
		shootRay(object, cam, current, 1, debug);

		if (samples == 0) {
			vec4 col = shootRay(object, cam, current, 1, debug);
			for (int i = 0; i < 3; i++) buffer[current].v(i) = sqrt(col.v(i));
		}
		else {
			vec4 col = shootRay(object, cam, current, DEPTH, debug);
			//if (col.v(0) + col.v(1) + col.v(2) > 100) std::cout << "Final color: " << col.v(0) << " " << col.v(1) << " " << col.v(2) << "\n\n\n";;
			if (debug) std::cout << "Final color: " << col.v(0) << " " << col.v(1) << " " << col.v(2) << "\n\n\n";
			for (int i = 0; i < 3; i++) {
				buffer[current].v(i) = buffer[current].v(i) * buffer[current].v(i) * (samples - 1) + col.v(i);
			}
			buffer[current].v /= samples;
			for (int i = 0; i < 3; i++) {
				buffer[current].v(i) = sqrt(buffer[current].v(i));
			}
		}
	}
}

void ThreadPool::startWork(const Camera& cam, const Scene& scene, std::vector<vec4>& buffer) {
	total_work = cam.resolution.x * cam.resolution.y;
	current_work = 0;
	start = std::chrono::system_clock::now();
	while (m_threadPool.size() < std::thread::hardware_concurrency() * 2 / 3) {
		std::thread newThread(threadWork, std::ref(*this), std::ref(cam), std::ref(scene), std::ref(buffer));
		m_threadPool.push_back(std::move(newThread));
	}
}