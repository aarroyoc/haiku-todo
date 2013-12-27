#include "PluginLoader.h"
#include "SynchronizationPlugin.h"
#include "Utility.h"

#include <cassert>
#include <dlfcn.h>
#include <iostream> // For error reporting
#include <stdexcept>


namespace PluginEngine {


const BString PluginLoader::PluginFactorySymbol
								= "CreateSynchronizationPlugin";


//////////////////////////////////////////////////////////////////////////////
//
//	Constructor & destructor
//
//////////////////////////////////////////////////////////////////////////////


PluginLoader::PluginLoader()
	:
	fLibraryHandle(nullptr),
	fPlugin(),
	fPluginFilename()
{
}


PluginLoader::PluginLoader(BString filename)
	:
	fLibraryHandle(nullptr),
	fPlugin(),
	fPluginFilename()
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
	return fPlugin.GetPointer() != nullptr;	
}


SynchronizationPlugin&
PluginLoader::GetPlugin() const
{
	assert(IsPluginLoaded());
	return *fPlugin.GetPointer();	
}


BString
PluginLoader::GetPluginFilename() const
{
	assert(IsPluginLoaded());
	return fPluginFilename;	
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - setters
//
//////////////////////////////////////////////////////////////////////////////


bool
PluginLoader::Load(BString filename)
{
	assert(fPlugin.GetPointer() == nullptr);
	
	if (filename == "") {
		// Get default plugin (no synchronization)
		fPlugin.Reset(new SynchronizationPlugin());
		return true;
	}
	
	fLibraryHandle = dlopen(filename.String(), RTLD_LAZY);
	if (fLibraryHandle == nullptr) {
		BString message = ("PluginLoader::Load() - Cannot load library "
			"\"" + filename + "\" due to: " + dlerror());
		//throw std::runtime_error(message.String());
		std::cerr << message.String() << std::endl;
		return false;
	}
	
	try {
		PluginFactoryPtr factory = reinterpret_cast<PluginFactoryPtr>(
			_GetSymbol(PluginFactorySymbol.String()));
		
		factory(fPlugin);
	}
	catch (const std::runtime_error& error) {
		dlclose(fLibraryHandle);
		fLibraryHandle = nullptr;
		
		//throw
		std::cerr << error.what() << std::endl;
		return false;
	}
	
	fPluginFilename = filename;
	return true;
}


void
PluginLoader::Unload()
{
	assert(fPlugin.GetPointer() != nullptr);
	
	fPlugin.Reset(nullptr);
	fPluginFilename = "";
	
	if (fLibraryHandle != nullptr) {
		// Free library if it is loaded (all cases excluding default plugin)
		dlclose(fLibraryHandle);
		fLibraryHandle = nullptr;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//	Private methods
//
//////////////////////////////////////////////////////////////////////////////


void*
PluginLoader::_GetSymbol(BString symbol) const
{
	assert(fLibraryHandle != nullptr);
	
	while (dlerror());
		// Clear any existing error
	
	void* exportedSymbol = dlsym(fLibraryHandle, symbol.String());
	
	if (exportedSymbol == nullptr) {		
		BString message = "PluginLoader::_GetSymbol() - Symbol \"" + symbol
			+ "\"not found in library \"" + fPluginFilename + "\"";
		const char* error = dlerror();
		if (error != nullptr)
			message += BString(" due to: ") + error;
		
		throw std::runtime_error(message.String());
	}
	
	while (dlerror());
		// Clear any existing error
	
	return exportedSymbol;
}


} // namespace PluginEngine
