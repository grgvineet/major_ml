set(DIRS
        kmeans
        knn
        linearreg
        randomforest
        )

foreach(dir ${DIRS})
    add_subdirectory(${dir})
endforeach()
