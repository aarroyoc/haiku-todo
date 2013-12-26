#include "PluginLoader.h"
#include "SynchronizationPlugin.h"

#include <cassert>
#include <dlfcn.h>
#include <stdexcept>


namespace PluginEngine {


const char* const PluginLoader::PluginFactorySymbol
									= "CreateSynchronizationPlugin";
const char* const PluginLoader::PluginDestructorSymbol
									= "DestroySynchronizationPlugin";


//////////////////////////////////////////////////////////////////////////////
//
//	Constructor & destructor
//
//////////////////////////////////////////////////////////////////////////////


PluginLoader::PluginLoader(std::string filename)
	:
	fLibraryHandle(nullptr),
	fPlugin(nullptr)
{
	Load(filename);
}


PluginLoader::~PluginLoader()
{
	if (IsPluginLoaded())
		Unload();	
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - getters
//
//////////////////////////////////////////////////////////////////////////////


bool
PluginLoader::IsPluginLoaded() const
{
	return fPlugin != nullptr;	
}


SynchronizationPlugin&
PluginLoader::GetPlugin() const
{
	assert(IsPluginLoaded());
	return *fPlugin;	
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - setters
//
//////////////////////////////////////////////////////////////////////////////


void
PluginLoader::Load(std::string filename)
{
	assert(fPlugin == nullptr);
	
	if (filename == "") {
		// Get default plugin (no synchronization)
		fPlugin = new SynchronizationPlugin();
		return;
	}
	
	fLibraryHandle = dlopen(filename.c_str(), RTLD_LAZY);
	if (fLibraryHandle == nullptr) {
		throw std::runtime_error("PluginLoader::Load() - Cannot load library "
			"\"" + filename + "\" due to: " + dlerror());
	}
	
	try {
		PluginFactoryPtr factory = reinterpret_cast<PluginFactoryPtr>(
			_GetSymbol(PluginFactorySymbol));
		_GetSymbol(PluginDestructorSymbol);
			// Just ensure to get error here instead during unloading
	
		fPlugin = factory();
	}
	catch (std::runtime_error) {
		dlclose(fLibraryHandle);
		fLibraryHandle = nullptr;
		
		throw;	
	}
	
	fPluginFilename = filename;
}


void
PluginLoader::Unload()
{
	assert(fPlugin != nullptr);
	
	if (fLibraryHandle == nullptr) {
		// using default plugin (no synchronization)
		delete fPlugin;
		fPlugin = nullptr;
		return;	
	}
	
	PluginDestructorPtr destructor = reinterpret_cast<PluginDestructorPtr>(
		_GetSymbol(PluginDestructorSymbol));
	destructor(fPlugin);
	dlclose(fLibraryHandle);
	fLibraryHandle = fPlugin = nullptr;
	fPluginFilename = "";
}


//////////////////////////////////////////////////////////////////////////////
//
//	Private methods
//
//////////////////////////////////////////////////////////////////////////////


void*
PluginLoader::_GetSymbol(std::string symbol) const
{
	assert(fLibraryHandle != nullptr);
	
	while (dlerror());
		// Clear any existing error
	
	void* exportedSymbol = dlsym(fLibraryHandle, symbol.c_str());
	
	if (exportedSymbol == nullptr) {		
		std::string message = "PluginLoader::_GetSymbol() - Symbol \"" + symbol
			+ "\"not found in library \"" + fPluginFilename + "\"";
		const char* error = dlerror();
		if (error != nullptr)
			message += std::string(" due to: ") + error;
		
		throw std::runtime_error(message);
	}
	
	while (dlerror());
		// Clear any existing error
	
	return exportedSymbol;
}


} // namespace PluginEngine
