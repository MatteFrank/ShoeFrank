#! /bin/tcsh
#define sbgli server env

set file = 'nightbuild.new'
set pattern = 'PatternError'
 
if ( { grep -q  $pattern $file } ) then
  echo "sending mail"
  cat nightbuild.new | mail -s "shoe night build" cfinck@iphc.cnrs.fr
else
  echo "No error found"
endif
\rm  ~/nightbuild.new
