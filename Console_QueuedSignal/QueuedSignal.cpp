#include "QueuedSignal.h"

//QueuedSignal::QueuedSignal()
//    : privateSignals{ new PrivateSignals }
//{
//}


//Foo::Foo()
//	: privateSignals{ new FooPrivateSignals }
//{
//}

Foo::Foo()
{
}

void Foo::DoWork() {
	//emit privateSignals->Error(1);
}


void Bar::Handler(int type) {
	int xxx = 9;
}
