project(Random CXX)

set(CXX_SOURCES
    "${PROJECT_SOURCE_DIR}/Random.cpp"
)
set(SOURCES ${CXX_SOURCES})

add_library(Random ${SOURCES})

target_include_directories(Random PUBLIC
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/../CMSIS/Include/"
    "${PROJECT_SOURCE_DIR}/../CMSIS/Device/Include/"
)
