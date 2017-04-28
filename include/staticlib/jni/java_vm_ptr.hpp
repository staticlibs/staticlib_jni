/*
 * Copyright 2017, alex at staticlibs.net
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   java_vm_ptr.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 7:00 PM
 */

#ifndef STATICLIB_JNI_JAVA_VM_PTR_HPP
#define	STATICLIB_JNI_JAVA_VM_PTR_HPP

#include <atomic>

#include <jni.h>

#include "staticlib/concurrent.hpp"
#include "staticlib/config.hpp"
#include "staticlib/support.hpp"

namespace staticlib {
namespace jni {

/**
 * Wraps`JavaVM` pointer and provides some machinery to keep
 * track of JVM state.
 */
class java_vm_ptr {
    sl::support::observer_ptr<JavaVM> jvm;
    std::atomic<bool> init_flag;
    sl::concurrent::countdown_latch init_latch;
    std::atomic<bool> shutdown_flag;
    sl::concurrent::condition_latch shutdown_latch;

public:
    /**
     * Constructor
     * 
     * @param vm pointer to JVM
     */
    java_vm_ptr(JavaVM* vm) :
    jvm(vm),
    init_latch(1),
    shutdown_flag(false),
    shutdown_latch([this] {
        return !this->running();
    }) { }

    /**
     * Deleted copy constructor
     */
    java_vm_ptr(const java_vm_ptr&) = delete;

    /**
     * Deleted copy assignment operator
     */
    java_vm_ptr& operator=(const java_vm_ptr&) = delete;

    /**
     * Provides access to JVM pointer
     * 
     * @returns pointer to JVM
     */
    JavaVM* operator->() {
        return jvm.get();
    }

    /**
     * Provides access to JVM pointer
     * 
     * @returns pointer to JVM
     */
    JavaVM* get() {
        return jvm.get();
    }

    /**
     * Checks whether JVM went from "running" into "shutting_down" state
     * 
     * @return true if no shutdown in progress, false otherwise
     */
    bool running() {
        return !shutdown_flag.load(std::memory_order_acquire);
    }

    /**
     * Caller thread waits until JVM is switched from "initializing" into "running" state
     */
    void await_init_complete() {
        init_latch.await();
    }

    /**
     * Notifies all init-waiting threads about completed startup
     */
    void notify_init_complete() {
        init_flag.store(true, std::memory_order_release);
        init_latch.count_down();
    }

    /**
     * Checks whether JVM went from "initializing" into "running" state
     * 
     * @return true if initialization complete, false otherwise
     */
    bool init_complete() {
        return init_flag.load(std::memory_order_acquire);
    }

    /**
     * Caller tread sleeps until specified timeout passes or 
     * JVM shuts down (which comes first)
     * 
     * @param millis
     */
    void thread_sleep_before_shutdown(std::chrono::milliseconds millis) {
        shutdown_latch.await(millis);
    }

    /**
     * Notifies all sutdown-waiting threads about begging of the shutdown
     */
    void notify_shutdown() {
        shutdown_flag.store(true, std::memory_order_release);
        // initialization may not yet happen
        init_latch.reset();
        shutdown_latch.notify_all();
    }
};

/**
 * Stores process-global pointer to JVM. It is expected, that only
 * one single JVM instance is used in current process.
 * 
 * @param jvm pointer to JVM, must NOT be specified by client code
 * @return pointer to JVM
 */
java_vm_ptr& static_java_vm(JavaVM* jvm = nullptr) {
    static java_vm_ptr vm{jvm};
    return vm;
}

} // namespace
}

#endif	/* STATICLIB_JNI_JAVA_VM_PTR_HPP */

