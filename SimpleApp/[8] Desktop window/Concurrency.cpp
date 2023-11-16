#include "Concurrency.h"
#include "Helpers.h"


namespace H {
    //TaskChain::~TaskChain() {
    //    LOG_DEBUG("~TaskChain() ...");
    //    CancelAndWait();
    //}

    //bool TaskChain::Reset() {
    //    std::lock_guard lk{ mx };
    //    if (!canReset) {
    //        LOG_WARNING_D("can't reset task chain");
    //        return false;
    //    }
    //    ResetInternal();
    //    return true;
    //}

    //void TaskChain::Append(std::function<void()> taskLambda) {
    //    std::lock_guard lk{ mx };
    //    LOG_DEBUG("append new task lambda");
    //    task = task.then(taskLambda); // when completion event set all previous and next tasks execute immediatly
    //}

    //void TaskChain::StartExecuting() {
    //    std::lock_guard lk{ mx };
    //    if (executing.exchange(true))
    //        return; // return if previous value == true

    //    canReset = false;

    //    LOG_DEBUG("start executing task chain");
    //    taskCompletedEvent.set(); // at this point tasks start executing in some thread via task scheduler
    //}

    //void TaskChain::CancelAndWait() {
    //    std::lock_guard lk{ mx };
    //    if (!executing.exchange(false))
    //        return; // return if previous value == false

    //    LOG_DEBUG("cancel and wait task chain");
    //    if (task.is_done()) {
    //        LOG_WARNING_D("no wait, last task already completed");
    //    }

    //    cancelationToken.cancel();

    //    try {
    //        task.wait();
    //    }
    //    catch (const std::exception& ex) {
    //        LOG_ERROR_D("task std exception: {}", ex.what());
    //    }
    //    catch (...) {
    //        LOG_ERROR_D("task unrecognized exception !!!");
    //    }

    //    ResetInternal();
    //    canReset = true;
    //}

    //void TaskChain::ResetInternal() {
    //    LOG_DEBUG("reset tasks chain");
    //    cancelationToken = {};
    //    taskCompletedEvent = {};
    //    task = Concurrency::task<void>(taskCompletedEvent, cancelationToken.get_token());
    //}
}