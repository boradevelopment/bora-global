include(ExternalProject)

set(CARGO_PROFILE_FLAG "$<IF:$<CONFIG:Debug>,,--release>")
set(CARGO_OUTPUT_DIR "$<IF:$<CONFIG:Debug>,debug,release>")

if(ENABLE_ASAN)
    set(ASAN_PREFIX "ASAN")
    if(WIN32)
    set(ASAN_CONFIG 
    "RUSTC_BOOTSTRAP=set:1"
    "RUSTFLAGS=set:-Zsanitizer=address"
        )
    endif()
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE Debug)
else()
    set(BUILD_TYPE Release)
endif()

ExternalProject_Add(wasmtime_lib
    SOURCE_DIR "${WASMTIME_SOURCE_DIR}"
    CONFIGURE_COMMAND 
    ${CMAKE_COMMAND} 
     -S ${WASMTIME_SOURCE_DIR}/crates/c-api -B ${WASMTIME_SOURCE_DIR}/targets/c-api  -DCMAKE_BUILD_TYPE=${CMAKE_CONFIG_TYPE} --install-prefix ${WASMTIME_BUILD_DIR}/${BUILD_TYPE}${ASAN_PREFIX} -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF

    BUILD_ENVIRONMENT_MODIFICATION ${ASAN_CONFIG}

    BUILD_COMMAND  cmake --build ${WASMTIME_SOURCE_DIR}/targets/c-api
    
    CONFIGURE_HANDLED_BY_BUILD ON

    INSTALL_COMMAND cmake --install ${WASMTIME_SOURCE_DIR}/targets/c-api

    USES_TERMINAL_BUILD true
)

link_directories(${WASMTIME_BUILD_DIR}/${BUILD_TYPE}${ASAN_PREFIX}/lib)
include_directories(${WASMTIME_BUILD_DIR}/${BUILD_TYPE}${ASAN_PREFIX}/include)