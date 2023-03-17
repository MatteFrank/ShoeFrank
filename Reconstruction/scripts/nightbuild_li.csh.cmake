#! /bin/tcsh
#define sbgli server env

set file = 'nightbuild.new'
set pattern = 'PatternError'
 
setenv FOOTREC /iphc-work/desis/FOOT/build/Reconstruction/
cd $FOOTREC

if ( { grep -q  $pattern $file } ) then
  echo "sending mail"
  cat nightbuild.new | mail -s "shoe night build" cfinck@iphc.cnrs.fr
else
  echo "No error found"
endif
