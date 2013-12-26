#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H


#include <string>


namespace PluginEngine {

class SynchronizationPlugin;

class PluginLoader {
	public:
									PluginLoader(); // = default;
									PluginLoader(std::string filename);
									~PluginLoader();
	
		void						Load(std::string filename);
		void						Unload();
		bool						IsPluginLoaded() const;
		SynchronizationPlugin&		GetPlugin() const;
		
		
	private:
		void*						_GetSymbol(std::string symbol) const;
										// Helper function for Load() and
										// Unload(). CAUTION! If symbol is not
										// found exception will be thrown.
		
		
	private:
		void*						fLibraryHandle;
		SynchronizationPlugin*		fPlugin;
		std::string					fPluginFilename;
			// variable for std::runtime_error in _GetSymbol
		
		
	private:
		static const char* const	PluginFactorySymbol;
									//	= "CreateSynchronizationPlugin";
		static const char* const	PluginDestructorSymbol;
									//	= "DestroySynchronizationPlugin";
	
		typedef SynchronizationPlugin* (*PluginFactoryPtr)();
		typedef void (*PluginDestructorPtr)(SynchronizationPlugin*);
};

} // namespace PluginEngine


#endif // PLUGINLOADER_H
