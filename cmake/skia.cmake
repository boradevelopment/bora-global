include(ExternalProject)

include_directories(${SKIA_SOURCE_DIR})

# Compiler flags
if (MSVC)
    if(ENABLE_ASAN)
    endif ()
endif()

if(ENABLE_ASAN)
    set(ASAN_PREFIX "ASAN")
endif()

if(WIN32)
    set(SKIA_SUFFIX ".bat")
else ()
    set(SKIA_SUFFIX ".sh")
endif ()

if(WIN32)
    set(SHELL_CMD "cmd" "/C")
else()
    set(SHELL_CMD "") # Unix can usually execute .sh directly if permissions are set
endif()

if(NOT WIN32)
    execute_process(COMMAND chmod +x "${SKIA_SOURCE_DIR}/build_all.sh")
    execute_process(COMMAND chmod +x "${SKIA_SOURCE_DIR}/bin/ninja")
endif()

# ----------------------
# DEBUG BUILD
# ----------------------
ExternalProject_Add(
        skia_debug
        SOURCE_DIR ${SKIA_SOURCE_DIR}
        BINARY_DIR ${SKIA_SOURCE_DIR}

        CONFIGURE_COMMAND
        ${SHELL_CMD} 
        "${SKIA_SOURCE_DIR}/build_all${SKIA_SUFFIX}${SCRIPT_EXT}" 
        "${SKIA_BUILD_DIR}" 
        "DEBUG" 
        "${ASAN_CODE}" 
        "${ASAN_PREFIX}"

        INSTALL_COMMAND ""
        BUILD_COMMAND ""

        USES_TERMINAL_CONFIGURE true
)

ExternalProject_Add(
        skia_release
        SOURCE_DIR ${SKIA_SOURCE_DIR}
        BINARY_DIR ${SKIA_SOURCE_DIR}


        CONFIGURE_COMMAND
        ${SHELL_CMD} 
        "${SKIA_SOURCE_DIR}/build_all${SKIA_SUFFIX}${SCRIPT_EXT}" 
        "${SKIA_BUILD_DIR}" 
        "RELEASE" 
        "${ASAN_CODE}" 
        "${ASAN_PREFIX}"


        INSTALL_COMMAND ""
        BUILD_COMMAND ""

        USES_TERMINAL_CONFIGURE true
)


add_custom_target(skia_all
        DEPENDS skia_debug skia_release
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE Debug)
else()
    set(BUILD_TYPE Release)
endif()
file(GLOB_RECURSE BORA_SKIA_LIBRARY ${SKIA_SOURCE_DIR}/../../cached_libs/bskia/${BUILD_TYPE}${ASAN_PREFIX}/*.lib ${SKIA_SOURCE_DIR}/../../cached_libs/bskia/${BUILD_TYPE}${ASAN_PREFIX}/*.lib)
file(GLOB_RECURSE BORA_SKIA_SOURCE ${SKIA_SOURCE_DIR}/src/* )