# Define the files that we need to compile.
# Anything not in this list will not be compiled into mlpack.
set(SOURCES
        csv_reader.cpp
        data_frame.cpp
        data_frame.h
        data_row.cpp
        data_row.h
        )

# add directory name to sources
set(DIR_SRCS)
foreach(file ${SOURCES})
    set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()

set(SRCS ${SRCS} ${DIR_SRCS} PARENT_SCOPE)
