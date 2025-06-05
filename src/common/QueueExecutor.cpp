#include "QueueExecutor.h"
void QueueExecutor::start(TaskStart fn) {
    if (running_)
        return;

    running_ = true;
    canSubmit_ = true;
    allowSameThreadExec_ = false;
    tasks_.clear();
    executor_ = std::thread{ &QueueExecutor::loop, this };
    async([fn, this]()
    {
        bool allowSameThreadExec = fn();
        {
            std::lock_guard lck(mutex_);
            allowSameThreadExec_ = allowSameThreadExec;
        }
    });
}

bool QueueExecutor::async(Task task) {
    bool notify;
    {
        std::unique_lock<std::mutex> lck(mutex_);
        if (!canSubmit_)
            return false;

        notify = !hasPendingTasks();
        tasks_.emplace_back(std::move(task));
    }

    if (notify)
        cv_.notify_one();

    return true;
}

void QueueExecutor::stop(Task task) {
    stopAsync(std::move(task));
    stopWait();
}

bool QueueExecutor::stopAsync(Task task)
{
    if (!running_)
        return false;

    bool notify;
    {
        std::unique_lock<std::mutex> lck(mutex_);
        canSubmit_ = false;
        notify = !hasPendingTasks();
        tasks_.emplace_back([this, aTask(std::move(task))]() {
            running_ = false;
            if (aTask) aTask();
        });
    }

    if (notify)
        cv_.notify_one();

    return true;
}

void QueueExecutor::stopWait()
{
    executor_.join();
}

void QueueExecutor::loop() {
    std::unique_lock<std::mutex> lck(mutex_);
    while (running_) {
        if (!hasPendingTasksForExecutor())
            cv_.wait(lck, [&] { return hasPendingTasksForExecutor(); });

        auto task = std::move(tasks_.front());
        lck.unlock();

        task();

        lck.lock();
        tasks_.pop_front();
    }
}
