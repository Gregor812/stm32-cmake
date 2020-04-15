project(systick CXX)

set(CXX_SOURCES
    "${PROJECT_SOURCE_DIR}/systick.cpp"
)
set(SOURCES ${CXX_SOURCES})

add_library(systick ${SOURCES})

target_include_directories(systick PUBLIC
    "${PROJECT_SOURCE_DIR}"    
    "${PROJECT_SOURCE_DIR}/../CMSIS/Include/"
    "${PROJECT_SOURCE_DIR}/../CMSIS/Device/Include/"
)
