#include "threading.h"

void threadWork(ThreadPool& threadPool, const Camera& cam, const Scene& object, std::vector<vec4>& buffer) {
	while (!threadPool.isDone()) {
		long current = threadPool.getWork();
		int samples = threadPool.getSamplePerPixel();
		if (current >= buffer.size()) std::cout << current;
		if (current < 0) std::cout << current;
		vec4 col = shootRay(object, cam, buffer, current);
		for (int i = 0; i < 3; i++) {
			buffer[current].v(i) = buffer[current].v(i) * buffer[current].v(i) * (samples - 1) + col.v(i);
		}
		buffer[current].v /= samples;
		for (int i = 0; i < 3; i++) {
			buffer[current].v(i) = sqrt(buffer[current].v(i));
		}
	}
}

void ThreadPool::startWork(const Camera& cam, const Scene& scene, std::vector<vec4>& buffer) {
	total_work = cam.resolution.x * cam.resolution.y;
	current_work = 0;
	while (m_threadPool.size() < std::thread::hardware_concurrency() / 2) {
		std::thread newThread(threadWork, std::ref(*this), std::ref(cam), std::ref(scene), std::ref(buffer));
		m_threadPool.push_back(std::move(newThread));
	}
}