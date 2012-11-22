#include <stdio.h>
#include <string.h>
#include "glift.h"

Glift_Taint_t * Glift_NodeGenerator( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj)
{
	Glift_Taint_t * temp;
	temp = NULL;
	if ( !Abc_NtkIsSopNetlist(pNtk) )
	{
		printf("Glift cannot generated on non-Sop netlists.\n");
		return NULL;
	}
	if ( Abc_ObjIsNode(pObj) )
	{

		// Determine what gate is pObj
		if (!strcmp((char *)pObj->pData, "11 1\n"))		// 2-INPUT AND Gate
		{
			temp = Glift_AddAndGlift(pNtk, pObj);
		}
		else if(!strcmp((char *)pObj->pData, "00 0\n"))		// 2-INPUT OR Gate
		{
			temp = Glift_AddOrGlift(pNtk, pObj);
		}
		else if(!strcmp((char *)pObj->pData, "00 1\n"))		// 2-INPUT NOR Gate
		{
			temp = Glift_AddNorGlift(pNtk, pObj);
		}
		else if(!strcmp((char *)pObj->pData, "11 0\n"))		// 2-INPUT NAND Gate
		{
			temp = Glift_AddNandGlift(pNtk, pObj);
		}
		else if(!strcmp((char *)pObj->pData, "01 1\n10 1\n"))	// 2-INPUT XOR Gate
		{
			temp = Glift_AddXorGlift(pNtk, pObj);
		}
		else if(!strcmp((char *)pObj->pData, "11 1\n00 1\n"))	// 2-INPUT NXOR Gate
		{
			temp = Glift_AddNxorGlift(pNtk, pObj);
		}
		else if(!strcmp((char *)pObj->pData, "1 1\n"))		// BUFFER
		{
			temp = Glift_AddBufferGlift(pNtk, pObj);
		}else if(!strcmp((char *)pObj->pData, "0 1\n"))		// NOT Gate
		{
			temp = Glift_AddBufferGlift(pNtk, pObj);
		}
	}else if( Abc_ObjIsLatch(pObj) )
	{
		temp = Glift_AddLatchGlift(pNtk, pObj);
	}
		
	return temp;
}

char ** Glift_GetFaninFanoutNames( Abc_Ntk_t *pNtk, Abc_Obj_t * pObj )
{
	int faninSize, fanoutSize;
	int i,j;
	char ** pNames;
	faninSize = Abc_ObjFaninNum( pObj );
	fanoutSize = Abc_ObjFanoutNum( pObj );
	pNames = (char **) malloc(sizeof(char *) * (faninSize + fanoutSize));
	for(i = 0;i < faninSize;i++)
	{
		pNames[i] = Nm_ManFindNameById( pNtk->pManName, Abc_ObjFaninId( pObj, i ));
	}
	for(j = 0;j < fanoutSize;j++)
	{
		pNames[i+j] = Nm_ManFindNameById( pNtk->pManName, pObj->vFanouts.pArray[j]);
	}
	return pNames;
}

char ** Glift_GenerateTaintedNames( char * pNames[], int size )
{
	char ** pTaintNames;
	int i;
	pTaintNames = (char **)malloc(sizeof(char *) * size);
	for(i = 0;i < size;i++)
	{
		pTaintNames[i] = (char *)malloc(strlen(pNames[i])+3);
		memset(pTaintNames[i], 0, strlen(pNames[i]) + 3);
		memcpy(pTaintNames[i], pNames[i], strlen(pNames[i]));
		strcat(pTaintNames[i], "_t\0");
	}
	return pTaintNames;
}

char ** Glift_GenerateNegativeNames( char * pNames[], int size )
{
	char ** pNegNames;
	int i;
	pNegNames = (char **)malloc(sizeof(char *) * size);
	for(i = 0;i < size;i++)
	{
		pNegNames[i] = (char *)malloc(strlen(pNames[i])+3);
		memset(pNegNames[i], 0, strlen(pNames[i]) + 3);
		memcpy(pNegNames[i], pNames[i], strlen(pNames[i]));
		strcat(pNegNames[i], "_n\0");
	}
	return pNegNames;
}

char * Glift_GenerateFanoutNamesFromFanin( char * pFaninNames[], int size )
{
	int fanoutNameSize = 0;
	int i;
	char * fanoutNames;
	for( i = 0;i < size;i++ )
	{
		fanoutNameSize += strlen( pFaninNames[i] );
	}
	fanoutNames = (char *) malloc( fanoutNameSize + 1 );
	memset(fanoutNames, 0, fanoutNameSize + 1);
	for( i = 0;i < size;i++)
	{
		strcat( fanoutNames, pFaninNames[i] );
	}
	strcat(fanoutNames, "\0");
	return fanoutNames;
}

char ** Glift_CombineInputNames( char * inputName1, char * inputName2 )
{
	char ** inputNames;
	inputNames = (char **) malloc(sizeof(char *)*2);
	inputNames[0] = inputName1;
	inputNames[1] = inputName2;
	return inputNames;
}

Abc_Obj_t * Glift_CreateNode( Abc_Ntk_t * pNtk, char * pNameOut, char * pNameIn[], int nInputs )
{
	Abc_Obj_t * pNet, * pNode;
	int i;
	pNode = Abc_NtkCreateNode( pNtk );
	for ( i = 0; i < nInputs; i++ )
	{
		pNet = Abc_NtkFindOrCreateNet( pNtk, pNameIn[i] );
		Abc_ObjAddFanin(pNode, pNet);
	}
	pNet = Abc_NtkFindOrCreateNet( pNtk, pNameOut );
	Abc_ObjAddFanin( pNet, pNode );
	return pNode;
}

Glift_Taint_t * Glift_AddAndGlift ( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj )
{
	Glift_Taint_t * temp;
	char ** pNames, ** pTaintNames;
	temp = (Glift_Taint_t *) malloc( sizeof(Glift_Taint_t) );
	temp->vFaninTaint = temp->vFanoutTaint = NULL;
	// Get the net names
	pNames = Glift_GetFaninFanoutNames( pNtk, pObj );
	// Create new net names for glift network
	pTaintNames = Glift_GenerateTaintedNames( pNames, 3 );
	// Generate nodes and nets used in glift network
	Abc_Obj_t * pNode;
	char * pNameTemp[3];
	// The Construction Process
	// First AND Gate ( A & Bt )
	pNameTemp[0] = Glift_GenerateFanoutNamesFromFanin( Glift_CombineInputNames(pNames[0], pTaintNames[1]), 2 );
	pNode = Glift_CreateNode( pNtk, pNameTemp[0], Glift_CombineInputNames(pNames[0], pTaintNames[1]), 2 );
	
	Abc_ObjSetData( pNode, Abc_SopCreateAnd((Mem_Flex_t *)pNtk->pManFunc, 2, NULL) );
	//Abc_ObjSetData( pNode, "11 1\n" );

	// Second AND Gate ( B & At )
	pNameTemp[1] = Glift_GenerateFanoutNamesFromFanin( Glift_CombineInputNames(pNames[1], pTaintNames[0]), 2 );
	pNode = Glift_CreateNode( pNtk, pNameTemp[1], Glift_CombineInputNames(pNames[1], pTaintNames[0]), 2 );

	Abc_ObjSetData( pNode, Abc_SopCreateAnd((Mem_Flex_t *)pNtk->pManFunc, 2, NULL) );
	//Abc_ObjSetData( pNode, "11 1\n" );

	// First OR Gate ( A & Bt + B & At )
	pNameTemp[2] = Glift_GenerateFanoutNamesFromFanin( pNameTemp, 2 );
	pNode = Glift_CreateNode( pNtk, pNameTemp[2], pNameTemp, 2);

	Abc_ObjSetData( pNode, Abc_SopCreateOr((Mem_Flex_t *)pNtk->pManFunc, 2, NULL) );
	//Abc_ObjSetData( pNode, "00 0\n" );

	// Third AND Gate ( At & Bt )
	pNameTemp[1] = Glift_GenerateFanoutNamesFromFanin( pTaintNames, 2 );
	pNode = Glift_CreateNode( pNtk, pNameTemp[1], pTaintNames, 2 );

	Abc_ObjSetData( pNode, Abc_SopCreateAnd((Mem_Flex_t *)pNtk->pManFunc, 2, NULL) );
	//Abc_ObjSetData( pNode, "11 1\n" );

	temp->vFaninTaint = Abc_ObjFaninVec( pNode );
	// Last OR Gate ( First Or gate with third and gate )
	pNode = Glift_CreateNode( pNtk, pTaintNames[2], &pNameTemp[1], 2 );

	Abc_ObjSetData( pNode, Abc_SopCreateOr((Mem_Flex_t *)pNtk->pManFunc, 2, NULL) );
	//Abc_ObjSetData( pNode, "00 0\n" );

	temp->vFanoutTaint = Abc_ObjFanoutVec( pNode );
	// Construct the return value indicate the tainted output and tainted input
	return temp;
}

Glift_Taint_t * Glift_AddOrGlift( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj )
{
	Glift_Taint_t * temp;
	char ** pNames, ** pTaintNames;
	Abc_Obj_t * pNode;
	temp = (Glift_Taint_t *) malloc( sizeof(Glift_Taint_t) );
	temp->vFaninTaint = temp->vFanoutTaint = NULL;
	// Get the net names
	pNames = Glift_GetFaninFanoutNames( pNtk, pObj );
	// Create new net names for glift network
	pTaintNames = Glift_GenerateTaintedNames( pNames, 3 );
	// First we should generate negative net of inputs
	char ** pNegNames = Glift_GenerateNegativeNames( pNames, 2 );
	pNode = Glift_CreateNode( pNtk, pNegNames[0], pNames, 1);
	Abc_ObjSetData( pNode, Abc_SopCreateInv((Mem_Flex_t *)pNtk->pManFunc) );
	pNode = Glift_CreateNode( pNtk, pNegNames[1], &pNames[1], 1);
	Abc_ObjSetData( pNode, Abc_SopCreateInv((Mem_Flex_t *)pNtk->pManFunc) );
	// Generate nodes and nets used in glift network
	char * pNameTemp[3];
	// The Construction Process
	// First AND Gate ( A & Bt )
	pNameTemp[0] = Glift_GenerateFanoutNamesFromFanin( Glift_CombineInputNames(pNegNames[0], pTaintNames[1]), 2 );
	pNode = Glift_CreateNode( pNtk, pNameTemp[0], Glift_CombineInputNames(pNegNames[0], pTaintNames[1]), 2 );
	
	Abc_ObjSetData( pNode, Abc_SopCreateAnd((Mem_Flex_t *)pNtk->pManFunc, 2, NULL) );
	//Abc_ObjSetData( pNode, "11 1\n" );

	// Second AND Gate ( B & At )
	pNameTemp[1] = Glift_GenerateFanoutNamesFromFanin( Glift_CombineInputNames(pNegNames[1], pTaintNames[0]), 2 );
	pNode = Glift_CreateNode( pNtk, pNameTemp[1], Glift_CombineInputNames(pNegNames[1], pTaintNames[0]), 2 );

	Abc_ObjSetData( pNode, Abc_SopCreateAnd((Mem_Flex_t *)pNtk->pManFunc, 2, NULL) );
	//Abc_ObjSetData( pNode, "11 1\n" );

	// First OR Gate ( A & Bt + B & At )
	pNameTemp[2] = Glift_GenerateFanoutNamesFromFanin( pNameTemp, 2 );
	pNode = Glift_CreateNode( pNtk, pNameTemp[2], pNameTemp, 2);

	Abc_ObjSetData( pNode, Abc_SopCreateOr((Mem_Flex_t *)pNtk->pManFunc, 2, NULL) );
	//Abc_ObjSetData( pNode, "00 0\n" );

	// Third AND Gate ( At & Bt )
	pNameTemp[1] = Glift_GenerateFanoutNamesFromFanin( pTaintNames, 2 );
	pNode = Glift_CreateNode( pNtk, pNameTemp[1], pTaintNames, 2 );

	Abc_ObjSetData( pNode, Abc_SopCreateAnd((Mem_Flex_t *)pNtk->pManFunc, 2, NULL) );
	//Abc_ObjSetData( pNode, "11 1\n" );

	temp->vFaninTaint = Abc_ObjFaninVec( pNode );
	// Last OR Gate ( First Or gate with third and gate )
	pNode = Glift_CreateNode( pNtk, pTaintNames[2], &pNameTemp[1], 2 );

	Abc_ObjSetData( pNode, Abc_SopCreateOr((Mem_Flex_t *)pNtk->pManFunc, 2, NULL) );
	//Abc_ObjSetData( pNode, "00 0\n" );

	temp->vFanoutTaint = Abc_ObjFanoutVec( pNode );
	// Construct the return value indicate the tainted output and tainted input
	return temp;
	
}

Glift_Taint_t * Glift_AddXorGlift( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj )
{
	Glift_Taint_t * temp;
	char ** pNames, ** pTaintNames;
	temp = (Glift_Taint_t *) malloc( sizeof(Glift_Taint_t) );
	temp->vFaninTaint = temp->vFanoutTaint = NULL;
	// Get the net names
	pNames = Glift_GetFaninFanoutNames( pNtk, pObj );
	// Create new net names for glift network
	pTaintNames = Glift_GenerateTaintedNames( pNames, 3 );
	// Generate nodes and nets used in glift network
	Abc_Obj_t * pNode;
	// Construct Process
	// Ct = At | Bt
	pNode = Glift_CreateNode( pNtk, pTaintNames[2], pTaintNames, 2 );
	Abc_ObjSetData( pNode, Abc_SopCreateOr((Mem_Flex_t *)pNtk->pManFunc, 2, NULL) );

	temp->vFaninTaint = Abc_ObjFaninVec( pNode );
	temp->vFanoutTaint = Abc_ObjFanoutVec( pNode );

	return temp;
}

Glift_Taint_t * Glift_AddBufferGlift( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj )
{
	Glift_Taint_t * temp;
	char ** pNames, ** pTaintNames;
	temp = (Glift_Taint_t *)malloc( sizeof(Glift_Taint_t) );
	temp->vFanoutTaint = temp->vFaninTaint = NULL;
	// Get the names
	pNames = Glift_GetFaninFanoutNames( pNtk, pObj );
	// Create new net names  for glift network
	pTaintNames = Glift_GenerateTaintedNames( pNames, 2 );
	// Generate nodes and nets used in the glift network
	Abc_Obj_t * pNode;
	// Construct Process
	// Bt = At
	pNode = Glift_CreateNode( pNtk, pTaintNames[1], pTaintNames, 1 );
	Abc_ObjSetData( pNode, Abc_SopCreateBuf((Mem_Flex_t *)pNtk->pManFunc) );

	temp->vFaninTaint = Abc_ObjFaninVec( pNode );
	temp->vFanoutTaint = Abc_ObjFanoutVec( pNode );

	return temp;
}

Glift_Taint_t * Glift_AddLatchGlift( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj )
{
	Glift_Taint_t * temp;
	char ** pNames, ** pTaintNames;
	temp = (Glift_Taint_t *)malloc( sizeof(Glift_Taint_t) );
	temp->vFaninTaint = temp->vFanoutTaint = NULL;
	pNames = (char **)malloc( sizeof(char *) * 2);

	// Get the Names
	pNames[0] = Abc_ObjName(Abc_ObjFanin0(Abc_ObjFanin0(pObj)));
	pNames[1] = Abc_ObjName(Abc_ObjFanout0(Abc_ObjFanout0(pObj)));
	// Generate Tainted Names
	pTaintNames = Glift_GenerateTaintedNames( pNames, 2 );
	// Construct Process
	Abc_Obj_t * pNode;
	pNode = Io_ReadCreateLatch( pNtk, pTaintNames[0], pTaintNames[1] );
	
	temp->vFaninTaint = Abc_ObjFaninVec( Abc_ObjFanin0( pNode ) );
	temp->vFanoutTaint = Abc_ObjFanoutVec( Abc_ObjFanout0( pNode ) );
	
	return temp;
}
