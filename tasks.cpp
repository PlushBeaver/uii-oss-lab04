#include <iostream>   // std::cout, etc.
#include <string>     // std::string
#include <queue>      // std::queue<T>
#include <windows.h>  // Sleep()

#include "api.h"
#include "generator.h"
#include "stdcall.h"


// "Процесс", печатающий сообщение и отдающий управление после каждого символа.
void STDCALL printer_process(void* parameter)
{
	std::string message {(const char*)parameter};

	for (size_t i = 0; i < message.size(); ++i) {
		std::cout.put(message[i]);
		OS::API::yield();
	}

	// Все процессы должны вызывать OS::API::terminate() перед окончанием
	// своей работы.  Это вызов exit() в POSIX и ExitProcess() в Windows API.
	// ОС с кооперативной многозадачностью принудительно добавляют такой
	// системный вызов к программе при её запуске.
	OS::API::terminate();
}


// Некая очередь полезных данных, доступ к которой синхронизирован семафором 0.
static std::queue<double> numbers;


// "Процесс"-производитель полезных данных.
void STDCALL producer_process(void*)
{
	OS::API::Semaphore& lock = OS::API::get_semaphore(0);
	while (true) {
		double const number = rand() / (double)RAND_MAX;
		numbers.push(number);
		std::cout.precision(2);
		std::cout << "[producer] supplied " << number << '\n';
		lock.up();
	}

	OS::API::terminate();
}


// "Процесс"-потребитель полезных данных.
void STDCALL consumer_process(void* parameter)
{
	std::string name {(const char*)parameter};

	OS::API::Semaphore& lock = OS::API::get_semaphore(0);
	while (true) {
		lock.down();
		double number = numbers.front();
		numbers.pop();
		std::cout.precision(2);
		std::cout << "[" << name << "] extracted " << number << '\n';
	}

	OS::API::terminate();
}


void STDCALL counter_process(void* parameter)
{
	std::string name {(const char*)parameter};

	// В многопоточной среде srand() и rand() могут выдавать одинаковые,
	// псевдослучайные последовательности для всех потоков. Используются
	// собственные генераторы для каждой задачи.
    RandomGenerator generator(1, 1000);

	size_t counter = 0;
	while (true) {
		size_t increment = generator.next();
		Sleep(increment);
		counter += increment;
		std::cout << "[" << name << "] counted to " << counter << '\n';
		OS::API::yield();
	}

	OS::API::terminate();
}


void STDCALL init_process(void* /*unused*/)
{
	// Для проверки циклического и лотерейного планировщика.
//	OS::API::spawn(printer_process, (void*)"First message.");
//	OS::API::spawn(printer_process, (void*)"Second message, a bit longer one.");

	// Для проверки приоритетного планировщика: единственному производителю
	// целесообразно выделять больше времени, чем многим потребителям.
//	OS::API::spawn(producer_process);
//	OS::API::spawn(consumer_process, (void*)"consumer 1");
//	OS::API::spawn(consumer_process, (void*)"consumer 2");
//	OS::API::spawn(consumer_process, (void*)"consumer 3");


	// Для проверки гарантированного планировщика: каждый процесс наращивает
	// счетчик случайное время, но планировщик должен обеспечивать приближенно
	// одинаковые значения.
//	OS::API::spawn(counter_process, (void*)"counter 1");
//	OS::API::spawn(counter_process, (void*)"counter 2");
//	OS::API::spawn(counter_process, (void*)"counter 3");

	OS::API::terminate();
}
