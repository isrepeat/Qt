#pragma once
#include <qobject.h>
#include <future>
#include <thread>


class FutureWaiterNotifier : public QObject {
	Q_OBJECT
signals:
	void Ready();
	void Timeout();
};


template <typename T>
class FutureWaiter : public FutureWaiterNotifier {
public:

	//FutureWaiter(const FutureWaiter&) {}

	FutureWaiter(std::future<T>&& future) : future{ std::move(future) } {

	}
	FutureWaiter(FutureWaiter&&) {};

	~FutureWaiter() {
		waiting = false;
		if (waiter.joinable())
			waiter.join();
	}

	bool IsWaiting() {
		return waiting;
	}

	void StartWaitinig() {
		waiting = true;
		waiter = std::thread([this] {
			if (!this->future.valid()) {
				throw std::exception("Future was invalid");
				return;
			}

			this->future.wait();
			waiting = false;
			emit Ready();
			});
	}

	void StartWaitinig(uint32_t waitTime) {
		waiting = true;
		waiter = std::thread([this] {
			if (!this->future.valid()) {
				throw std::exception("Future was invalid");
				return;
			}

			auto status = this->future.wait_for(std::chrono::duration<std::chrono::seconds>(waitTime));
			waiting = false;

			if (status == std::future_status::ready)
				emit Ready();
			else {
				emit Timeout();
			}
			});

	}


	T GetResult() {
		if (!future.valid()) {
			waiting = false;
			throw std::exception("Future is invalid");
		}


		if constexpr (std::is_same_v<T, void>) {
			future.get();
		}
		else {
			auto tmp = future.get();
			return std::move(tmp);
		}
	}

private:
	std::atomic<bool> waiting;
	std::future<T> future;
	std::thread waiter;
};

