cmake_minimum_required(VERSION 3.8)
project(database)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/linkedlist.h
        src/pf.h
        src/pf_buffermgr.cc
        src/pf_buffermgr.h
        src/pf_error.cc
        src/pf_filehandle.cc
        src/pf_hashtable.cc
        src/pf_hashtable.h
        src/pf_internal.h
        src/pf_manager.cc
        src/pf_pagehandle.cc
        src/pf_statistics.cc
        src/pf_test1.cc
        src/pf_test2.cc
        src/pf_test3.cc
        src/redbase.h
        src/rm.h
        src/rm_filehandle.cc
        src/rm_filescan.cc
        src/rm_manager.cc
        src/rm_record.cc
        src/rm_rid.cc
        src/rm_rid.h
        src/rm_test.cc
        src/statistics.cc
        src/statistics.h)

add_executable(database ${SOURCE_FILES})