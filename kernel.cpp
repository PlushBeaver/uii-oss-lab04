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

// Макрос `klog' можно использовать как `cout' для печати сообщений ядра.
// Если заменить `false' на `true', будут показываться сообщения ядра.
#define klog \
    if (false) \
        std::clog << "KERNEL: " << __func__ << ": "

namespace OS
{

// Позволяет выводить состояние задачи в читаемом виде через <<.
static ostream& operator<<(ostream& output, const TaskState& value);

/// Все задачи в "ядре".
static std::vector<Task*> tasks;

// HINT:
//    Планировщик "первым пришел --- первым обслужен" (как пример).
//    Выбирает первую задачу в очереди, готовую к исполнению.
//    Удаляет завершенные задачи, игнорирует заблокированные.
Task* first_in_first_out_scheduler()
{
	auto iterator = tasks.begin();
	while (iterator != tasks.end()) {
		Task* task = *iterator;
		switch (task->state) {
		case Ready:
		    klog << "first ready task is " << task->id << '\n';
			return task;
		case Terminated:
		    klog << "task " << task->id << " has terminated, removing it\n";
			delete task;
			iterator = tasks.erase(iterator);
			break;
		case Blocked:
		    klog << "task " << task->id << " is blocked, skipping it\n";
			++iterator;
			break;
		default:
			// На случай, если будут добавлены новые состояния,
			// которые планировщик обрабатывать не способен.
			assert(!"Unsupported task state in FIFO scheduler!");
		}
	}
	return nullptr;
}


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
	// Любой планировщик должен:
	//    1. Выбирать очередную готовую задачу для выполнения.
	//    2. Удалять завершенные задачи через remove_task().
	//    3. Вернуть nullptr, если никакую задачу нельзя запустить
	//       (задач не осталось или они все заблокированы).
	// Планировщик может хранить нужные ему данные между вызовами
	// в глобальных или статических переменных.
    //
    // Раскомментируйте одну из следующих трех строк, чтобы работал один
    // из алгоритмов планирования: FIFO, циклического или приоритетного.
	return first_in_first_out_scheduler();
//	return round_robin_scheduler();
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

    if (Task* spawner = get_current_task()) {
        klog << "task "         << spawner->id << " "
             << "spawned task " << task->id    << "\n";
    }
}


void yield()
{
	klog << "task " << get_current_task()->id << " yielded.\n";

	switch_to_kernel();
}


void terminate()
{
	klog << "task " << get_current_task()->id << " is terminating.\n";

	Task* task = get_current_task();
	task->state = Terminated;
	yield();
}

} // namespace API


// --------------------------------------------------------------------------
// Функции ниже имитируют работу механизмов ОС, их не требуется изменять.
// --------------------------------------------------------------------------

static Context kernel_context = nullptr;
static Task* current_task = nullptr;

// "Ядро" операционной системы:
void run_kernel()
{
	// 1. Сохраняет свой контекст исполнения.
	kernel_context = ConvertThreadToFiber(NULL);

	// 2. Запускает процесс init, который запустит остальные.
	API::spawn(init_process, nullptr);

	// 3. Планирует выполнение очередной задачи, если они еще остались.
	while (Task* task = schedule()) {

		klog << "task " << task->id << " was selected by scheduler\n";
		if (task->state != Ready) {
            klog << "ERROR: active task is not in Ready state!\n";
            klog << "WARNING: system may now halt or abort!\n";
		}

		// 4. Переключается на выбранную планировщиком задачу.
		switch_to(task);
	}

	klog << "all tasks terminated, system will now halt\n";
}


Task* get_current_task()
{
	return current_task;
}


void switch_to_kernel()
{
	SwitchToFiber(kernel_context);
}


void switch_to(Task* task)
{
	current_task = task;
	SwitchToFiber(task->context);
}


static ostream& operator<<(ostream& output, const TaskState& value)
{
	switch (value) {
		case Ready:      output << "Ready";      break;
		case Blocked:    output << "Blocked";    break;
		case Terminated: output << "Terminated"; break;
		default:         output << "<Unknown>";  break;
	}
	return output;
}

} // namespace OS
