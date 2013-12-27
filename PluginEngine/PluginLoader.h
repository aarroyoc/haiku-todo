#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H


#include <String.h>


namespace PluginEngine {

class SynchronizationPlugin;

class PluginLoader {
	public:
									PluginLoader(); // = default;
									PluginLoader(BString filename);
									~PluginLoader();
	
		void						Load(BString filename);
		void						Unload();
		bool						IsPluginLoaded() const;
		SynchronizationPlugin&		GetPlugin() const;
		
		
	private:
		void*						_GetSymbol(BString symbol) const;
										// Helper function for Load() and
										// Unload(). CAUTION! If symbol is not
										// found exception will be thrown.
		
		
	private:
		void*						fLibraryHandle;
		SynchronizationPlugin*		fPlugin;
		BString						fPluginFilename;
			// variable for std::runtime_error in _GetSymbol
		
		
	private:
		static const BString		PluginFactorySymbol;
									//	= "CreateSynchronizationPlugin";
		static const BString		PluginDestructorSymbol;
									//	= "DestroySynchronizationPlugin";
	
		typedef SynchronizationPlugin* (*PluginFactoryPtr)();
		typedef void (*PluginDestructorPtr)(SynchronizationPlugin*);
};

} // namespace PluginEngine


#endif // PLUGINLOADER_H
