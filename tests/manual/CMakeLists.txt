project(systray)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QT_INCLUDE_DIR}
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    )

qt4_add_resources(tests_RC systray.qrc)

# systray
set(systray_SRCS
    main.cpp
    window.cpp
    )

qt4_automoc(${systray_SRCS})

add_executable(systray ${systray_SRCS} ${tests_RC})

target_link_libraries(systray
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )

# delayedmenu
set(delayedmenu_SRCS delayedmenu.cpp)

qt4_automoc(${delayedmenu_SRCS})

add_executable(delayedmenu ${delayedmenu_SRCS} ${tests_RC})

target_link_libraries(delayedmenu
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )
