/*
 * Copyright 2016, alex at staticlibs.net
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

#include "JniTest.h"

#include <iostream>

#include "error_checker_test.hpp"
#include "java_vm_ptr_test.hpp"
#include "thread_local_jni_env_ptr_test.hpp"
#include "jclass_ptr_test.hpp"
#include "jobject_ptr_test.hpp"

void JNICALL Java_JniTest_printHello(JNIEnv*, jclass) {
    try {
        test_error_checker();
        test_java_vm_ptr();
        test_thread_local_jni_env_ptr();
        test_jclass_ptr();
        test_jobject_ptr();
        std::cout << "Hello JNI success!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}
