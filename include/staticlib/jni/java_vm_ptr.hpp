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

class java_vm_ptr {
    sl::support::observer_ptr<JavaVM> jvm;
    sl::concurrent::countdown_latch init_latch;
    std::atomic<bool> shutdown_flag;
    sl::concurrent::condition_latch shutdown_latch;

public:
    java_vm_ptr(JavaVM* vm) :
    jvm(vm),
    init_latch(1),
    shutdown_flag(false),
    shutdown_latch([this] {
        return !this->running();
    }) { }

    java_vm_ptr(const java_vm_ptr&) = delete;

    java_vm_ptr& operator=(const java_vm_ptr&) = delete;

    JavaVM* operator->() {
        return jvm.get();
    }

    JavaVM* get() {
        return jvm.get();
    }

    bool running() {
        return !shutdown_flag.load(std::memory_order_acquire);
    }

    void await_init_complete() {
        init_latch.await();
    }

    void notify_init_complete() {
        init_latch.count_down();
    }

    void thread_sleep_before_shutdown(std::chrono::milliseconds millis) {
        shutdown_latch.await(millis);
    }

    void notify_shutdown() {
        shutdown_flag.store(true, std::memory_order_release);
        shutdown_latch.notify_all();
    }
};

java_vm_ptr& static_java_vm(JavaVM* jvm = nullptr) {
    static java_vm_ptr vm{jvm};
    return vm;
}

} // namespace
}

#endif	/* STATICLIB_JNI_JAVA_VM_PTR_HPP */

