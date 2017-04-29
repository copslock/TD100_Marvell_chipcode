#==============================================================================
# Phonies
#==============================================================================
all: clean $(PATH_USR_LOG) $(PATH_USR_BIN) $(PATH_USR_OBJ) banner $(OUTPUT_TARGET)
banner:
	@echo ------------Build Log------------------------- >$(USR_LOG_TRACE)
	@date >>$(USR_LOG_TRACE)
clean:
	-rm -f $(OUTPUT_TARGET)
	-rm -f $(USR_LOG_TRACE)
	-rm -f $(OBJS_C)


#==============================================================================
# Directories
#==============================================================================
$(PATH_USR_LOG):
	-mkdir $(PATH_USR_LOG)
$(PATH_USR_BIN):
	-mkdir $(PATH_USR_BIN)
	
#==============================================================================
# Compile                                                         (user update)
#==============================================================================
$(OBJS_C):%.o:%.c
	-$(CC) -c $< -o $@ $(CFLAGS) $(OPT_INC_EXT) 1>>$(USR_LOG_TRACE) 2>>$(USR_LOG_TRACE)
	@if [ -e $@ ]; then echo [success] C Compile [$<] to [$@] 1>>$(USR_LOG_TRACE); else echo [failed] C Compile [$<] to [$@] 1>>$(USR_LOG_TRACE); fi

#==============================================================================
# link                                                            (user update)
#==============================================================================
$(OUTPUT_TARGET):$(OBJS_C) 
	$(LN) -o $@ $(OBJS_C) $(USR_LIBS) $(LNFLAGS) 1>>$(USR_LOG_TRACE) 2>>$(USR_LOG_TRACE)
	@if [ -e $(OUTPUT_TARGET) ]; then echo [success] Link [$@] 1>>$(USR_LOG_TRACE); else echo [failed] Link [$@] 1>>$(USR_LOG_TRACE); fi

	