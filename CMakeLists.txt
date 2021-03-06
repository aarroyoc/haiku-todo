PROJECT(HaikuToDo C CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

OPTION(BUILD_SQLITE "Build with SQLite" OFF)

SET(HAIKU_TO_DO_SRC
"HaikuToDo/Application.cpp"
"HaikuToDo/MainWindow.cpp"
"HaikuToDo/Start.cpp"
"HaikuToDo/TaskGoogle.cpp"
)
IF(BUILD_SQLITE)
	LIST(APPEND HAIKU_TO_DO_SRC "HaikuToDo/TaskSQL.cpp")
ELSE()
	LIST(APPEND HAIKU_TO_DO_SRC "HaikuToDo/TaskFS.cpp")
ENDIF()

ADD_EXECUTABLE(HaikuToDo ${HAIKU_TO_DO_SRC})
TARGET_LINK_LIBRARIES(HaikuToDo be root tracker netapi network shared)
IF(BUILD_SQLITE)
	ADD_DEFINITIONS(-DBUILD_SQLITE)
	TARGET_LINK_LIBRARIES(HaikuToDo sqlite3)
ENDIF()
