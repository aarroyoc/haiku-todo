PROJECT(HaikuToDo C CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

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
"Utility/AutoDeleter.cpp"
"Utility/Internet.cpp"
"Utility/OAuth2.cpp"
"Utility/Utility.cpp"
)
SET(JSON_SRC
"ExternalLibs/SuperEasyJSON/json.cpp"
)
SET(HAIKU_TO_DO_SRC
"HaikuToDo/Application.cpp"
"HaikuToDo/MainWindow.cpp"
"HaikuToDo/Start.cpp"
)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/AppEngine")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/Utility")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/PluginEngine")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/ExternalLibs/SuperEasyJSON")


ADD_LIBRARY(AppEngine ${UTILITY_SRC} ${APPENGINE_SRC} ${PLUGINENGINE_SRC} ${JSON_SRC})

ADD_EXECUTABLE(HaikuToDo ${HAIKU_TO_DO_SRC})
TARGET_LINK_LIBRARIES(HaikuToDo be root tracker curl)
TARGET_LINK_LIBRARIES(HaikuToDo ${AppEngine})
