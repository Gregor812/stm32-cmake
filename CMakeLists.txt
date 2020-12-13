cmake_minimum_required(VERSION 3.10)

project(F249_Display VERSION 1.0
    LANGUAGES C ASM CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions(-DSTM32F429xx)

add_subdirectory(CMSIS)
add_subdirectory(Systick)
add_subdirectory(Random)
add_subdirectory(Application)

set(TARGET_NAME firmware)
set(EXE_NAME ${TARGET_NAME}.elf)

add_executable(${EXE_NAME})

target_include_directories(${EXE_NAME} PUBLIC
    "${PROJECT_SOURCE_DIR}/Application"
    "${PROJECT_SOURCE_DIR}"
)

target_sources(
    ${EXE_NAME}
    PRIVATE
    ${PROJECT_SOURCE_DIR}/main.cpp
)

configure_file(config.h.in ${PROJECT_SOURCE_DIR}/config.h)

target_link_libraries(${EXE_NAME} PUBLIC
   Application
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BUILD_DIR}/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_BUILD_DIR}/${TARGET_NAME}.hex)
set(BIN_FILE ${PROJECT_BUILD_DIR}/${TARGET_NAME}.bin)

add_custom_command(TARGET ${EXE_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${EXE_NAME}> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${EXE_NAME}> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")
