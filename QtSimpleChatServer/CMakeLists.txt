project(QtSimpleChatServer LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 5.7 COMPONENTS Core Gui Widgets Network REQUIRED)
add_executable(SimpleChatServer
    chatserver.cpp
    servermain.cpp
    serverworker.cpp
    serverwindow.cpp
    chatserver.h
    serverworker.h
    serverwindow.h
    serverwindow.ui
)
target_link_libraries(SimpleChatServer PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
target_compile_definitions(SimpleChatServer PRIVATE QT_NO_CAST_FROM_ASCII)
set_target_properties(SimpleChatServer PROPERTIES
	AUTOMOC ON
	AUTOUIC ON
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
	VERSION "1.0.0"
	EXPORT_NAME SimpleChatServer
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

