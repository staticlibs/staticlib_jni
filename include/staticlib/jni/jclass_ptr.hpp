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
#define STATICLIB_JNI_JCLASS_PTR_HPP

#include <functional>
#include <memory>
#include <string>

#include <jni.h>

#include "staticlib/config.hpp"
#include "staticlib/support.hpp"

#include "staticlib/jni/jni_exception.hpp"
#include "staticlib/jni/thread_local_jni_env_ptr.hpp"

namespace staticlib {
namespace jni {

// forward decl
class jobject_ptr;

/**
 * Smart pointer that wraps JNI's `jclass` pointer.
 * Can be shared between JNI environments (threads).
 * Global reference to target `jclass` is deallocated after 
 * all copies of this object are destroyed.
 */
class jclass_ptr {
    std::string clsname;
    std::shared_ptr<_jclass> cls;

public:
    /**
     * Constructor
     * 
     * @param classname name (in JNI notation) of the class to request from JVM
     */
    jclass_ptr(const std::string& classname) :
    clsname(classname.data(), classname.length()),
    cls([this] {
        auto env = thread_local_jni_env_ptr();
        jclass clazz_local = env->FindClass(clsname.c_str());
        if (nullptr == clazz_local) {
            throw jni_exception(TRACEMSG("Cannot load class, name: [" + clsname + "]"));
        }
        auto clazz = std::unique_ptr<_jclass, std::function<void(jclass)>>(clazz_local, [&env] (jclass cl) {
            env->DeleteLocalRef(cl);
        });
        jclass clazz_global = static_cast<jclass> (env->NewGlobalRef(clazz.get()));
        if (nullptr == clazz_global) {
            throw jni_exception(TRACEMSG("Cannot create global ref for class, name: [" + clsname + "]"));
        }
        return std::shared_ptr<_jclass>(clazz_global, [](jclass clazz) {
            auto env = thread_local_jni_env_ptr();
            env->DeleteGlobalRef(clazz);
        });
    }()) { }

    /**
     * Copy constructor
     * 
     * @param other other instance
     */
    jclass_ptr(const jclass_ptr& other) :
    clsname(other.clsname.data(), other.clsname.length()),
    cls(other.cls) { }

    /**
     * Copy assignment operator
     * 
     * @param other other instance
     * @return this instance
     */
    jclass_ptr& operator=(const jclass_ptr& other) {
        clsname = std::string(other.clsname.data(), other.clsname.length());
        cls = other.cls;
        return *this;
    }

    /**
     * Provides access to `jclass` pointer
     * 
     * @returns pointer to `jclass`
     */
    jclass operator->() {
        return cls.get();
    }

    /**
     * Provides access to `jclass` pointer
     * 
     * @returns pointer to `jclass`
     */
    jclass get() {
        return cls.get();
    }

    /**
     * Class name (in JNI notation)
     * 
     * @return class name
     */
    const std::string& name() const {
        return clsname;
    }

    /**
     * Calls arbitrary java static method on current class object.
     * 
     * @param methodname name of the method to call
     * @param signature method signature (in JNI notation)
     * @param func pointer to `JNIEnv's`member method to use for calling
     * @param args method arguments
     * @return method output
     * @throws jni_exception on non-existed method or java exception
     */
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

    /**
     * Calls arbitrary java static method, that returns java object, on current class object.
     * 
     * @param resclass result class
     * @param methodname name of the method to call
     * @param signature method signature (in JNI notation)
     * @param args args method arguments
     * @return resulting object
     * @throws jni_exception on non-existed method or java exception
     */
    template<typename... Args>
    jobject_ptr call_static_object_method(const jclass_ptr& resclass, 
            const std::string& methodname, const std::string& signature, Args... args);
};

} // namespace
}

#endif /* STATICLIB_JNI_JCLASS_PTR_HPP */

