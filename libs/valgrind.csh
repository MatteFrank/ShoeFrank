#!/bin/tcsh

# Exemaple of valgrind
valgrind --leak-check=full --dsymutil=yes --suppressions=$ROOTSYS/etc/valgrind-root.supp --log-file=valgrind.log --error-limit=no EXECUATBLE
