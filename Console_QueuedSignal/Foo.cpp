#include "Foo.h"

Foo::Foo() {

}

void Foo::DoWork() {
	emit privateSignals->Error(1);
}