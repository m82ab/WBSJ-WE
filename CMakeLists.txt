cmake_minimum_required(VERSION 3.24)
project(WBSJ_WE VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Network)

qt_standard_project_setup(REQUIRES 6.5)

add_library(wbsj_ipc STATIC
    core/ipc/Protocol.h
    core/ipc/ServiceClient.h
    core/ipc/ServiceClient.cpp
)
target_include_directories(wbsj_ipc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(wbsj_ipc PUBLIC Qt6::Core Qt6::Network Qt6::Gui)

qt_add_executable(WBSJ_WE
    apps/desktop/main.cpp
)
set_source_files_properties(apps/desktop/qml/Main.qml PROPERTIES QT_RESOURCE_ALIAS Main.qml)
qt_add_qml_module(WBSJ_WE
    URI WBSJWE
    VERSION 1.0
    QML_FILES
        apps/desktop/qml/Main.qml
)
target_link_libraries(WBSJ_WE PRIVATE wbsj_ipc Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2)
qt_add_resources(WBSJ_WE wbsj_app_resources PREFIX "/" BASE resources FILES resources/WBSJ_WE_icon.png)
target_compile_definitions(WBSJ_WE PRIVATE WBSJ_WE_VERSION="${PROJECT_VERSION}")

add_executable(WBSJ_WE_RecordingService
    apps/recording-service/main.cpp
    core/service/ServiceHost.h
    core/service/ServiceHost.cpp
)
target_include_directories(WBSJ_WE_RecordingService PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(WBSJ_WE_RecordingService PRIVATE Qt6::Core Qt6::Network)
target_compile_definitions(WBSJ_WE_RecordingService PRIVATE WBSJ_WE_VERSION="${PROJECT_VERSION}")

if(WIN32)
    target_compile_definitions(WBSJ_WE_RecordingService PRIVATE _WIN32_WINNT=0x0601 WINVER=0x0601)
    target_link_libraries(WBSJ_WE_RecordingService PRIVATE Advapi32)
    set_target_properties(WBSJ_WE PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

install(TARGETS WBSJ_WE WBSJ_WE_RecordingService RUNTIME DESTINATION .)
install(DIRECTORY config/ DESTINATION config)
