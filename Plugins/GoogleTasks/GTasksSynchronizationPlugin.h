#ifndef GTASKSSYNCHRONIZATIONPLUGIN_H
#define GTASKSSYNCHRONIZATIONPLUGIN_H


#include "SynchronizationPlugin.h"


namespace Plugin {

// extern "C" due to dynamic loading (PluginLoader searches symbols)
extern "C"
void CreateSynchronizationPlugin(
	AutoDeleter<PluginEngine::SynchronizationPlugin>& plugin);


} // namespace Plugin


#endif // GTASKSSYNCHRONIZATIONPLUGIN_H
