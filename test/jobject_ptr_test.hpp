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
 * File:   jobject_ptr_test.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 7:50 PM
 */

#ifndef STATICLIB_JNI_JOBJECT_PTR_TEST_HPP
#define	STATICLIB_JNI_JOBJECT_PTR_TEST_HPP

#include "staticlib/jni/jobject_ptr.hpp"

#include <iostream>

#include "staticlib/config/assert.hpp"

void test_jobject_ptr() {
    auto rt = sl::jni::jclass_ptr("java/lang/Runtime");
    auto obj = rt.call_static_object_method(rt, "getRuntime", "()Ljava/lang/Runtime;");
    jlong res = obj.call_method<jlong>("totalMemory", "()J", &JNIEnv::CallLongMethod);
    slassert(res > 0);
}

#endif	/* STATICLIB_JNI_JOBJECT_PTR_TEST_HPP */

