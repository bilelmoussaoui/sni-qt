# Macros to create "check" target
macro(add_test_executable _test)
    set(_src ${ARGN})
    qt4_generate_moc(${_test}.cpp ${_test}.moc)
    qt4_automoc(${_src})
    add_executable(${_test} ${_test}.cpp ${_test}.moc ${_src} ${SRC_DIR}/debug.cpp ${SRC_DIR}/settings.cpp ${test_RC})
    target_link_libraries(${_test} ${test_LIBRARIES})

    add_test(${_test} ${_test})
    add_dependencies(check ${_test})
endmacro(add_test_executable)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

qt4_add_resources(test_RC test.qrc)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

include_directories(
    ${SRC_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDE_DIR}
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTTEST_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR}
    )

set(test_LIBRARIES
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTDBUS_LIBRARY}
    ${QT_QTTEST_LIBRARY}
    dbusmenu-qt
    )

enable_testing()

add_test_executable(dbusimagetest ${SRC_DIR}/dbusimage.cpp)
add_test_executable(fsutilstest ${SRC_DIR}/fsutils.cpp)
add_test_executable(iconcachetest ${SRC_DIR}/iconcache.cpp ${SRC_DIR}/fsutils.cpp)
