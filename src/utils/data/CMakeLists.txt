# Define the files that we need to compile.
# Anything not in this list will not be compiled into mlpack.
set(SOURCES
        data_frame.cpp
        data_frame.h
        data_row.cpp
        data_row.h
        big_data.cpp
        big_data.h
        )

set(DIRS
        server
        )

foreach(dir ${DIRS})
    add_subdirectory(${dir})
endforeach()

# add directory name to sources
set(DIR_SRCS)
foreach(file ${SOURCES})
    set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()

set(SRCS ${SRCS} ${DIR_SRCS} PARENT_SCOPE)
