/** @file DutEfuse.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#define BIT_IN_A_BYTE			(8)

#define EFU_REG_BASE			(0x90000400)
 
#define EFU_DATA_RD				(EFU_REG_BASE)

#define EFU_PGM_SIG				(EFU_REG_BASE+0x204)
#define EFU_BLOCK_BOTH			(3)
typedef struct	EfuseReg_ProgSingals
{
	DWORD PrgSgn_Pgm:1;
	DWORD PrgSgn_Sclk:1;
	DWORD PrgSgn_CsBlk:2;
//	DWORD PrgSgn_CsBlk0:1;
//	DWORD PrgSgn_CsBlk1:1;
	DWORD PrgSgn_WrProtBlk:2;
//	DWORD PrgSgn_WrProtBlk0:1;
//	DWORD PrgSgn_WrProtBlk1:1;
	DWORD PrgSgn_EfusRstBlk:2;
//	DWORD PrgSgn_EfusRstBlk0:1;
//	DWORD PrgSgn_EfusRstBlk1:1;
	DWORD PrgSgn_Reserved:24;
}EfuseReg_ProgSingals;

#define EFU_WR_PROT						(EFU_REG_BASE+0x208)
#define EFUVALUE_WR_PROT_WREN			(0xAC)
#define EFUVALUE_WR_PROT_PROT			(0x0)
 

int DutBtApiClssHeader SetEfuseData(int Block, int LenInWord, WORD *pData)
{
	int status=0;

#ifdef _W8689_
	int i=0, j=0, cntBits=0;
	char *pDataBits=NULL, *pBitsArray=NULL;
	DWORD PgmSgns=0;
	EfuseReg_ProgSingals *pPgmSngals=NULL;

//	WriteMemDword(0x90000000, 1);  
//	WriteMemDword(0x90000004, 0);  

// unprotect
	WriteMemDword(EFU_WR_PROT, EFUVALUE_WR_PROT_WREN);  

	Block++;
//	ReadMemDword(EFU_PGM_SIG, &PgmSgns); // 5D
	PgmSgns=0xFC;
	WriteMemDword(EFU_PGM_SIG, PgmSgns);  
	pPgmSngals = (EfuseReg_ProgSingals*)&PgmSgns;

	cntBits = BIT_IN_A_BYTE*sizeof(WORD)*LenInWord;
	pDataBits = (char*)malloc(cntBits);
	memset(pDataBits, 0, cntBits);
	pBitsArray = pDataBits;


 	pPgmSngals->PrgSgn_Sclk=1;
 	WriteMemDword(EFU_PGM_SIG, PgmSgns); //0xFE); // 5D
	pPgmSngals->PrgSgn_EfusRstBlk=	((~Block)&EFU_BLOCK_BOTH);
 	WriteMemDword(EFU_PGM_SIG, PgmSgns); //0xBE); // 5D


	pPgmSngals->PrgSgn_Pgm=1;
	pPgmSngals->PrgSgn_Sclk=0;
	pPgmSngals->PrgSgn_CsBlk=EFU_BLOCK_BOTH;
	pPgmSngals->PrgSgn_WrProtBlk =	((~Block)&EFU_BLOCK_BOTH);
	pPgmSngals->PrgSgn_EfusRstBlk=	((~Block)&EFU_BLOCK_BOTH);

	WriteMemDword(EFU_PGM_SIG, PgmSgns); //0xAD); // 5D

	pPgmSngals->PrgSgn_CsBlk=((~Block)&EFU_BLOCK_BOTH);
	WriteMemDword(EFU_PGM_SIG, PgmSgns); //0xA9); // 55

	pPgmSngals->PrgSgn_Pgm=0;
	WriteMemDword(EFU_PGM_SIG, PgmSgns); //0xA8); // 54

	for (i=0; i<LenInWord; i++)
	{
		for (j=0; j<BIT_IN_A_BYTE*sizeof(WORD); j++)
		{	 
			*pDataBits = (pData[i]>>j)&1;
			pDataBits++;
		}
	}

	pDataBits = pBitsArray;

 
	for (i=0; i<cntBits; i++)
	{
//		if(pDataBits[i] == 1)
		{
 			pPgmSngals->PrgSgn_Pgm=pDataBits[i];  //scl=0, pgm=data
  			pPgmSngals->PrgSgn_Sclk=0; //scl=1, pgm=data
			WriteMemDword(EFU_PGM_SIG, PgmSgns); //0xA9); // 55
  			pPgmSngals->PrgSgn_Sclk=1; //scl=1, pgm=data
			WriteMemDword(EFU_PGM_SIG, PgmSgns); //0xAB);	// 57
			if(pDataBits[i] == 1)	
				Sleep(1);
  			pPgmSngals->PrgSgn_Sclk=0; //scl=1, pgm=data
//			WriteMemDword(EFU_PGM_SIG, PgmSgns); //0xA8); // 58
 			pPgmSngals->PrgSgn_Pgm=0;  //scl=0, pgm=data
			WriteMemDword(EFU_PGM_SIG, PgmSgns); //0xA8); // 58
		}
//		else
//		{
// 			pPgmSngals->PrgSgn_Pgm=0;	//scl=0, pgm=0
//			WriteMemDword(EFU_PGM_SIG, PgmSgns); //0xA8); // 54
// 			pPgmSngals->PrgSgn_Sclk=1;	//scl=1, pgm=0
//			WriteMemDword(EFU_PGM_SIG, PgmSgns); // 0xAA);	// 56
//		}

	}
	pPgmSngals->PrgSgn_Pgm=0;	//scl=0, pgm=0
	pPgmSngals->PrgSgn_Sclk=0;	//scl=0, pgm=0
	WriteMemDword(EFU_PGM_SIG, PgmSgns); //0xA8);	// 54

 	pPgmSngals->PrgSgn_CsBlk=EFU_BLOCK_BOTH;
	pPgmSngals->PrgSgn_WrProtBlk =	EFU_BLOCK_BOTH;
	pPgmSngals->PrgSgn_EfusRstBlk=	EFU_BLOCK_BOTH;

	WriteMemDword(EFU_PGM_SIG, PgmSgns); //0xFC);

//  write protect
	WriteMemDword(EFU_WR_PROT, EFUVALUE_WR_PROT_PROT);  

#else //#ifdef _W8689_

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_EFUSE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdEfuse_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	TxBuf.BlockNo = Block;
	TxBuf.LenInWord = LenInWord;
	memcpy(TxBuf.Data, pData, LenInWord);

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
#endif //#ifdef _W8689_
 
	return status;

}



int DutBtApiClssHeader GetEfuseData(int Block, int LenInWord, WORD *pData)
{
	int status=0;
#ifdef _W8689_
	int i=0;
	DWORD PgmSgns=0;
	EfuseReg_ProgSingals *pPgmSngals=NULL;
	DWORD RdBk=0;

//	WriteMemDword(EFU_WR_PROT, EFUVALUE_WR_PROT_WREN);  

//	Block++;
	ReadMemDword(EFU_PGM_SIG, &PgmSgns); // 5D
	pPgmSngals = (EfuseReg_ProgSingals*)&PgmSgns;

 	pPgmSngals->PrgSgn_EfusRstBlk=	((~EFU_BLOCK_BOTH)&EFU_BLOCK_BOTH);
	WriteMemDword(EFU_PGM_SIG, PgmSgns);  
		Sleep(1);
 	pPgmSngals->PrgSgn_EfusRstBlk=	(EFU_BLOCK_BOTH);
	WriteMemDword(EFU_PGM_SIG, PgmSgns);  
		Sleep(1);

	for (i=0; i<LenInWord; i++)
	{
		ReadMemDword(EFU_DATA_RD+Block*0x40+i*4,	&RdBk);
		pData[i]=RdBk;
	}
	
#else //#ifdef _W8689_

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_EFUSE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_CmdEfuse_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

  	TxBuf.BlockNo = Block;
	TxBuf.LenInWord = LenInWord;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pData)  
		memcpy(pData, TxBuf.Data, LenInWord);

#endif //#ifdef _W8689_

	return status;

}


