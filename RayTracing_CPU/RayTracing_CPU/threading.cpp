#include "threading.h"

void threadWork(ThreadPool& threadPool, const Camera& cam, const Object& scene, std::vector<vec4>& buffer) {
	while (!threadPool.isDone()) {
		int current = threadPool.getWork();
		int samples = threadPool.getSamplePerPixel();
		if (current >= buffer.size()) std::cout << current;
		if (current < 0) std::cout << current;
		buffer[current] = buffer[current] * (samples - 1) + shootRay(scene.mesh, cam, buffer, current);
		buffer[current].x /= samples;
		buffer[current].y /= samples;
		buffer[current].z /= samples;
	}
}

void ThreadPool::startWork(const Camera& cam, const Object& scene, std::vector<vec4>& buffer) {
	total_work = cam.resolution.x * cam.resolution.y;
	current_work = 0;
	while (m_threadPool.size() < std::thread::hardware_concurrency() / 8) {
		std::thread newThread(threadWork, std::ref(*this), std::ref(cam), std::ref(scene), std::ref(buffer));
		m_threadPool.push_back(std::move(newThread));
	}
}