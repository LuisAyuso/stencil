project(Stencil)
cmake_minimum_required(VERSION 2.8)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include (FindX11)

# -------------------------------------------------------------------

if(APPLE)
    include_directories(AFTER "/opt/X11/include")
    set(NO_TEST True)
endif()


if (CMAKE_COMPILER_IS_GNUCC)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                    OUTPUT_VARIABLE GCC_VERSION)
    string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
    list(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
    list(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)

endif()

# -------------------------------------------------------------------
# check for intel compiler if target is mic

if (COMPILE_MIC)

	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmic")
	else()
		message( FATAL_ERROR "mic requires intel compiler, you could use gcc... up to you")
	endif()

    set(NO_TEST True)

	# the mic binary can not run and cilk test fails, since
	# this is intel mic, enable it. It should work
endif()

# -------------------------------------------------------------------
# add flag to compile trace instrumentation

if (INSTRUMENT)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DINSTRUMENT=1")
	message(STATUS " INSTRUMENT TRACES")
endif()

# -------------------------------------------------------------------

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )


#	Configure build 
#	===============
#	include path, compiler flags
include_directories ( "include/" )
include_directories ( "cimg/include/" )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# ========================== SEQUENTIAL =====================================

# 1d exec
add_executable("Stencil1D" "src/main1D.cxx" ${sources} )

# 2d exec
add_executable("Stencil2D" "src/main2D.cxx" ${sources} )

# 3d exec
add_executable("Stencil3D" "src/main3D.cxx" ${sources} )


# ========================== OPENMP =====================================

if (NOT NO_OMP)
find_package(OpenMP)
if (OPENMP_FOUND)

	# 1d exec 
	add_executable	     ("Stencil1D-omp" "src/main1D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil1D-omp" PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
	SET_TARGET_PROPERTIES("Stencil1D-omp" PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})

	# 2d exec
	add_executable	     ("Stencil2D-omp" "src/main2D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil2D-omp" PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
	SET_TARGET_PROPERTIES("Stencil2D-omp" PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})

	# 3d exec
	add_executable		 ("Stencil3D-omp" "src/main3D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil3D-omp" PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
	SET_TARGET_PROPERTIES("Stencil3D-omp" PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})

endif()
endif()

# ========================== CILK =====================================

find_package(Cilk)
if (CILK_FOUND OR COMPILE_MIC)

	# 1d exec
	add_executable		 ("Stencil1D-cilk" "src/main1D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil1D-cilk" PROPERTIES COMPILE_FLAGS "-DCILK ${Cilk_CXX_FLAGS}")
	if (Cilk_LINK_LIBRARIES)
		SET_TARGET_PROPERTIES("Stencil1D-cilk" PROPERTIES LINK_FLAGS ${Cilk_LINK_LIBRARIES})
	endif()

	# 2d exec
	add_executable		 ("Stencil2D-cilk" "src/main2D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil2D-cilk" PROPERTIES COMPILE_FLAGS "-DCILK ${Cilk_CXX_FLAGS}")
	if (Cilk_LINK_LIBRARIES)
		SET_TARGET_PROPERTIES("Stencil2D-cilk" PROPERTIES LINK_FLAGS ${Cilk_LINK_LIBRARIES})
	endif()

	# 3d exec
	add_executable		 ("Stencil3D-cilk" "src/main3D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil3D-cilk" PROPERTIES COMPILE_FLAGS "-DCILK ${Cilk_CXX_FLAGS}")
	if (Cilk_LINK_LIBRARIES)
		SET_TARGET_PROPERTIES("Stencil3D-cilk" PROPERTIES LINK_FLAGS ${Cilk_LINK_LIBRARIES})
	endif()

endif()

# ========================== CXX_ASYNC PARALLELISM ==================================
if (NOT COMPILE_MIC) # sorry, mic does not like c++11 async

	# 1d exec
	add_executable		 ("Stencil1D-cxx" "src/main1D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil1D-cxx" PROPERTIES COMPILE_FLAGS "-DCXX_ASYNC")

	# 2d exec
	add_executable		 ("Stencil2D-cxx" "src/main2D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil2D-cxx" PROPERTIES COMPILE_FLAGS "-DCXX_ASYNC")

	# 3d exec
	add_executable		 ("Stencil3D-cxx" "src/main3D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil3D-cxx" PROPERTIES COMPILE_FLAGS "-DCXX_ASYNC")

endif()

# ========================== INSIEME RT =====================================

if(INSIEME_CODE_PATH)

	message(STATUS "compile InsiemeRT version")

	set(RT_HEADERS ${INSIEME_CODE_PATH}/runtime/include)
	set(COMMON_HEADERS ${INSIEME_CODE_PATH}/common/include)

	message(STATUS "INSIEME RT: ${RT_HEADERS}")

	# 1d exec
	add_executable		 ("Stencil1D-insiemert" "src/main1D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil1D-insiemert" PROPERTIES COMPILE_FLAGS "-I ${RT_HEADERS} -I ${COMMON_HEADERS} -DINSIEME_RT -DIRT_STEAL_OTHER_POP_FRONT")

	add_executable		 ("Stencil2D-insiemert" "src/main2D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil2D-insiemert" PROPERTIES COMPILE_FLAGS "-I ${RT_HEADERS} -I ${COMMON_HEADERS} -DINSIEME_RT -DIRT_STEAL_OTHER_POP_FRONT")

	# 3d exec
	add_executable		 ("Stencil3D-insiemert" "src/main3D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil3D-insiemert" PROPERTIES COMPILE_FLAGS "-I ${RT_HEADERS} -I ${COMMON_HEADERS} -DINSIEME_RT -DIRT_STEAL_OTHER_POP_FRONT")


endif()


# ========================== MPI =====================================

find_package(MPI)
if(MPI_CXX_FOUND)

	# 3d exec
	add_executable		 ("Stencil3D-mpi" "src/main3D.cxx" ${sources} )

endif()


# ================================= IMAGE VERSION =================================

if (IMAGE)
	
	add_executable	("StencilImg" "src/image2D.cxx" ${sources} )
	add_dependencies("StencilImg" cimg)
	target_link_libraries("StencilImg" ${X11_X11_LIB})
endif()


# ================================= TESTS =================================

if (NOT NO_TEST)
	message(STATUS " ~~~~~~~~~~~~Compile tests ~~~~~~~~~~~~~")

    file(GLOB_RECURSE test_sources "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc" )

    foreach( test_source ${test_sources} )

        get_filename_component (unit_test ${test_source} NAME_WE )

        include_directories( ${GTEST_INCLUDE_DIR} )
        add_definitions(-DGTEST_COLOR ) 
        add_executable (${unit_test} ${test_source})
        target_link_libraries(${unit_test} ${GTEST_LIBRARY})
        target_link_libraries(${unit_test} ${GTEST_LIBRARY_WITH_MAIN})
        target_link_libraries(${unit_test} ${X11_X11_LIB})
        add_test (${unit_test} ${unit_test} )

    endforeach(test_source)

    enable_testing()

endif()

