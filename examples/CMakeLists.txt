include_directories(${PROJECT_SOURCE_DIR}/src)

set(DIRS
    algorithms
)

foreach(dir ${DIRS})
    add_subdirectory(${dir})
endforeach()
