#include "api.h"
#include "kernel.h"

#include <list>
#include <stdexcept>
#include <vector>

namespace OS
{

/// Семафор с точки зрения "ядра".
class Semaphore : public API::Semaphore
{
public:
    Semaphore()
        : counter_{0}
    {
    }

    void up() override
    {
        // TODO: 1. Если есть потоки, ожидающие на семафоре, перевести их
        //          в состояние готовности к исполнению.
        //       2. Иначе увеличить счетчик семафора.
        //       3. Отдать управление планировщику.

        API::yield();
    }

    void down() override
    {
        // TODO: 1. Если счетчик семафора положительный, уменьшить его.
        //       2. Иначе текущая задача переходит в режим блокировки,
        //          а управление возвращается "ядру".
        API::yield();
    }

private:
    size_t counter_;              ///< Счетчик семафора.
    std::vector<Task*> blocked_;  ///< Список заблокированых семафором задач.
};


/// Все семафоры в "ядре".
std::vector<Semaphore> semaphores;

namespace API
{

Semaphore& get_semaphore(API::Handle handle) {
    // Дескриптор семафора --- это индекс в их таблице.
    if (handle < semaphores.size()) {
        return *(semaphores.begin() + handle);
    }
    else if (handle == semaphores.size()) {
        semaphores.push_back({});
        return get_semaphore(handle);
    }
    else {
        throw std::logic_error("invalid semaphore ID");
    }
}

} // namespace API

} // namespace OS
