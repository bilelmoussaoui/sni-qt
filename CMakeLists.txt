project(sni-qt)
cmake_minimum_required(VERSION 2.8.0)

# Versions
set(sni_qt_VERSION_MAJOR 0)
set(sni_qt_VERSION_MINOR 2)
set(sni_qt_VERSION_PATCH 6)
set(sni_qt_VERSION ${sni_qt_VERSION_MAJOR}.${sni_qt_VERSION_MINOR}.${sni_qt_VERSION_PATCH})

# Packaging
set(ARCHIVE_NAME sni-qt-${sni_qt_VERSION})
add_custom_target(dist
    COMMAND bzr export --root=${ARCHIVE_NAME} ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

add_custom_target(distcheck
    COMMAND cd ${CMAKE_BINARY_DIR}
    && rm -rf ${ARCHIVE_NAME}
    && tar xf ${ARCHIVE_NAME}.tar.bz2
    && mkdir ${ARCHIVE_NAME}/build
    && cd ${ARCHIVE_NAME}/build
    && cmake -DCMAKE_INSTALL_PREFIX=../install ..
    && make
    && make check
    && make install
    )
add_dependencies(distcheck dist)

# Dependencies
find_package(PkgConfig REQUIRED)
find_package(Qt4 REQUIRED)
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual -Wall -std=c++0x")

# Build with all symbols hidden. The symbols declared by the Q_EXPORT_PLUGIN2
# macro are explicitly marked as visible, so the plugin can be loaded.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

# Source
add_subdirectory(src)
add_subdirectory(tests/auto)
add_subdirectory(tests/manual)
