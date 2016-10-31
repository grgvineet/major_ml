set(DIRS
    kmeans
    knn
    linearreg
    randomforest
)

foreach(dir ${DIRS})
    add_subdirectory(${dir})
endforeach()

set(SOURCES
    algo_base.h
    algo_base.cpp
)

# add directory name to sources
set(DIR_SRCS)
foreach(file ${SOURCES})
    set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()

set(SRCS ${DIR_SRCS} ${SRCS} PARENT_SCOPE)
