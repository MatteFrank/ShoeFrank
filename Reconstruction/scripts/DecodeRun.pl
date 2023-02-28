#!/usr/bin/perl
use Getopt::Long;

&GetOptions('run=s',\$run,'files=s',\$files);

$myRun = 0;
if($run) { $myRun = $run; }

$myFiles = 1;
if($files) { $myFiles = $files; }


$stringE = '/lustre/nyx/bio/gtraini/FOOT/NewGeom/Ale/shoe/build/Reconstruction/level0';
$ifile   = '/lustre/nyx/bio/gtraini/FOOT/NewGeom/Ale/shoe/build/Reconstruction/level0/SubmitRunDecoding.sh';
$stringO = '/lustre/nyx/bio/asarti/FOOT/Processing';

$dirO = ${stringO}.'/Run_'.${myRun};
print "Output dir:: ".${dirO}." \n";

if(!(-e "$dirO")) {
    print "Creating $dirO output dir\n";
    system("mkdir ${dirO}");
}

print "Dir created\n";

open(INF,$ifile);
open OUTF,">${dirO}/RunDecoding_R${myRun}_F${myFiles}.sh";

while (<INF>) {
    chomp($_);
    #print "string:: $_\n";
    if ($_ =~ /Processing_RUN/) {
	print OUTF "#SBATCH -J Processing_${myRun}\n";
    } elsif ($_ =~ /OUTLOG/) {
	print OUTF "#SBATCH -o ${dirO}/OutputR${myRun}F${myFiles}_%j_%N.log\n";
    } elsif ($_ =~ /ERRLOG/) {
	print OUTF "#SBATCH -e ${dirO}/ErrorR${myRun}F${myFiles}_%j_%N.log\n";
    } elsif ($_ =~ /MAXNUM/) {
	print OUTF "#SBATCH --array=1-${myFiles}\n";
    } elsif ($_ =~ /processSingleRun/) {
	#data_test.00002034.physics_foot.daq.RAW._lb0000._EB-RCD._0003.data
	$runStr = sprintf("%08d",${myRun});
	$outFile = "${dirO}/physics_foot.daq.RAW._lb0000._EB-RCD_R${myRun}";
	print OUTF "./processSingleRun ${runStr} ${outFile}\n";
    } else {
	print OUTF "$_\n";
    }
}

close OUTF;

system("sbatch ${dirO}/RunDecoding_R${myRun}_F${myFiles}.sh");
