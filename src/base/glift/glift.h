/**CFile***************************************************************
 *
 * FileName	[glift.h]
 *
 * SystemName	[]
 *
 * Packagename	[]
 *
 * Synopsis	[]
 *
 * Author	[]
 *
 * Affiliation	[]
 *
 * Date		[]
 *
 * Revision	[]
 *
 * ********************************************************************/

#ifndef ABC__base__glift__glift_h
#define ABC__base__glift__glift_h

///////////////////////////////////////////////////////////////////////
///                           INCLUDES                              ///
///////////////////////////////////////////////////////////////////////

#include "base/abc/abc.h"
#include "base/io/ioAbc.h"
#include "misc/vec/vec.h"
#include "misc/nm/nm.h"
typedef struct Glift_Taint_ Glift_Taint_t;
struct Glift_Taint_ {
	Vec_Int_t * vFaninTaint;
	Vec_Int_t * vFanoutTaint;
};

// gliftIo.c
Abc_Ntk_t * Glift_IoReadNetlist ( char * pFileName, int fcheck );
void Glift_IoWrite( Abc_Ntk_t * pNtk, char * pFileName );

// gliftGenerator.c
Glift_Taint_t * Glift_NodeGenerator( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj );
char ** Glift_GetFaninFanoutNames( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj );
char ** Glift_GenerateTaintedNames( char * pNames[], int size );
char * Glift_GenerateFanoutnamesFromFanin( char * pFaninNames[], int size );
char ** Glift_CombineInputNames( char * inputName1, char * inputName2 );
Abc_Obj_t * Glift_CreateNode( Abc_Ntk_t * pNtk, char * pNameOut, char * pNameIn[], int nInputs );
Glift_Taint_t * Glift_AddAndGlift( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj ); 
Glift_Taint_t * Glift_AddOrGlift( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj ); 
Glift_Taint_t * Glift_AddXorGlift( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj ); 
Glift_Taint_t * Glift_AddBufferGlift( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj ); 
Glift_Taint_t * Glift_AddLatchGlift( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj );
// gliftWriteBench.c
int Glift_WriteBench( Abc_Ntk_t * pNtk, const char * pFileName );
// gliftNetwork.c
void Glift_AddGliftForAllNode( Abc_Ntk_t * pNtk );
void Glift_AddGliftForIsolateNtk( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj );
void Glift_AddGliftForNodeList( Abc_Ntk_t * pNtk, Vec_Ptr_t * vObjs );
Vec_Ptr_t * Glift_ObjFindConnection( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj );
// variables
static inline Glift_Taint_t * Glift_AddNandGlift( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj )	 { return Glift_AddAndGlift(pNtk, pObj);	}
static inline Glift_Taint_t * Glift_AddNorGlift( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj )	 { return Glift_AddOrGlift(pNtk, pObj);		}
static inline Glift_Taint_t * Glift_AddNxorGlift( Abc_Ntk_t * pNtk, Abc_Obj_t * pObj )	 { return Glift_AddXorGlift(pNtk, pObj);	}
#endif
