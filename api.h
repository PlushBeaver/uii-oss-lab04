//
// "API ОС" для прикладных задач.
//
#pragma once

#include "stdcall.h"

namespace OS
{
	namespace API
	{
		/// Функция, моделирующая задачу должна иметь такой вид:
		///    void STDCALL task(void* parameter);
		typedef void STDCALL (*EntryPoint)(void*);

	    /// Создает новую задачу, описанную функцией entry_point,
	    /// готовую к выполнению (но не переключается на неё).
	    void spawn(EntryPoint entry_point, void* data = nullptr);

	    /// Возвращает управляение "ядру".
        void yield();

        /// Сигнализирует "ядру" о завершении вызвавшей задачи.
        void terminate();

        /// Семафор с точки зрения задачи.
        struct Semaphore {
            virtual void up() = 0;
            virtual void down() = 0;
        };

        /// Дескриптор --- это просто число.
        typedef  unsigned long long int  Handle;

        /// Получает ссылку на семафор по числовому дескриптору (начиная с 0).
        /// Две задачи по одному дескриптору получают один и тот же семафор.
        Semaphore& get_semaphore(Handle handle);
	}
}
