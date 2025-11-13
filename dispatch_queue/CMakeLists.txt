add_compile_definitions(HYRENDER_DEVELOPMENT=1)
add_compile_options(-Wno-deprecated-declarations)

set(QUEUE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND QUEUE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/backend)
list(APPEND QUEUE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/common)

set(SOURCES)

foreach(P IN LISTS QUEUE_PATH)
    file(GLOB _TMP
            ${P}/*.h
            ${P}/*.hpp
            ${P}/*.cpp)
    list(APPEND SOURCES ${_TMP})
endforeach()

#将子目录的文件，添加到父目录的变量中，需要指定作用域
set(ALL_SRC
    ${ALL_SRC}
    ${SOURCES}
    PARENT_SCOPE)


source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

set(QUEUE_INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/backend
    PARENT_SCOPE
)