# Folders containing DIAG commands
DIAG_CODE_DIR_TEL = ../diag ../NVM_Proxy ../errorhandler 
DIAG_CODE_DIR_EEH = ../diag ../errorhandler 
#DIAG_CODE_DIR_AUD = ../diag ../aud_sw
DIAG_CODE_DIR_AUD = ../diag ../linux_audio
DIAG_CODE_DIR_ATCMD = ../diag ../linux_telephony/atcmdsrv

# IDs should be align to multiple of 8 for filter arrays
FIR_CMD_TEL = 0
FIR_CMD_AUD = 504
#FIR_CMD_EEH = 1008
FIR_CMD_ATCMD = 1008

FIR_REP_TEL = 0
FIR_REP_AUD = 504
#FIR_REP_EEH = 1008
FIR_REP_ATCMD = 1008

# Text MDB for Linux
DIAG_MDB_BN  = Boerne_DIAG.mdb.txt
DIAG_MDB_TEL = DIAG_tel.txt
DIAG_MDB_AUD = DIAG_aud.txt
DIAG_MDB_EEH = DIAG_eeh.txt
DIAG_MDB_ATCMD = DIAG_atcmd.txt

INT_LIST_TEL := $(shell find $(DIAG_CODE_DIR_TEL) -name "*.i")
INT_LIST_AUD := $(shell find $(DIAG_CODE_DIR_AUD) -name "*.i")
INT_LIST_EEH := $(shell find $(DIAG_CODE_DIR_EEH) -name "*.i")
INT_LIST_ATCMD := $(shell find $(DIAG_CODE_DIR_ATCMD) -name "*.i")

DIAGDB_C_TEL = diagDBtel.c
DIAGDB_H_TEL = diagDBtel.h
DIAGDB_C_AUD = diagDBaud.c
DIAGDB_H_AUD = diagDBaud.h
DIAGDB_C_EEH = diagDBeeh.c
DIAGDB_H_EEH = diagDBeeh.h
DIAGDB_C_ATCMD = diagDBatcmd.c
DIAGDB_H_ATCMD = diagDBatcmd.h

DIAGDB_DIR = ../diag/diag_al
DIAGDB_AUD_DIR = ../linux_audio
DIAGDB_ATCMD_DIR = ../linux_telephony/atcmdsrv/src

.PHONY: all merge clean

all: clean compile_tel compile_aud compile_atcmd merge

merge: $(DIAG_MDB_BN)

compile_tel: $(DIAGDB_C_TEL) $(DIAGDB_H_TEL)

compile_aud: $(DIAGDB_C_AUD) $(DIAGDB_H_AUD)

compile_eeh: $(DIAGDB_C_EEH) $(DIAGDB_H_EEH)

compile_atcmd : $(DIAGDB_C_ATCMD) $(DIAGDB_H_ATCMD)

$(DIAG_MDB_BN): $(DIAG_MDB_TEL)
	cp $(DIAG_MDB_TEL) $(DIAG_MDB_BN)
	./PPDBMerge.exe $(DIAG_MDB_BN) $(DIAG_MDB_AUD) $(DIAG_MDB_ATCMD)
	cp $(DIAG_MDB_BN) ..

$(DIAGDB_C_TEL) $(DIAGDB_H_TEL): $(INT_LIST_TEL)
	find $(DIAG_CODE_DIR_TEL) -name "*.i" | sort > i_tel.list	
	./PPC.exe @i_tel.list
	find $(DIAG_CODE_DIR_TEL) -name "*.pp" > pp_tel.list
	./PPL.exe $(DIAG_MDB_TEL) $(DIAGDB_C_TEL) @pp_tel.list -c:$(FIR_CMD_TEL) -r:$(FIR_REP_TEL)
#	chmod +w $(DIAGDB_DIR)/$(DIAGDB_C_TEL) $(DIAGDB_DIR)/$(DIAGDB_H_TEL)
	cp $(DIAGDB_C_TEL) $(DIAGDB_H_TEL) $(DIAGDB_DIR)

$(DIAGDB_C_AUD) $(DIAGDB_H_AUD): $(INT_LIST_AUD)
	find $(DIAG_CODE_DIR_AUD) -name *.i | sort > i_aud.list
	./PPC.exe @i_aud.list
	find $(DIAG_CODE_DIR_AUD) -name *.pp  > pp_aud.list
	./PPL.exe $(DIAG_MDB_AUD) $(DIAGDB_C_AUD) @pp_aud.list -c:$(FIR_CMD_AUD) -r:$(FIR_REP_AUD)
#	chmod +w $(DIAGDB_DIR)/$(DIAGDB_C_AUD) $(DIAGDB_DIR)/$(DIAGDB_H_AUD)
	cp $(DIAGDB_C_AUD) $(DIAGDB_H_AUD) $(DIAGDB_AUD_DIR)

$(DIAGDB_C_EEH) $(DIAGDB_H_EEH): $(INT_LIST_EEH)
	find $(DIAG_CODE_DIR_EEH) -name *.i | sort> i_eeh.list
	./PPC.exe @i_eeh.list
	find $(DIAG_CODE_DIR_EEH) -name *.pp  > pp_eeh.list
	./PPL.exe $(DIAG_MDB_EEH) $(DIAGDB_C_EEH) @pp_eeh.list -c:$(FIR_CMD_EEH) -r:$(FIR_REP_EEH)
#	chmod +w $(DIAGDB_DIR)/$(DIAGDB_C_EEH) $(DIAGDB_DIR)/$(DIAGDB_H_EEH)
	cp $(DIAGDB_C_EEH) $(DIAGDB_H_EEH) $(DIAGDB_DIR)

$(DIAGDB_C_ATCMD) $(DIAGDB_H_ATCMD): $(INT_LIST_ATCMD)
	find $(DIAG_CODE_DIR_ATCMD) -name *.i | sort > i_atcmd.list
	./PPC.exe @i_atcmd.list
	find $(DIAG_CODE_DIR_ATCMD) -name *.pp  > pp_atcmd.list
	./PPL.exe $(DIAG_MDB_ATCMD) $(DIAGDB_C_ATCMD) @pp_atcmd.list -c:$(FIR_CMD_ATCMD) -r:$(FIR_REP_ATCMD)
#	chmod +w $(DIAGDB_DIR)/$(DIAGDB_C_AUD) $(DIAGDB_DIR)/$(DIAGDB_H_AUD)
	cp $(DIAGDB_C_ATCMD) $(DIAGDB_H_ATCMD) $(DIAGDB_ATCMD_DIR)

clean:
	-rm -f Prepass-Merge.log $(DIAG_MDB_BN) \
		$(DIAG_MDB_TEL) $(DIAG_MDB_AUD) $(DIAG_MDB_EEH) \
		$(DIAGDB_C_TEL) $(DIAGDB_H_TEL) \
		$(DIAGDB_C_AUD) $(DIAGDB_H_AUD) \
		$(DIAGDB_C_EEH) $(DIAGDB_H_EEH) \
		$(DIAGDB_C_ATCMD) $(DIAGDB_H_ATCMD) \
		$(DIAGDB_DIR)/$(DIAGDB_C_TEL) $(DIAGDB_DIR)/$(DIAGDB_H_TEL) \
		*.list

