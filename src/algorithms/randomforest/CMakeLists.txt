# Define the files that we need to compile.
# Anything not in this list will not be compiled into mlpack.
set(SOURCES
        randomforest.h
        randomforest.cpp
        helper_functions.cpp
        helper_functions.h
        rf_client.cpp
        rf_data_structures.cpp
        rf_server.cpp
        rf_server.h
        serialise_opencv_mat.h
        serialise_opencv_mat.cpp
        )

# add directory name to sources
set(DIR_SRCS)
foreach(file ${SOURCES})
    set(BOOST_LIBRARYDIR ~/Documents/boost_1_61_0/libs/)
    set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()

set(SRCS ${SRCS} ${DIR_SRCS} PARENT_SCOPE)
