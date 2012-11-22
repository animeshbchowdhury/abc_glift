void Abc_GliftGenerate(Abc_Ntk_t * pNtk, Abc_Obj_t * pNode)
{
	if pNode is a and gate
		generate and glift for and gate(pNtk, pNode);
	else if pNode is a or gate
		...
	else if pNode is a nand gate
	else if pNode is a inv gate
	...
}
