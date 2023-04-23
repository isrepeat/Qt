#include "Foo.h"
#include <thread>
#include <Windows.h>

Temp::Temp() {
	this->Connect(&TempPrivateSignals::ErrorPrivate, this, [](int value) {
		return value / 100.0f;
		});

	//this->Connect(&TempPrivateSignals::ErrorPrivate, &second, &Second::ErrorSlot);
}

void Temp::EmitSignal() {
	int start = 0;

	auto th = std::thread([this] {
		Sleep(500);
		//auto res = emit ErrorPublic(911);
		auto res = emit privateSignals->ErrorPrivate(1488);
		Sleep(500);
		});

	//th.join();
	th.detach();

	int xxx = 9;
}


float Second::ErrorSlot(int value) {
	return value / 10.0f;
	//return 911;
}