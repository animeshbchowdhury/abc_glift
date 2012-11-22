#include "glift.h"
#include "base/io/ioAbc.h"
#include "base/main/main.h"

Abc_Ntk_t *	Glift_IoReadNetlist(char *pFileName, int fCheck)
{
	return Io_ReadNetlist( pFileName, Io_ReadFileType( pFileName ), fCheck );
}

/*
void Glift_IoWrite( Abc_Ntk_t * pNtk, char * pFileName )
{
	Abc_Ntk_t * pNtkTemp, * pNtkCopy;
	Io_FileType_t FileType = Io_ReadFileType( pFileName );
	if ( pNtk == NULL )
	{
		fprintf( stdout, "Empty network.\n" );
		return;
	}

	if ( FileType == IO_FILE_NONE || FileType == IO_FILE_UNKNOWN )
	{
		fprintf( stdout, "The generic file writer requires a known file extension." );
		return;
	}

	if ( FileType == IO_FILE_AIGER || FileType == IO_FILE_BAF )
	{
		fprintf( stdout, "Writing AIGs are not supported in this version.");
		return;
	}

		
}
*/
