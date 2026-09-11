#include "LoadedLibs.h"
#include "Printer.h"

#ifdef WIN32
#  include <windows.h>
#  include <psapi.h>
//#  include "dlfcn_win32.h"
#else
#  include <dlfcn.h>
#  include <link.h> // for dl_iterate_phdr
#endif /* WIN32 */

void CLoadedLibs::print_added_libs( int idx )
{
	for(std::size_t i(m_loaded_libraries.size()-1) ; i >= idx ; --i) 
		Printer::instance()->debug(m_loaded_libraries[ i ].c_str());
}


#ifdef WIN32

void CLoadedLibs::save()
{
		//std::vector<std::string> m_asDllList;

		HANDLE hProcess(OpenProcess(	PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
									FALSE, 
									GetCurrentProcessId()));
		if (hProcess) 
		{
			HMODULE hMods[1024];
			DWORD cbNeeded;

			if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) 
			{
				const DWORD SIZE(cbNeeded / sizeof(HMODULE));
				for (DWORD i(0); i < SIZE; ++i) 
				{
				TCHAR szModName[MAX_PATH];

				// Get the full path to the module file.
				if (GetModuleFileNameEx(hProcess,
										hMods[i],
										szModName,
										sizeof(szModName) / sizeof(TCHAR))) 
				{
#ifdef UNICODE
					std::wstring wStr(szModName);
					std::string tModuleName(wStr.begin(), wStr.end());
#else
					std::string tModuleName(szModName);
#endif /* UNICODE */
					if (tModuleName.substr(tModuleName.size()-3) == "dll" ) 
					//if (tModuleName.substr(tModuleName.size()-3) == "dll" || tModuleName.substr(tModuleName.size()-3) == "pyd" ) 
					{
						m_loaded_libraries.push_back(tModuleName);
					}
					}
				}
			}
			CloseHandle(hProcess);
		}
		//return m_asDllList;
}

void CLoadedLibs::unload( int idx )
{
		// Now list the loaded libraries again and start manually unloading them
	// starting from the end
    
	//loaded_libraries = list_loaded_libraries();


	// NB: this below assumes that start_idx != 0, which should always hold true
	for(std::size_t i(m_loaded_libraries.size()-1) ; i >= idx ; --i) 
	{
		Printer::instance()->debug(m_loaded_libraries[ i ].c_str());

		if (m_loaded_libraries[ i ].find("_path.pyd") != std::string::npos)
			continue;
		if (m_loaded_libraries[ i ].find("trimesh.pyd") != std::string::npos)
			continue;
		if (m_loaded_libraries[ i ].find("multiarray.pyd") != std::string::npos)
			continue;
		if (m_loaded_libraries[ i ].find("tcl85.dll") != std::string::npos)
			continue;
		if (m_loaded_libraries[ i ].find("_tkinter.pyd") != std::string::npos)
			continue;

		HINSTANCE pHandle = LoadLibrary( m_loaded_libraries[ i ].c_str() );
		if (pHandle) 
		{
			Printer::instance()->debug( m_loaded_libraries[ i ].c_str() );
			const unsigned int Nmax(50); // Avoid getting stuck in an infinite loop
			//for (unsigned int j(0) ; j < Nmax && !dlclose(pHandle) ; ++j);
			//for (unsigned int j(0) ; j < Nmax && !FreeLibrary(pHandle) ; ++j);
			for (unsigned int j(0) ; j < Nmax ; ++j)
			{
				BOOL res = FreeLibrary( pHandle ); //crash: 'tcl85.dll'
				if( res )
					Printer::instance()->debug("Unloaded");
				else
					break;
			}
		}
	}
}

#else //Linux // TODO: just implement it

void CLoadedLibs::save()
{
}

void CLoadedLibs::unload( int idx )
{
}


#endif
