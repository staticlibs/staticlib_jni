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
 * File:   java_vm_ptr_test.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 7:36 PM
 */

#ifndef STATICLIB_JNI_JAVA_VM_PTR_TEST_HPP
#define STATICLIB_JNI_JAVA_VM_PTR_TEST_HPP

#include "staticlib/jni/java_vm_ptr.hpp"

#include <iostream>

#include "staticlib/config/assert.hpp"

jint JNI_OnLoad(JavaVM* vm, void* /* reserved */) {
    sl::jni::static_java_vm(vm);
    return JNI_VERSION_1_6;
}

void test_java_vm_ptr() {
    auto& jvm = sl::jni::static_java_vm();
    slassert(nullptr != jvm.get());
}

#endif /* STATICLIB_JNI_JAVA_VM_PTR_TEST_HPP */

