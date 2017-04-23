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
 * File:   jclass_ptr.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 7:01 PM
 */

#ifndef STATICLIB_JNI_JCLASS_PTR_HPP
#define	STATICLIB_JNI_JCLASS_PTR_HPP

#include <memory>
#include <string>

#include <jni.h>

#include "staticlib/config.hpp"
#include "staticlib/support.hpp"

#include "staticlib/jni/jni_exception.hpp"
#include "staticlib/jni/thread_local_jni_env_ptr.hpp"

namespace staticlib {
namespace jni {

class jobject_ptr;

class jclass_ptr {
    std::string clsname;
    std::shared_ptr<_jclass> cls;

public:
    jclass_ptr(const std::string& classname) :
    clsname(classname.data(), classname.length()),
    cls([this] {
        auto env = thread_local_jni_env_ptr();
        jclass local = env->FindClass(clsname.c_str());
        if (nullptr == local) {
            throw jni_exception(TRACEMSG("Cannot load class, name: [" + clsname + "]"));
        }
        jclass global = static_cast<jclass> (env->NewGlobalRef(local));
        if (nullptr == global) {
            throw jni_exception(TRACEMSG("Cannot create global ref for class, name: [" + clsname + "]"));
        }
        env->DeleteLocalRef(local);
        return std::shared_ptr<_jclass>(global, [](jclass clazz) {
            auto env = thread_local_jni_env_ptr();
            env->DeleteGlobalRef(clazz);
        });
    }()) { }

    jclass_ptr(const jclass_ptr& other) :
    clsname(other.clsname.data(), other.clsname.length()),
    cls(other.cls) { }

    jclass_ptr& operator=(const jclass_ptr& other) {
        clsname = std::string(other.clsname.data(), other.clsname.length());
        cls = other.cls;
        return *this;
    }

    jclass operator->() {
        return cls.get();
    }

    jclass get() {
        return cls.get();
    }

    const std::string& name() const {
        return clsname;
    }

    template<typename Result, typename Func, typename... Args>
    Result call_static_method(const std::string& methodname, const std::string& signature,
            Func func, Args... args) {
        auto env = thread_local_jni_env_ptr();
        jmethodID method = env->GetStaticMethodID(cls.get(), methodname.c_str(), signature.c_str());
        if (nullptr == method) {
            throw jni_exception(TRACEMSG("Cannot find method, name: [" + methodname + "]," +
                    " signature: [" + signature + "], class: [" + clsname + "]"));
        }
        Result res = (env.get()->*func)(cls.get(), method, args...);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
            throw jni_exception(TRACEMSG("Exception raised when calling method,"
                    " name: [" + methodname + "], signature: [" + signature + "], class: [" + clsname + "]"));
        }
        return res;
    }

    template<typename... Args>
    jobject_ptr call_static_object_method(const jclass_ptr& resclass, 
            const std::string& methodname, const std::string& signature, Args... args);
};

} // namespace
}

#endif	/* STATICLIB_JNI_JCLASS_PTR_HPP */

