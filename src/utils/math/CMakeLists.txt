set(SOURCES
        euclidean_distance.cpp
        euclidean_distance.h
        )

# add directory name to sources
set(DIR_SRCS)
foreach(file ${SOURCES})
    set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()

set(SRCS ${DIR_SRCS} ${SRCS} PARENT_SCOPE)
