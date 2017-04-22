# Copyright 2016, alex at staticlibs.net
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required ( VERSION 2.8.12 )

# toolchain setup
set ( STATICLIB_TOOLCHAIN linux_amd64_gcc CACHE STRING "toolchain triplet" )
if ( NOT DEFINED STATICLIB_CMAKE )
    set ( STATICLIB_CMAKE ${CMAKE_CURRENT_LIST_DIR}/../../cmake CACHE INTERNAL "" )    
endif ( )
set ( CMAKE_TOOLCHAIN_FILE ${STATICLIB_CMAKE}/toolchains/${STATICLIB_TOOLCHAIN}.cmake CACHE INTERNAL "" )

# project
project ( staticlib_jni_test CXX )
include ( ${STATICLIB_CMAKE}/staticlibs_common.cmake )

# dependencies
if ( NOT DEFINED STATICLIB_DEPS )
    set ( STATICLIB_DEPS ${CMAKE_CURRENT_LIST_DIR}/../../ CACHE INTERNAL "" )    
endif ( )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_config )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_support )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_concurrent )
staticlib_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../../staticlib_jni )
set ( ${PROJECT_NAME}_DEPS staticlib_jni )
staticlib_pkg_check_modules ( ${PROJECT_NAME}_DEPS_PC REQUIRED ${PROJECT_NAME}_DEPS )

# library
add_library ( ${PROJECT_NAME} SHARED 
        ${CMAKE_CURRENT_LIST_DIR}/jni_test.cpp
        ${CMAKE_CURRENT_LIST_DIR}/JniTest.h )
source_group ( "src" FILES ${CMAKE_CURRENT_LIST_DIR}/jni_test.cpp )
source_group ( "include" FILES ${CMAKE_CURRENT_LIST_DIR}/JniTest.h )
target_include_directories ( ${PROJECT_NAME} BEFORE PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${${PROJECT_NAME}_DEPS_PC_INCLUDE_DIRS} )
target_compile_options ( ${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_DEPS_PC_CFLAGS_OTHER} )

# test
if ( "${STATICLIB_ROOT_SOURCE_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}" )
    message ( STATUS "Compiling test java app" )
    execute_process ( COMMAND javac 
            ${CMAKE_CURRENT_LIST_DIR}/JniTest.java 
            -cp ${CMAKE_CURRENT_LIST_DIR}
            -g
            -d ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
    message ( STATUS "Compiling test java app - done" )
    enable_testing ( )
    add_test ( jni_test 
            java 
            -Djava.library.path=${CMAKE_RUNTIME_OUTPUT_DIRECTORY} 
            -cp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} 
            JniTest )
endif ( )
