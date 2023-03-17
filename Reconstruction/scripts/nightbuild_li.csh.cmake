#! /bin/tcsh
#define sbgli server env

setenv FOOTREC /iphc-work/desis/FOOT/build//Reconstruction/
cd $FOOTREC

echo "sending mail"
cat nightbuild.new | mail -s "shoe night build" cfinck@iphc.cnrs.fr
