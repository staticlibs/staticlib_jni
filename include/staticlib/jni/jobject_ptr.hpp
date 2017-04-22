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
 * File:   jobject_ptr.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 7:02 PM
 */

#ifndef STATICLIB_JNI_JOBJECT_PTR_HPP
#define	STATICLIB_JNI_JOBJECT_PTR_HPP

#include <memory>
#include <string>

#include <jni.h>

#include "staticlib/config.hpp"
#include "staticlib/support.hpp"

#include "staticlib/jni/jni_exception.hpp"
#include "staticlib/jni/jclass_ptr.hpp"
#include "staticlib/jni/thread_local_jni_env_ptr.hpp"

namespace staticlib {
namespace jni {

class jobject_ptr {
    jclass_ptr cls;
    std::shared_ptr<_jobject> obj;

public:
    jobject_ptr(jclass_ptr clazz, jobject local) :
    cls(clazz),
    obj([this, local] {
        auto env = thread_local_jni_env_ptr();
        // local references will die on jni detach
        jobject global = static_cast<jobject> (env->NewGlobalRef(local));
        if (nullptr == global) {
            throw jni_exception(TRACEMSG("Cannot create global ref for object, class name: [" + cls.name() + "]"));
        }
        env->DeleteLocalRef(local);
        return std::shared_ptr<_jobject>(global, [](jobject obj) {

            auto env = thread_local_jni_env_ptr();
            env->DeleteGlobalRef(obj);
        });
    }()) { }

    jobject_ptr(const jobject_ptr& other) :
    cls(other.cls),
    obj(other.obj) { }

    jobject_ptr& operator=(const jobject_ptr& other) {
        cls = other.cls;
        obj = other.obj;
        return *this;
    }

    jobject operator->() {
        return obj.get();
    }

    jobject get() {
        return obj.get();
    }

    template<typename Result, typename Func, typename... Args>
    Result call_method(const std::string& methodname, const std::string& signature,
            Func func, Args... args) {
        auto env = thread_local_jni_env_ptr();
        jmethodID method = env->GetMethodID(cls.get(), methodname.c_str(), signature.c_str());
        if (nullptr == method) {
            throw jni_exception(TRACEMSG("Cannot find method, name: [" + methodname + "]," +
                    " signature: [" + signature + "], class: [" + cls.name() + "]"));
        }
        Result res = (env.get()->*func)(obj.get(), method, args...);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
            throw jni_exception(TRACEMSG("Exception raised when calling method,"
                    " name: [" + methodname + "], signature: [" + signature + "], class: [" + cls.name() + "]"));
        }
        return res;
    }
};

template<typename... Args>
jobject_ptr jclass_ptr::call_static_object_method(const std::string& methodname, const std::string& signature, Args... args) {
    auto env = thread_local_jni_env_ptr();
    jobject local = call_static_method<jobject>(methodname, signature, &JNIEnv::CallStaticObjectMethod, args...);
    return jobject_ptr(*this, local);
}

} //namespace
}

#endif	/* STATICLIB_JNI_JOBJECT_PTR_HPP */

