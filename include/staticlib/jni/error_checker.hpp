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
 * File:   error_checker.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 7:00 PM
 */

#ifndef STATICLIB_JNI_ERROR_CHECKER_HPP
#define STATICLIB_JNI_ERROR_CHECKER_HPP

#include <jni.h>

#include "staticlib/config.hpp"
#include "staticlib/support.hpp"

#include "staticlib/jni/jni_exception.hpp"

namespace staticlib {
namespace jni {

/**
 * Helper class that can be used to receive the resulting value from functions,
 * that return JNI error codes. 
 */
class error_checker {
public:

    /**
     * JNI error code value is checked on assignment.
     * `jni_exception` is thrown if input value is not equal to `JNI_OK`
     * 
     * @param err JNI error code to check
     * @throws jni_exception
     */
    void operator=(jint err) {
        if (JNI_OK != err) {
            throw jni_exception(TRACEMSG("JNI error code: [" + sl::support::to_string(err) + "]"));
        }
    }
};

} // namespace
}

#endif /* STATICLIB_JNI_ERROR_CHECKER_HPP */

