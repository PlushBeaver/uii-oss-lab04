//
// Реализация "API ОС"; в конечном счете, обращается к ядру.
//
#include "kernel.h"

namespace OS
{
namespace API
{

// spawn() реализуется только в "ядре" (kernel.cpp).

void yield()
{
	Context kernel = get_kernel_context();
	switch_to(kernel);
}


void terminate()
{
	Task* task = get_current_task();
	task->state = Terminated;
	yield();
}

// get_semaphore() реализуется в semaphore.cpp.

} // namespace API
} // namespace OS
