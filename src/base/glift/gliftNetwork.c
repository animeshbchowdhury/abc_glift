#include "glift.h"


void Glift_AddGliftForAllNode( Abc_Ntk_t * pNtk ) 			{ Glift_AddGliftForNodeList( pNtk, pNtk->vObjs );}
void Glift_AddGliftForIsolateNtk( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj ) 	{ Glift_AddGliftForNodeList( pNtk, Glift_ObjFindConnection(pNtk, pObj) );}
void Glift_AddGliftForNodeList( Abc_Ntk_t * pNtk, Vec_Ptr_t * vObjs )
{
	Vec_Ptr_t * orig;
	Abc_Obj_t * pObjTemp;
	Glift_Taint_t * temp;
	temp = (Glift_Taint_t *)malloc( sizeof(Glift_Taint_t) );
	temp->vFaninTaint = temp->vFanoutTaint = NULL;
	int i, num;
	orig = Vec_PtrDup( vObjs );
	Vec_PtrForEachEntry( Abc_Obj_t *, orig, pObjTemp, i )
	{
		if( Abc_ObjIsNode(pObjTemp) || Abc_ObjIsLatch(pObjTemp) )
		{	
			temp = Glift_NodeGenerator( pNtk, pObjTemp );
			if( Abc_ObjIsNode(pObjTemp) )
			{
				for(num = 0; num < Abc_ObjFaninNum(pObjTemp); num++)
				{
					if( Abc_ObjIsPi(Abc_ObjFanin0(Abc_ObjFanin(pObjTemp, num))) )
					{
						Abc_Obj_t * pNodeNet;
						pNodeNet = (Abc_Obj_t *)pNtk->vObjs->pArray[temp->vFaninTaint->pArray[num]];
						if( !Abc_ObjFaninNum(pNodeNet) )
						{
							Abc_Obj_t * pNodePi;
							pNodePi = Abc_NtkCreatePi( pNtk );
							Abc_ObjAddFanin( pNodeNet, pNodePi );
						}
					}
				}
				if( Abc_ObjIsPo(Abc_ObjFanout0(Abc_ObjFanout0(pObjTemp))) )
				{
					Abc_Obj_t * pNodePo, * pNodeNet;
					pNodePo = Abc_NtkCreatePo( pNtk );
					pNodeNet = (Abc_Obj_t *)pNtk->vObjs->pArray[temp->vFanoutTaint->pArray[0]];
					Abc_ObjAddFanin( pNodePo, pNodeNet );
				}
			}else
			{
				if( Abc_ObjIsPi(Abc_ObjFanin0(Abc_ObjFanin0(Abc_ObjFanin0(pObjTemp)))) )
				{
					Abc_Obj_t * pNet;
					pNet = (Abc_Obj_t *)pNtk->vObjs->pArray[temp->vFaninTaint->pArray[0]];
					if( !Abc_ObjFaninNum(pNet) )
					{
						Abc_Obj_t * pNodePi;
						pNodePi = Abc_NtkCreatePi( pNtk );
						Abc_ObjAddFanin( pNet, pNodePi );
					}
				}

				if( Abc_ObjIsPo(Abc_ObjFanout0(Abc_ObjFanout0(Abc_ObjFanout0(pObjTemp)))) )
				{
					Abc_Obj_t * pNodePo, * pNet;
					pNodePo = Abc_NtkCreatePo( pNtk );
					pNet = (Abc_Obj_t *)pNtk->vObjs->pArray[temp->vFanoutTaint->pArray[0]];
					Abc_ObjAddFanin( pNodePo, pNet );
				}
				
			}
		}
	}
	Vec_PtrFree( orig );
}

void FindConnection( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj, Vec_Ptr_t * vecCon, int * objVisited)
{
	// check whether this node has been visited
	if(objVisited[Abc_ObjId(pObj)])
	{
		return;
	}
	// this node has not been visited
	objVisited[Abc_ObjId(pObj)] = 1;
	Vec_PtrPush(vecCon, pObj);
	int i;
	for( i = 0; i < Abc_ObjFaninNum(pObj);i++ )
	{
		Abc_Obj_t * pObjFanin;
		pObjFanin = Abc_ObjFanin( pObj, i );
		FindConnection( pNtk, pObjFanin, vecCon, objVisited );
	}
	for( i = 0; i < Abc_ObjFanoutNum(pObj);i++ )
	{
		Abc_Obj_t * pObjFanout;
		pObjFanout = Abc_ObjFanout( pObj, i );
		FindConnection( pNtk, pObjFanout, vecCon, objVisited );
	}
}

Vec_Ptr_t * Glift_ObjFindConnection( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj )
{
	Vec_Ptr_t * vecCon;
	vecCon = Vec_PtrAlloc(5);
	int * objVisited;
	objVisited = (int *)malloc( sizeof(int) * Abc_NtkObjNum(pNtk) );
	memset(objVisited, 0, Abc_NtkObjNum(pNtk) * sizeof(int));
	FindConnection( pNtk, pObj, vecCon, objVisited );
	return vecCon;
}
