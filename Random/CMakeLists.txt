project(Random CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CXX_SOURCES
    "${PROJECT_SOURCE_DIR}/Random.cpp"
)
set(SOURCES ${CXX_SOURCES})

add_library(Random ${SOURCES})

target_include_directories(Random PUBLIC
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/../CMSIS/Include"
    "${PROJECT_SOURCE_DIR}/../CMSIS/Device/Include"
)

target_link_libraries(Random PUBLIC
   F4-CMSIS
)
