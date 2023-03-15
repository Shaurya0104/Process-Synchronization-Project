#ifndef MY_SEMAPHORES_H
#define MY_SEMAPHORES_H

#include <mutex>
#include <condition_variable>
#include <stdexcept>

class my_sem_t {
public:
    my_sem_t(int value = 0) {
        if (value < 0)
            throw std::invalid_argument("Semaphore value must be non-negative");
        value_ = value;
    }

    my_sem_t(const my_sem_t&) = delete;
    my_sem_t& operator=(const my_sem_t&) = delete;

    ~my_sem_t() {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (value_ <= 0)
            condition_.wait(lock);
        --value_;
    }

    void post() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++value_;
        condition_.notify_one();
    }

private:
    int value_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

typedef my_sem_t sem_t;

int sem_init(sem_t *sem, int pshared, unsigned int value) {
    if (pshared != 0)
        throw std::runtime_error("Shared semaphores not supported");
    try {
        new (sem) my_sem_t(value);
        return 0;
    } catch (const std::exception&) {
        return -1;
    }
}

int sem_wait(sem_t *sem) {
    sem->wait();
    return 0;
}

int sem_post(sem_t *sem) {
    sem->post();
    return 0;
}

#endif // MY_SEMAPHORES_H
