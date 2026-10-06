##############################################################################
################################## makefile ##################################
##############################################################################
#                                                                            #
#   makefile of MultiKnapsackAssignBlock                                     #
#                                                                            #
#   Note that $(SMS++INC) is assumed to include any -I directive             #
#   corresponding to external libraries needed by SMS++, at least to the     #
#   extent in which they are needed by the parts of SMS++ used by            #
#   MultiKnapsackAssignBlock, and that the same holds for $(BKBkINC) and     #
#   BinaryKnapsackBlock.                                                     #
#                                                                            #
#   Input:  $(CC)        = compiler command                                  #
#           $(SW)        = compiler options                                  #
#           $(SMS++INC)  = the -I$( core SMS++ directory )                   #
#           $(SMS++OBJ)  = the libSMS++ library itself                       #
#           $(BKBkINC)   = the -I$( BinaryKnapsackBlock directory )          #
#           $(BKBkH)     = the .h files of BinaryKnapsackBlock               #
#           $(MKABkSDR)  = the directory where the source is                 #
#                                                                            #
#   Output: $(MKABkOBJ)  = the final object(s) / library                     #
#           $(MKABkH)    = the .h files to include                           #
#           $(MKABkINC)  = the -I$( source directory )                       #
#                                                                            #
#                             Antonio Frangioni                              #
#                         Dipartimento di Informatica                        #
#                             Universita' di Pisa                            #
#                                                                            #
##############################################################################

# macros to be exported - - - - - - - - - - - - - - - - - - - - - - - - - - -

MKABkOBJ = $(MKABkSDR)/obj/MultiKnapsackAssignBlock.o

MKABkINC = -I$(MKABkSDR)/include

MKABkH   = $(MKABkSDR)/include/MultiKnapsackAssignBlock.h

# clean - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

clean::
	rm -f $(MKABkOBJ) $(MKABkSDR)/*~

# dependencies: every .o from its .cpp + every recursively included .h- - - -

$(MKABkSDR)/obj/MultiKnapsackAssignBlock.o: \
	$(MKABkSDR)/src/MultiKnapsackAssignBlock.cpp $(MKABkH) $(BKBkH) \
	$(SMS++H) $(SMS++OBJ)
	$(CC) -c $(MKABkSDR)/src/MultiKnapsackAssignBlock.cpp -o $@ \
	$(MKABkINC) $(BKBkINC) $(SMS++INC) $(SW)

########################## End of makefile ###################################
