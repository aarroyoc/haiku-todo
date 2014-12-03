PROJECT(HaikuToDo C CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

OPTION(BUILD_OLD_PROJECT "Build old project" OFF)

SET(APPENGINE_SRC
"AppEngine/Task.cpp"
"AppEngine/TaskList.cpp"
"AppEngine/TaskListManager.cpp"
)
SET(PLUGINENGINE_SRC
"PluginEngine/PluginLoader.cpp"
"PluginEngine/SynchronizationPlugin.cpp"
"PluginEngine/TaskSerializer.cpp"
"PluginEngine/TaskSynchronizer.cpp"
)
SET(UTILITY_SRC
#"Utility/AutoDeleter.cpp"
"Utility/Internet.cpp"
"Utility/OAuth2.cpp"
"Utility/Utility.cpp"
)
SET(JSON_SRC
"ExternalLibs/SuperEasyJSON/json.cpp"
)
SET(GTASKS_SRC
"Plugins/GoogleTasks/GTasksSynchronizationPlugin.cpp"
"Plugins/GoogleTasks/GTasksSynchronizer.cpp"
"Plugins/GoogleTasks/Utility/TimeRfc3339.cpp"
)
SET(HAIKU_TO_DO_SRC
"HaikuToDo/Application.cpp"
"HaikuToDo/MainWindow.cpp"
"HaikuToDo/Start.cpp"
"HaikuToDo/TaskLocal.cpp"
)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/AppEngine")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/Utility")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/PluginEngine")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/ExternalLibs/SuperEasyJSON")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/Plugins/GoogleTasks")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/Plugins/GoogleTasks/Utility")

IF(BUILD_OLD_PROJECT)
	ADD_LIBRARY(AppEngine ${UTILITY_SRC} ${APPENGINE_SRC} ${PLUGINENGINE_SRC})

	ADD_LIBRARY(SuperEasyJSON ${JSON_SRC})

	ADD_LIBRARY(GoogleTasks SHARED ${GTASKS_SRC})

ENDIF()

ADD_EXECUTABLE(HaikuToDo ${HAIKU_TO_DO_SRC})
TARGET_LINK_LIBRARIES(HaikuToDo be root tracker sqlite3)
#TARGET_LINK_LIBRARIES(HaikuToDo AppEngine SuperEasyJSON curl)
