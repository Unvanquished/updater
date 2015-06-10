find_package(Qt5Core REQUIRED)
# set all include directories for in and out of source builds
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
)

file(GLOB SRCS "*.c" "*.cpp")
file(GLOB PUBLIC_HEADERS "*.h")

# Must be added to enable export macro
add_definitions(-DQUAZIP_STATIC -DQUAZIP_BUILD)

set(SRCS ${SRCS} ${PUBLIC_HEADERS})

add_library(quazip STATIC ${SRCS})

