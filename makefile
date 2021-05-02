##############################################################################
################################ makefile ####################################
##############################################################################
#                                                                            #
#                              Antonio Frangioni                             #
#                          Operations Research Group                         #
#                         Dipartimento di Informatica                        #
#                             Universita' di Pisa                            #
#                                                                            #
##############################################################################


# macroes to be exported- - - - - - - - - - - - - - - - - - - - - - - - - - -

MKABkOBJ = $(MKABkSDR)MultiKnapsackAssignBlock.o

MKABkINC = -I$(MKABkSDR)

MKABkH   = $(MKABkSDR)MultiKnapsackAssignBlock.h

# clean - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

clean::
	rm -f $(MKABkOBJ) $(MKABkSDR)*~

# dependencies: every .o from its .cpp + every recursively included .h- - - -

$(MKABkSDR)MultiKnapsackAssignBlock.o: $(MKABkSDR)MultiKnapsackAssignBlock.cpp \
	$(MKABkSDR)MultiKnapsackAssignBlock.h $(SMS++OBJ)
	$(CC) -c $*.cpp -o $@ \
	$(MKABkINC) $(SMS++INC) $(SW)

########################## End of makefile ###################################
