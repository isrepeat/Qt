#include "Foo.h"
#include <thread>
#include <Windows.h>
//#include "Foo.h"
//
//Foo::Foo() {
//
//}
//
//void Foo::DoWork() {
//	emit customSignals->Error(1);
//}

Temp::Temp() {
	QtConnection::Connect(this, &Temp::ErrorPrivate, this, [](int value) {
		return value / 100.0f;
		});

	//QtConnection::Connect(this, &Temp::Error, &second, &Second::ErrorSlot);
}

void Temp::EmitSignal() {
	//emit classSignals->Hello(17);
	int start = 0;
	//auto res = emit Hello(17);


	auto th = std::thread([this] {
		Sleep(500);
		//auto res = emit ErrorPublic(911);
		auto res = emit ErrorPrivate(1488);
		Sleep(500);
		});

	//th.join();
	th.detach();

	int xxx = 9;
}


void Second::ErrorSlot(int value) {
	return;
	//return 911;
}