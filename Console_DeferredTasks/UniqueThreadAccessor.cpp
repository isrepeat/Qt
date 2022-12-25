#include "UniqueThreadAccessor.h"

namespace H {
	void UniqueThreadAccessor::Lock() {
		if (locked) {
			std::mutex m;
			std::unique_lock<std::mutex> lk(m);
			cvLocker.wait(lk);
		}
		locked = true;
	}

	void UniqueThreadAccessor::Unlock() {
		locked = false;
		cvLocker.notify_all();
	}

	bool UniqueThreadAccessor::IsLocked() {
		return locked;
	}

	UniqueThreadAccessor::~UniqueThreadAccessor() {
		cvLocker.notify_all();
	}
}