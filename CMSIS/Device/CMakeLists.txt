project(F4CMSIS)

file(GLOB_RECURSE C_SOURCES "*.c")
file(GLOB_RECURSE ASM_SOURCES "*.s")
set(SOURCES ${C_SOURCES} ${ASM_SOURCES})

add_library(device-cmsis ${SOURCES})

target_include_directories(device-cmsis PUBLIC
    "${PROJECT_SOURCE_DIR}/Include"
    "${PROJECT_SOURCE_DIR}/../Include"
)
