#include <stdio.h>
#include "glift.h"

extern void Abc_Start();
extern void Abc_Stop();

extern void * Abc_FrameGetGlobalFrame();

int main( int argc, char * argv[] )
{
	int i;
	if(argc != 3)
	{
		printf("Please enter the bench filename.\n");
		return 1;
	}
	
	Abc_Ntk_t * pNtk;
	Abc_Obj_t * pPi, * pPo;
	Abc_Start();
	pNtk = Glift_IoReadNetlist( argv[1], 0 );
	printf("Lists of all primary inputs of network.\n");
	Abc_NtkForEachPi( pNtk, pPi, i)
	{
		Abc_Obj_t * pNet = Abc_ObjFanout0(pPi);
		printf("%s ", Nm_ManFindNameById( pNtk->pManName, pNet->Id));

	}
	printf("\nLists of all primary outputs of the network.\n");
	Abc_NtkForEachPo( pNtk, pPo, i )
	{
		printf("%s ", Nm_ManFindNameById( pNtk->pManName, Abc_ObjFaninId0(pPo)));
	}

	char portName[20];
	Abc_Obj_t * pPreNet;
	printf("\nInput port names: ");
	scanf("%s", portName);
	printf("\n");
	printf("you have choosed net %s\n", portName);
	pPreNet = Abc_NtkFindNet( pNtk, portName );
	Glift_AddGliftForIsolateNtk( pNtk, pPreNet );

	Glift_WriteBench( pNtk, argv[2] );
	Abc_Stop();
	return 0;
}
