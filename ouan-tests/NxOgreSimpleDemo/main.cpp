#include <iostream>

#include "Application.h"

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
#define WIN32_LEAN_AND_MEAN 
#include "windows.h" 
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT) 
#else 
	int main(int argc, char **argv) 
#endif 
	{
		try
		{
			Application app;
			app.initialise();
			app.setup();
			app.update();
		}
		catch ( std::exception& e )
		{
			std::cerr << e.what() << std::endl;
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

#ifdef __cplusplus
}
#endif