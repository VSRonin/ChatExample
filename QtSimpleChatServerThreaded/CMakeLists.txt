project(QtSimpleChatServerThreaded LANGUAGES CXX)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.7 COMPONENTS Core Gui Widgets Network REQUIRED)
add_executable(SimpleChatServerThreaded
    chatserver.cpp
    servermain.cpp
    serverworker.cpp
    serverwindow.cpp
    chatserver.h
    serverworker.h
    serverwindow.h
    serverwindow.ui
)
target_link_libraries(SimpleChatServerThreaded PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)
target_include_directories(SimpleChatServerThreaded PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_compile_definitions(SimpleChatServerThreaded PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII)
set_target_properties(SimpleChatServerThreaded PROPERTIES
	AUTOMOC ON
	AUTOUIC ON
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
	VERSION "1.0.0"
)

