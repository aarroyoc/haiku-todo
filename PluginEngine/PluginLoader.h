#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H


#include <string>


namespace PluginEngine {

class SynchronizationPlugin;

class PluginLoader {
	public:
									PluginLoader() = default;
									PluginLoader(std::string filename);
									~PluginLoader();
	
		void						Load(std::string filename);
		void						Unload();
		bool						IsPluginLoaded() const;
		SynchronizationPlugin&		GetPlugin() const;
		
		
	private:
		void*						fLibraryHandle;
		SynchronizationPlugin*		fPlugin;
	
};

} // namespace PluginEngine


#endif // PLUGINLOADER_H
