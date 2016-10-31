set(SOURCE_FILES
    kmeans.cpp
)

add_executable(kmeans ${SOURCE_FILES})
target_link_libraries(kmeans major)
