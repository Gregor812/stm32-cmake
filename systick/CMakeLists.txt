project(Systick CXX)

set(CXX_SOURCES
    "${PROJECT_SOURCE_DIR}/Systick.cpp"
)
set(SOURCES ${CXX_SOURCES})

add_library(Systick ${SOURCES})

target_include_directories(Systick PUBLIC
    "${PROJECT_SOURCE_DIR}"    
    "${PROJECT_SOURCE_DIR}/../CMSIS/Include/"
    "${PROJECT_SOURCE_DIR}/../CMSIS/Device/Include/"
)
