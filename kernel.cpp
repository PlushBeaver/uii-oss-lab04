//
// Реализация "ядра ОС".
//
#include "api.h"
#include "init.h"
#include "kernel.h"

#include <algorithm>  // std::find_if()
#include <cassert>    // assert()
#include <iostream>   // std::cerr
#include <vector>     // std::vector<T>

#include <windows.h>

using namespace std;

namespace OS
{

/// Все задачи в "ядре".
std::vector<Task*> tasks;


Task* round_robin_scheduler()
{
	// TODO: алгоритм циклического планирования.
	return nullptr;
}


Task* guaranteed_scheduler()
{
	// TODO: алгоритм гарантированного планирования.
	return nullptr;
}


Task* schedule() {
	// TODO:
	// Любой планировщик должен:
	//    1. Выбирать очередную готовую задачу для выполнения.
	//    2. Удалять завершенные задачи через remove_task().
	//    3. Вернуть nullptr, если никакую задачу нельзя запустить
	//       (задач не осталось или они все заблокированы).
	// Планировщик может хранить нужные ему данные между вызовами
	// в глобальных или статических переменных.

	return round_robin_scheduler();
//	return guaranteed_scheduler();
}


namespace API
{

void spawn(EntryPoint entry_point, void* parameter /*= nullptr*/)
{
	// Значение next_task_id сохраняется между вызовами функции.
	static TaskID next_task_id = 0;

	auto task = new Task;
	task->id = ++next_task_id;
	task->context = CreateFiber(16*1024, entry_point, parameter);
	task->state = Ready;
	tasks.push_back(task);
}

} // namespace API


// --------------------------------------------------------------------------
// Функции ниже имитируют работу механизмов ОС, их не требуется изменять.
// --------------------------------------------------------------------------

static Context kernel_context;


// "Ядро" операционной системы:
void run_kernel()
{
	// 1. Сохраняет свой контекст исполнения.
	kernel_context = ConvertThreadToFiber(NULL);
	// 2. Запускает процесс init, который запустит остальные.
	API::spawn(init_process, nullptr);
	// 3. Планирует выполнение очередной задачи и переключается на неё.
	while (Task* task = schedule()) {
		assert(("will only switch to task in ready state", task->state == Ready));
		SwitchToFiber(task->context);
	}
}


Context get_kernel_context()
{
	return kernel_context;
}


Task* get_current_task() {
	LPVOID context = GetCurrentFiber();
	for (Task* task : tasks)
		if (task->context == context)
			return task;
	cerr << __PRETTY_FUNCTION__ << ": " << "no task owns current context!" << '\n';
	return nullptr;
}


void remove_task(Task* task) {
	auto iterator = std::find_if(
			tasks.begin(), tasks.end(),
			[task](auto item) { return task->id == item->id; });
	if (iterator != tasks.end())
		tasks.erase(iterator);
	else
		cerr << __PRETTY_FUNCTION__ << ": " << "unable to remove task " << task->id << " not managed by OS!" << '\n';
}


void switch_to(Context context)
{
	SwitchToFiber(context);
}

} // namespace OS
