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
 * File:   thread_local_jni_env_ptr_test.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 7:40 PM
 */

#ifndef STATICLIB_JNI_THREAD_LOCAL_JNI_ENV_PTR_TEST_HPP
#define	STATICLIB_JNI_THREAD_LOCAL_JNI_ENV_PTR_TEST_HPP

#include "staticlib/jni/thread_local_jni_env_ptr.hpp"

#include <iostream>

#include "staticlib/config/assert.hpp"

void test_thread_local_jni_env_ptr() {
    {
        auto env = sl::jni::thread_local_jni_env_ptr();
        slassert(nullptr != env.get());
        auto env_second = sl::jni::thread_local_jni_env_ptr();
        slassert(nullptr != env_second.get());
    }
    auto env_reattach = sl::jni::thread_local_jni_env_ptr();
    slassert(nullptr != env_reattach.get());
}

#endif	/* STATICLIB_JNI_THREAD_LOCAL_JNI_ENV_PTR_TEST_HPP */

