## ======================================================================== ##
## Copyright 2009-2017 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

option(OSPRAY_MODULE_EXP_BSP "Experimental MPI BSP stuff" OFF)

if (OSPRAY_MODULE_EXP_BSP)

  ospray_configure_mpi()

  SET(CMAKE_THREAD_PREFER_PTHREAD TRUE)
  SET(THREADS_PREFER_PTHREAD_FLAG TRUE) # for gcc6
  FIND_PACKAGE(Threads REQUIRED)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

  include_directories(${CMAKE_CURRENT_LIST_DIR})

  ospray_create_application(bspCommTest
    bspCommTest.cpp
  LINK
    ${MPI_CXX_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
  )

endif ()
