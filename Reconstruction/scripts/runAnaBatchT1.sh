#!/bin/bash

# This bash script launches the SHOE executable DecodeGlbAna on processed files using the HTCondor suite on Tier1. It is conceived to run on files produced from data and MC FILES. The script processes a single file launching a number of jobs in condor so that each of them processes ~30k events.

# Before launching this script, a working SHOE installation and a properly set FootGlobal.par file is needed for the campaign you want to process. The SHOE installation must be placed in the "/opt/exp_software/foot/${USER}" folder, where "${USER}" is your username on the tier1 machine.
# IMPORTANT: make sure to compile SHOE using the shared installation of ROOT, which is placed in the "/opt/exp_software/foot/root" folder. A set of basic commands to compile SHOE on the Tier1 is given below in this file.

# To launch this script, issue the command:

# > ./path/to/runAnaBatchT1_MC.sh -i inputFile -o outputFolder -m isMc

# where:
# - inputFile is the path of the input file. The input file is forced to be inside "/storage/gpfs_data/foot". This option needs either the full path of the input directory or the relative one starting from the "/storage/gpfs_data/foot/" folder.
# - outputFile is the path to the output file. This is forced to be in "/storage/gpfs_data/foot/${USER}". If you don't have a directory like the one you requested already in the Tier1, it will be created. This argument is optional, by default the file will be placed in the same directory as the input one with a default name ("MergeAna_{campaign}_{runNumber}.root")
# - isMc is a flag signaling if the file was produced from real ("0") or MC data ("1"). This argument is optional with default value = "0", i.e. real data

# There is no need to indicate the campaign and run number since they are retrieved from the "runinfo" object in the input file

############# IMPORTANT!!!!! #################

# To check the status of your jobs, launch the command

# > condor_q -name sn-02 $USER

# All condor auxiliary files (.err/.out/.log) are automatically downloaded and the completed jobs removed when the file merge is requested. If one turns off the file merging, it might be needed to download the auxiliarry files for debug purposes. If needed, this operation can be performed thorugh the command: 

# > condor_transfer_data -name sn-02 $USER

# If anything were to fail in the processing chain, you might need to remove your jobs by hand. To perform this operation, issue the command:

# > condor_rm -name sn-02 $USER

############# IMPORTANT!!!!! #################


############# SHOE INSTALLATION GUIDE #################

# To properly set-up the SHOE installation and have it working on condor, follow the steps given below:

# 1) Clone the appropriate branch of the SHOE git repository in your "/opt/exp_software/foot/${USER}" folder
#    > git clone https://baltig.infn.it/asarti/shoe.git -b <branch_name>

# 2) Set-up your work environment to have the correct gcc, cmake, python and ROOT versions ready to use.
#   > source /opt/exp_software/foot/root_shoe_foot.sh

# 3) Compile SHOE as you usually do:
#   > cd shoe
#   > mkdir build
#   > cd build
#   > cmake .. -D FILECOPY=ON                       (the "-DCMAKE_BUILD_TYPE=Debug" option is now given by default)
#   > make

# Here is a list of all the options available for SHOE compilation:

# - CMAKE_BUILD_TYPE    Type of build, usually either "Debug" of "Release" (default = Debug)
# - FILECOPY            Turn ON/OFF the config/calib/cammaps file copy (default = OFF)
# - GENFIT_DIR          Turn ON/OFF the global reconstruction w/ genfit (default = ON)
# - TOE_DIR             Turn ON/OFF the global reconstruction w/ TOE (default = OFF)
# - ANC_DIR             Turn ON/OFF the ancillary directory (default = OFF)
# - GEANT4_DIR          Turn ON/OFF the G4 simulations (default = OFF)

############# SHOE INSTALLATION GUIDE #################

# To signal any possible issue/missing feature, please contact zarrella@bo.infn.it

################ SCRIPT START ######################

echo "Start job submission!"

INPUT_BASE_PATH="/storage/gpfs_data/foot/"
OUTPUT_BASE_PATH="/storage/gpfs_data/foot/"
SHOE_BASE_PATH="/opt/exp_software/foot/${USER}"
SHOE_PATH=$(dirname $(realpath "$0"))
SHOE_PATH=${SHOE_PATH%Reconstruction/scripts}

if [[ ! "$SHOE_PATH" == *"$SHOE_BASE_PATH"* ]]; then
    echo "SHOE installation is not where it's supposed to be. Please move everything to ${SHOE_BASE_PATH} and re-run!"
    exit 0
fi

#Initialize some variables
outFile=""
isMc=0

while getopts i:o:m: flag
do
    case "${flag}" in
        i) inFile=${OPTARG};;
        o) outFile=${OPTARG};;
        m) isMc=${OPTARG};;
    esac
done

#I/O checks of input file
if [[ ! "$inFile" == *"$INPUT_BASE_PATH"* ]]; then
    inFile=$INPUT_BASE_PATH"/"$inFile
    echo "Input file path set outside ${INPUT_BASE_PATH}. Changed to ${inFile}"
fi

if [ ! -e "$inFile" ]; then
    echo "Input file ${inFile} not found!"
    exit 0
fi

#Check if output file has been set and initialize output folder
if [ -z "$outFile" ]; then
    echo "Output file not set, changing to default values for output directory"
    outFolder=$(dirname $inFile)
else
    outFolder=$(dirname $outFile)
fi

#I/O checks of output folder
if [[ ! "$outFolder" == *"$OUTPUT_BASE_PATH"* ]]; then
    outFolder=$OUTPUT_BASE_PATH"/"$outFolder
    echo "Output folder path set outside ${OUTPUT_BASE_PATH}. Changed to ${outFolder}"
fi

#Create folder if it did not exist
if [ ! -d "$outFolder" ]; then
    mkdir $outFolder
    if [ $? -ne 0 ]; then
        echo "Failed to create output directory. Exiting"
        exit 0
    fi
    echo "Directory ${outFolder} did not exist, created now!"
fi

#Remove slash from output folder if present
if [[ ${outFolder: -1} == "/" ]]; then
    outFolder=${outFolder::-1}
fi

#Create folder for condor auxiliary files if not present
HTCfolder="${outFolder}/HTCfiles"

if [ ! -d $HTCfolder ]; then
    mkdir $HTCfolder
    if [ $? -ne 0 ]; then
        echo "Failed to create condor files directory. Exiting"
        exit 0
    fi
    echo "Directory ${HTCfolder} did not exist, created now!"
fi

echo "Retrieving campaign and run number from input file..."

#Find number of events, campaign name and run number in the MC file using root
source /opt/exp_software/foot/root_shoe_foot.sh > /dev/null 2>&1

cd ${SHOE_PATH}/build/Reconstruction
root -l $inFile <<-EOF > /dev/null 2>&1
std::ofstream ofs("${HTCfolder}/temp_evts.txt")
std::ofstream ofsCamp("${HTCfolder}/temp_campaign.txt")
std::ofstream ofsRun("${HTCfolder}/temp_runNumber.txt")
ofs << tree->GetEntries()
ofsCamp << runinfo->CampaignName()
ofsRun << runinfo->RunNumber()
EOF
cd - > /dev/null 2>&1

echo "Done"

nTotEv=$(cat ${HTCfolder}/temp_evts.txt)
campaign=$(cat ${HTCfolder}/temp_campaign.txt)
runNumber=$(cat ${HTCfolder}/temp_runNumber.txt)

if [ -z $campaign ]; then
    echo "Campaign name not read correctly from input file. Check runinfo object"
    exit 0
fi

if [ -z $runNumber ]; then
    echo "Run number not read correctly from input file. Check runinfo object"
    exit 0
fi

#Remove temporary files
rm ${HTCfolder}/temp_*.txt

#Remove slash from extracted campaign if present
if [[ ${campaign: -1} == "/" ]]; then
    campaign=${campaign::-1}
fi

#Set output merged file name if not set before -> default value (campaign+runNumber form file)
if [ -z "$outFile" ]; then
    echo "Output file not set, changing to default name"
    outFile="${outFolder}/MergeAna_${campaign}_${runNumber}.root"
fi

# Set number of events per job and find number of jobs
nEvPerFile=50000

if [[ $(( $nTotEv % $nEvPerFile )) -eq 0 ]]; then
    nJobs=$(( $nTotEv / $nEvPerFile ))
else
    nJobs=$(( $nTotEv / $nEvPerFile + 1 ))
fi

echo
echo "-----------------------------------------------------"
echo "Running on MC file = "$inFile
echo "Campaign = " $campaign
echo "Run = "$runNumber
echo "Number of events = "$nTotEv
echo 'Number of jobs = '$nJobs
echo 'Output file = '$outFile
echo "-----------------------------------------------------"
echo

export _condor_SCHEDD_HOST=sn-02.cr.cnaf.infn.it

outFile_base="${outFolder}/outputAna_${campaign}_run${runNumber}_Job"

#Spawn total number jobs equal to number of file to process
jobExec="${HTCfolder}/runAnaInBatchMC_${campaign}_${runNumber}.sh"
jobExec_base=${jobExec::-3}

#Set mc command if sample is from simulation
mcflag=""
if [ $isMc -ne 0 ]; then
    mcflag="-mc"
fi

echo "Creating jobs for histogram folders merging"

# Create executable
# - par[1] = Process Id -> condor $(Process) variable + 1
# - par[2] = Number of events to skip in current process
#
# This executable processes a portion of the current input file, i.e. "nEvPerFile" events from a certain event onwards
cat <<EOF > $jobExec
#!/bin/bash

SCRATCH="\$(pwd)"
outFile_temp="\${SCRATCH}/tempAna_${campaign}_${runNumber}_\${1}.root"

source /opt/exp_software/foot/root_shoe_foot.sh
source ${SHOE_PATH}/build/setupFOOT.sh
cd ${SHOE_PATH}/build/Reconstruction

../bin/DecodeGlbAna -in ${inFile} -out \${outFile_temp} -exp ${campaign} -run ${runNumber} -nsk \${2} -nev ${nEvPerFile} ${mcflag}

if [ \$? -eq 0 ]; then
    mv \${outFile_temp} ${outFolder}
    mv ${outFolder}/\$(basename \${outFile_temp}) ${outFile_base}\${1}.root
else
    echo "Unexpected error in processing of file ${inFile} with options nsk=\${2} and nev=${nEvPerFile}"
fi
EOF

# Create single submit file for all jobs
# Spawn "nJobs" jobs to a single cluster with one submit file
filename_sub="${HTCfolder}/submitAnaMC_${campaign}_${runNumber}.sub"

cat <<EOF > $filename_sub
plusone = \$(Process) + 1
FileNum = \$INT(plusone,%d)
sk = \$(Process)*$nEvPerFile
SkipEv= \$INT(sk,%d)

executable            = ${jobExec}
arguments             = \$(FileNum) \$(SkipEv)
error                 = ${jobExec_base}_Job\$(FileNum).err
output                = ${jobExec_base}_Job\$(FileNum).out
log                   = ${jobExec_base}_Job\$(FileNum).log

queue $nJobs
EOF


# Submit SHOE processing jobs
chmod 754 ${jobExec}

echo "Finding list of folders..."

#Merge files !!
source /opt/exp_software/foot/root_shoe_foot.sh > /dev/null 2>&1
source ${SHOE_PATH}/build/setupFOOT.sh > /dev/null 2>&1
cd ${SHOE_PATH}/build/Reconstruction
../bin/DecodeGlbAna -in ${inFile} -out ${outFolder}/dummy_folders_${campaign}_${runNumber}.root -exp ${campaign} -run ${runNumber} ${mcflag} -nev 10 > /dev/null 2>&1
cd - > /dev/null 2>&1

root -l <<-EOF
TFile* _file0 = new TFile("${outFolder}/dummy_folders_${campaign}_${runNumber}.root", "r");
std::ofstream ofs("${outFolder}/dirs_${campaign}_${runNumber}.txt");
for( auto it : *_file0->GetListOfKeys() ){ofs << it->GetName() << endl; }
EOF

listOfDirs=($(cat ${outFolder}/dirs_${campaign}_${runNumber}.txt))
nDirs=${#listOfDirs[@]}

echo "Done"

mergeSingleDirExec="${HTCfolder}/MergeSingleDir_${campaign}_${runNumber}.sh"
mergeSingleDirExec_base=${mergeSingleDirExec::-3}

cat <<EOF > $mergeSingleDirExec
#!/bin/bash

source /opt/exp_software/foot/root_shoe_foot.sh
source ${SHOE_PATH}/build/setupFOOT.sh
SCRATCH="\$(pwd)"
outputFile="\${SCRATCH}/MergeDir_${campaign}_${runNumber}_\${1}_temp.root"
inputFiles=""

#While loop that checks if all files have been processed
while true; do
    for iFile in \$(seq 1 $nJobs);
    do
        if [[ -e ${outFile_base}\${iFile}.root && ! -e \${SCRATCH}/temp_\${1}_\${iFile}.root ]]; then
            echo "Found new processed file -> number: \${iFile}"
		    rootcp --recreate -r ${outFile_base}\${iFile}.root:\${1} \${SCRATCH}/temp_\${1}_\${iFile}.root
        fi
    done
    nCompletedFiles=\$(ls \${SCRATCH}/temp_\${1}_*.root | wc -l)

    #If all files have been processed, merge
	if [ \${nCompletedFiles} -eq ${nJobs} ]; then
        #Add files in correct order
        for iFile in \$(seq 1 $nJobs); do
            inputFiles="\${inputFiles} \${SCRATCH}/temp_\${1}_\${iFile}.root"
        done

        LD_PRELOAD=/opt/exp_software/foot/root/setTreeLimit_C.so hadd -j -f \${outputFile} \${inputFiles}
        mv \${outputFile} ${outFolder}
        rm \${inputFiles}
		break
	else
        echo "Waiting for merge of directory \${1} -> Imported \${nCompletedFiles}/${nJobs} files."
		sleep 10
	fi
done
EOF

# Create submit file for merge job
directory_sub="${HTCfolder}/submitMergeDir_${campaign}_${runNumber}.sub"

cat <<EOF > $directory_sub
executable            = ${mergeSingleDirExec}
arguments             = \$(dir)
error                 = ${mergeSingleDirExec_base}_\$(dir).err
output                = ${mergeSingleDirExec_base}_\$(dir).out
log                   = ${mergeSingleDirExec_base}_\$(dir).log

periodic_hold = time() - jobstartdate > 10800
periodic_hold_reason = "Merge for directory \$(dir) exceeded maximum runtime allowed, check presence of files in the output folder"

queue dir from (
$(cat ${outFolder}/dirs_${campaign}_${runNumber}.txt)
)
EOF

rm ${outFolder}/dummy_folders_${campaign}_${runNumber}.root
rm ${outFolder}/dirs_${campaign}_${runNumber}.txt

# Submit merge job
chmod 754 ${mergeSingleDirExec}


##Create merge job -> merge all single output files in the correct order
echo "Creating job for file merging!"
mergeJobExec="${HTCfolder}/MergeFilesAna_${campaign}_${runNumber}.sh"
mergeJobExec_base=${mergeJobExec::-3}

cat <<EOF > $mergeJobExec
#!/bin/bash

source /opt/exp_software/foot/root_shoe_foot.sh
source ${SHOE_PATH}/build/setupFOOT.sh
SCRATCH="\$(pwd)"
tempOutput="\${SCRATCH}/MergeAna_temp.root"
inputFiles="${outFolder}/MergeDir_${campaign}_${runNumber}_*_temp.root"

#While loop that checks if all files have been processed
while true; do
    nCompletedDirs=\$(ls ${outFolder}/MergeDir_${campaign}_${runNumber}_*_temp.root | wc -l)

	if [ \${nCompletedDirs} -eq ${nDirs} ]; then
        LD_PRELOAD=/opt/exp_software/foot/root/setTreeLimit_C.so hadd -f \${tempOutput} \${inputFiles}
        mv \${tempOutput} ${outFile}
        rm \${inputFiles} ${outFile_base}*.root
		break
	else
        echo "ERROR:: Analysis of ${campaign} run ${runNumber}, final merge -> Processed \${nCompletedDirs}/${nDirs} directories. Exiting..."
        exit 0
	fi
done
EOF

# Create submit file for merge job
merge_sub="${HTCfolder}/submitMergeAna_${campaign}_${runNumber}.sub"

cat <<EOF > $merge_sub
executable            = ${mergeJobExec}
error                 = ${mergeJobExec_base}.err
output                = ${mergeJobExec_base}.out
log                   = ${mergeJobExec_base}.log

periodic_hold = time() - jobstartdate > 10800
periodic_hold_reason = "Final merge of file ${outFile} exceeded maximum runtime allowed, check presence of files in the output folder"

queue
EOF

chmod 754 ${mergeJobExec}

# Create DAG for job steering
# 1. Process and directory merging 
# 2. Final merge
dag_sub="${HTCfolder}/submitAnaDAG_${campaign}_${runNumber}.sub"
cat <<EOF > ${dag_sub}
JOB process_Ana_${campaign}_${runNumber} ${filename_sub}
JOB merge_Dirs_${campaign}_${runNumber} ${directory_sub}
JOB merge_Final_${campaign}_${runNumber} ${merge_sub}
PARENT process_Ana_${campaign}_${runNumber} merge_Dirs_${campaign}_${runNumber} CHILD merge_Final_${campaign}_${runNumber}
EOF

# Submit DAG
cd ${HTCfolder}
condor_submit_dag -force ${dag_sub}
cd -