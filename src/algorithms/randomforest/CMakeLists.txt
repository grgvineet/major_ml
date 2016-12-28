# Define the files that we need to compile.
# Anything not in this list will not be compiled into mlpack.
set(SOURCES
        randomforest.h
        randomforest.cpp
        serialise_opencv_mat.h
        serialise_opencv_mat.cpp
        data_structures.cpp
        core_functions.h
        core_functions.cpp
        )

# add directory name to sources
set(DIR_SRCS)
foreach(file ${SOURCES})
    set(BOOST_LIBRARYDIR ~/Documents/boost_1_61_0/libs/)
    set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()

set(SRCS ${SRCS} ${DIR_SRCS} PARENT_SCOPE)
