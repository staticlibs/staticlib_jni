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
 * File:   error_checker_test.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 7:18 PM
 */

#ifndef STATICLIB_JNI_ERROR_CHECKER_TEST_HPP
#define STATICLIB_JNI_ERROR_CHECKER_TEST_HPP

#include "staticlib/jni/error_checker.hpp"

#include <functional>
#include <iostream>

#include "staticlib/config/assert.hpp"

bool throws_exc(std::function<void()> fun) {
    try {
        fun();
    } catch (const sl::jni::jni_exception& e) {
        (void) e;
        return true;
    }
    return false;
}

void test_error_checker() {
    sl::jni::error_checker ec;
    ec = JNI_OK;
    slassert(throws_exc([&ec](){
        ec = JNI_EDETACHED;
    }));
}

#endif /* STATICLIB_JNI_ERROR_CHECKER_TEST_HPP */

