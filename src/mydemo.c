#include <stdio.h>
extern void	Abc_Start();
extern void 	Abc_Stop();

extern void *	Abc_FrameGetGlobalFrame();

extern Abc_Ntk_t *	Io_Read( char * pFileName, Io_FileType_t FileType, int fCheck );
int main( int argc, char * argv[] )
{
	Abc_Start();
		
	Abc_Stop();
}
