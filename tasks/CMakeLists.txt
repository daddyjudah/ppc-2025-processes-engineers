project(parallel_programming_course LANGUAGES C CXX)

message(STATUS "Student's tasks")

# Test runner executables
set(FUNC_TEST_EXEC ppc_func_tests)
set(PERF_TEST_EXEC ppc_perf_tests)

# ——— Include helper scripts ——————————————————————————————————————
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

# ——— Initialize test executables —————————————————————————————————————
ppc_add_test(${FUNC_TEST_EXEC} common/runners/functional.cpp USE_FUNC_TESTS)
ppc_add_test(${PERF_TEST_EXEC} common/runners/performance.cpp USE_PERF_TESTS)

find_package(MPI REQUIRED)
if (MPI_FOUND)
    include_directories(${MPI_INCLUDE_PATH})
    target_link_libraries(${FUNC_TEST_EXEC} PRIVATE ${MPI_LIBRARIES})
    target_link_libraries(${PERF_TEST_EXEC} PRIVATE ${MPI_LIBRARIES})
else()
    include_directories("C:/Program Files (x86)/Microsoft SDKs/MPI/Include")
    link_directories("C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64")
    target_link_libraries(${FUNC_TEST_EXEC} PRIVATE msmpi)
    target_link_libraries(${PERF_TEST_EXEC} PRIVATE msmpi)
endif()

# ——— List of implementations ————————————————————————————————————————
set(PPC_IMPLEMENTATIONS "all;mpi;omp;seq;stl;tbb" CACHE STRING "Implementations to build (semicolon-separated)")

# ——— Configure each subproject —————————————————————————————————————
file(
  GLOB subdirs
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/*")
foreach(sub IN LISTS subdirs)
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${sub}" AND NOT sub STREQUAL
                                                           "common")
    ppc_configure_subproject(${sub})
  endif()
endforeach()

