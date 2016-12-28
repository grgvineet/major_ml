set(SOURCE_FILES
    randomforest.cpp
)

add_executable(randomforest ${SOURCE_FILES})
target_link_libraries(randomforest major)