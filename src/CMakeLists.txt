set(DIRS
    algorithms
    utils
)

foreach(dir ${DIRS})
    add_subdirectory(${dir})
endforeach()

set(SRCS ${SRCS} PARENT_SCOPE)

