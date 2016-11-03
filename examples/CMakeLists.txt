set(DIRS
    algorithms
)

foreach(dir ${DIRS})
    add_subdirectory(${dir})
endforeach()
