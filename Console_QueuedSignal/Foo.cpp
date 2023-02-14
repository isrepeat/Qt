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
	//QtConnection::Connect(this, &Temp::Error, this, [](int value) {
	//	return 1488;
	//	});

	//QtConnection::Connect(this, &Temp::Hello, &second, &Second::MySlot);
	QtConnection::Connect(this, &Temp::Error, &second, &Second::MySlot2);

}

void Temp::EmitSignal() {
	//emit classSignals->Hello(17);
	int start = 0;
	//auto res = emit Hello(17);


	auto th = std::thread([this] {
		Sleep(500);
		auto res = emit Error(17);
		Sleep(500);
		});

	//th.join();
	th.detach();

	int xxx = 9;
}


int Second::MySlot(int value) {
	return 3333;
}

int Second::MySlot2(int value) {
	return 911;
}