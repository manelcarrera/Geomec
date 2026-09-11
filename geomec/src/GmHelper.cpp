#include "stdafx.h"

#include "GmHelper.h"

#include <thread>

namespace  gm{
	namespace  send{
		void gen( const TMsg& d ){					PostMessage( d.hWnd, d.Msg, d.wParam, d.lParam ); }
		//
		void command( WPARAM wParam, LPARAM lParam ){		gen( TMsg{ AfxGetApp()->GetMainWnd()->m_hWnd, WM_COMMAND, wParam, lParam} ); }
		void msg( UINT Msg, WPARAM wParam, LPARAM lParam ){	gen( TMsg{ AfxGetApp()->GetMainWnd()->m_hWnd, Msg, wParam, lParam} ); }
		void quit(){										command(ID_APP_EXIT); } // WM_QUIT -> it doesn't work !!! 
		//
		void minimize(){									msg( WM_SYSCOMMAND, SC_MINIMIZE ); }
		//
		void status( const QString& message, eStatusType type )
		{
			auto qs_to_new_cp=[](const QString& message)
			{
				int size = message.size();
				char* cp = new char[ size + 1 ];
				memset( cp, 0, size + 1 );
				memcpy( cp, message.toStdString().c_str(), size );
				return cp;
			};
			char* cp = qs_to_new_cp(message);

			msg(WM_USER + 200, (WPARAM)type, (LPARAM)cp);
		}

		//
		// simulate a key pressed:
		// https://stackoverflow.com/questions/5607849/how-to-simulate-a-key-press-in-c
		// https://batchloaf.wordpress.com/2012/10/18/simulating-a-ctrl-v-keystroke-in-win32-c-or-c-using-sendinput/
		//
		// ‘V’, VK_KEY_V, 0x56
		//

		void key_stroke(const std::vector<key_ext>& key_v)
		{
			for( key_ext k : key_v )
			{
				for(int i=0; i < k.times; i++)
					key_stroke(k.key);
			}
		}


		void key_stroke(const std::vector<WORD>& key_v)
		{
			for( WORD key : key_v )
			{
				key_stroke(key);
				//
				// not needed
				//
				//std::this_thread::sleep_for(std::chrono::milliseconds( 100 ));
			}
		}

		void key_stroke(WORD key) //'A' -> 0x41
		{
			INPUT ip;

			// Pause for 5 seconds.
			//Sleep(5000);

			//
			// 1) Set up a generic keyboard event.
			//
			ip.type = INPUT_KEYBOARD;
			ip.ki.wScan = 0; // hardware scan code for key
			ip.ki.time = 0;
			ip.ki.dwExtraInfo = 0;


			// Press the "A" key
			ip.ki.wVk = key; // virtual-key code for the "a" key -> mcr: ascii code of capital a
			ip.ki.dwFlags = 0; // 0 for key press
			SendInput(1, &ip, sizeof(INPUT));

			//Sleep(30); // needed for gaming -> mcr: to em not needed so far but maybe better set it just in case

			// Release the "A" key
			ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
			SendInput(1, &ip, sizeof(INPUT));
		}
		//
		// ctrl + key
		//
		void key_stroke_ctrl(WORD key) //'A' -> 0x41
		{
			// Create a generic keyboard event structure
			INPUT ip;
			ip.type = INPUT_KEYBOARD;
			ip.ki.wScan = 0;
			ip.ki.time = 0;
			ip.ki.dwExtraInfo = 0;

			//
			//
			//
 
			// Press the "Ctrl" key
			ip.ki.wVk = VK_CONTROL;
			ip.ki.dwFlags = 0; // 0 for key press
			SendInput(1, &ip, sizeof(INPUT));
 
			// Press the "V" key
			//ip.ki.wVk = 'V';
			ip.ki.wVk = key;
			ip.ki.dwFlags = 0; // 0 for key press
			SendInput(1, &ip, sizeof(INPUT));
 
			// Release the "V" key
			//ip.ki.wVk = 'V';
			ip.ki.wVk = key;
			ip.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &ip, sizeof(INPUT));
 
			// Release the "Ctrl" key
			ip.ki.wVk = VK_CONTROL;
			ip.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &ip, sizeof(INPUT));
		}
	} // namespace  send

	UINT cmd( const QString& key_ )
	{
		QString key = key_.toUpper();
		//
		// 1) GeomecDoc
		//
		if(		key=="ID_FILE_MODEL_PROPERTIES")			return ID_FILE_MODEL_PROPERTIES;
		else if(key=="ID_FILE_SAVE")						return ID_FILE_SAVE;
		else if(key=="ID_FILE_SAVE_AS")						return ID_FILE_SAVE_AS;
		else if(key=="ID_ANALYSIS_LINEAR_DEPLETION")		return ID_ANALYSIS_LINEAR_DEPLETION;
		else if(key=="ID_EDIT_PASTE")						return ID_EDIT_PASTE;
		else if(key=="ID_EDIT_COPY")						return ID_EDIT_COPY;
		else if(key=="ID_ANALYSIS_NONLINEAR_CALCULATION")	return ID_ANALYSIS_NONLINEAR_CALCULATION;
		else if(key=="ID_ANALYSIS_NONLINEAR_CALC_PARAMS")	return ID_ANALYSIS_NONLINEAR_CALC_PARAMS;
		else if(key=="ID_SET_MESH_TOLERANCE")				return ID_SET_MESH_TOLERANCE;
		else if(key=="ID_CREATE_MESH")						return ID_CREATE_MESH;
		else if(key=="ID_INVALIDATE_MESH")					return ID_INVALIDATE_MESH;
		else if(key=="ID_INSERT_DEVIATED_WELLPATH_BY_POINTSET")	return ID_INSERT_DEVIATED_WELLPATH_BY_POINTSET;
		else if(key=="ID_ANALYSIS_PROPERTIES")				return ID_ANALYSIS_PROPERTIES;
		else if(key=="ID_FILE_OPEN")						return ID_FILE_OPEN;
		else if(key=="ID_ANALYSISLOG")						return ID_ANALYSISLOG;
		else if(key=="ID_INPUT_STATUS")						return ID_INPUT_STATUS ;
		else if(key=="ID_EXPORT_LINEAR")					return ID_EXPORT_LINEAR;
		else if(key=="ID_EXPORT_NONLIN")					return ID_EXPORT_NONLIN;
		else if(key=="ID_EXPORT_QUAD_DAT")					return ID_EXPORT_QUAD_DAT;
		else if(key=="ID_EXPORT_RESULTS")					return ID_EXPORT_RESULTS;
		else if(key=="ID_EXPORT_RESULTS_GOCAD")				return ID_EXPORT_RESULTS_GOCAD;
		else if(key=="ID_EXPORT_SURFACES")					return ID_EXPORT_SURFACES;
		else if(key=="ID_IMPORT_IMPORTMESH")				return ID_IMPORT_IMPORTMESH;
		else if(key=="ID_IMPORT_IMPORTRESULTS")				return ID_IMPORT_IMPORTRESULTS;
		else if(key=="ID_FILE_IMPORT")						return ID_FILE_IMPORT;
		else if(key=="ID_IMPORT_IMPORTPOINTELEMENTSETS")	return ID_IMPORT_IMPORTPOINTELEMENTSETS;
		else if(key=="ID_IMPORT_INSERTDEVIATEDWELLPATH")	return ID_IMPORT_INSERTDEVIATEDWELLPATH;
		else if(key=="ID_FILE_CLOSE")						return ID_FILE_CLOSE;
		else if(key=="ID_ANALYSIS_LINEAR_SETTINGS")			return ID_ANALYSIS_LINEAR_SETTINGS;
		else if(key=="ID_ANALYSIS_CLEARHISTORY")			return ID_ANALYSIS_CLEARHISTORY;
		else if(key=="ID_VIEW_SMOOTH_RESULTS")				return ID_VIEW_SMOOTH_RESULTS;
		else if(key=="ID_ANALYSIS_HEAT_FLOW")				return ID_ANALYSIS_HEAT_FLOW;
		else if(key=="ID_EXPORT_HEAT_FLOW")					return ID_EXPORT_HEAT_FLOW;
		else if(key=="ID_ANALYSIS_QUICKBLOCKS")				return ID_ANALYSIS_QUICKBLOCKS;
		else if(key=="ID_START_GVT")						return ID_START_GVT;
		else if(key=="ID_ANALYSIS_MIXTURE")					return ID_ANALYSIS_MIXTURE;
		else if(key=="ID_ANALYSIS_MIXTURE_CONTAINMENT")		return ID_ANALYSIS_MIXTURE_CONTAINMENT;
		else if(key=="ID_CONSISTENCY_GUARD_BUTTON")			return ID_CONSISTENCY_GUARD_BUTTON;
		else if(key=="ID_EXPORT_MIXTURE")					return ID_EXPORT_MIXTURE;
		else if(key=="ID_EXPORT_MIXTURE_CONTAINMENT")		return ID_EXPORT_MIXTURE_CONTAINMENT;
		else if(key=="ID_SETTINGS_GVTSETTINGS")				return ID_SETTINGS_GVTSETTINGS;
		else if(key=="ID_SETTINGS_BORROWLICENSE")			return ID_SETTINGS_BORROWLICENSE;
		else if(key=="ID_EXPORT_SVS")						return ID_EXPORT_SVS;
		else if(key=="ID_INVALIDATERESULTS_ALL")			return ID_INVALIDATERESULTS_ALL;
		else if(key=="ID_INVALIDATERESULTS_LINEAR")			return ID_INVALIDATERESULTS_LINEAR;
		else if(key=="ID_INVALIDATERESULTS_NONLINEAR")		return ID_INVALIDATERESULTS_NONLINEAR;
		else if(key=="ID_INVALIDATERESULTS_HEATFLOW")		return ID_INVALIDATERESULTS_HEATFLOW;
		else if(key=="ID_INVALIDATERESULTS_MIXTURE")		return ID_INVALIDATERESULTS_MIXTURE;
		else if(key=="ID_INVALIDATERESULTS_MIXTURE_CONTAINMENT")	return ID_INVALIDATERESULTS_MIXTURE_CONTAINMENT;
		else if(key=="ID_SETTINGS_FAULTINITIALIZATIONSETTINGS")		return ID_SETTINGS_FAULTINITIALIZATIONSETTINGS;
		else if(key=="ID_SETTINGS_OPTIMIZATIONSETTINGS")	return ID_SETTINGS_OPTIMIZATIONSETTINGS;
		else if(key=="ID_NOVALUEDRAW")						return ID_NOVALUEDRAW;
		else if(key=="ID_DRAWVALUE")						return ID_DRAWVALUE;
		else if(key=="ID_FILE_MODELSUMMARY")				return ID_FILE_MODELSUMMARY;
		else if(key=="ID_FILE_SAVEMODELWITHAVERAGEDRESULTSONLY")	return ID_FILE_SAVEMODELWITHAVERAGEDRESULTSONLY;
		else if(key=="ID_VIEW_STICKTOVIEW")					return ID_VIEW_STICKTOVIEW;
		else if(key=="ID_EXPORTCOMMANDFILESWITHDEFAULTS")	return ID_EXPORTCOMMANDFILESWITHDEFAULTS;
		else if(key=="ID_EXECUTE_SVS")						return ID_EXECUTE_SVS;
		else if(key=="ID_EXPORT_LINE_DATA")					return ID_EXPORT_LINE_DATA;
		else if(key=="ID_VIEW_CONVEX_HULL")					return ID_VIEW_CONVEX_HULL;
		else if(key=="ID_PLUGINS_ADD")						return ID_PLUGINS_ADD;
		//
		// command not found
		//
		else												return 0;
		//
		// FIXME: defined differetly, fix it ...
		//
		//ID_ON_RUN_MODEL
		//ID_ON_RUN_STEP
		//ID_ON_WRONG_PYTHONROOT

		//
		// 2) other classes that receive MFC commads
		//
		//	--CutSurfaceFrame--
		//	--modeltimesteppg--
		//	--AttriHotSpotDlg--
		//	--attricolorgradientdlg--
		//	--Geomec--
		//	--MainFrm--
		//	--DataTreeView--
		//	--ModelTreeView--
		//	--ModelView--
		//
	}


} // namespace  gm