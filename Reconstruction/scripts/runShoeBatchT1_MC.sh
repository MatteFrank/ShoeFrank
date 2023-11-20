#!/bin/bash

# This bash script launches SHOE on Monte Carlo simulated data using the HTCondor suite on Tier1. It is conceived to run on MC FILES. The script processes a single MC file launching a number of jobs in condor so that each of them processes ~20k events.

# Before launching this script, a working SHOE installation and a properly set FootGlobal.par file is needed for the campaign you want to process. The SHOE installation must be placed in the "/opt/exp_software/foot/${USER}" folder, where "${USER}" is your username on the tier1 machine.
# IMPORTANT: make sure to compile SHOE using the shared installation of ROOT, which is placed in the "/opt/exp_software/foot/root" folder. A set of basic commands to compile SHOE on the Tier1 is given below in this file.

# To launch this script, issue the command:

# > ./path/to/runShoeBatchT1_MC.sh -i inputFile -o outputFolder -c campaign -r run

# where:
# - inputFile is the path of the input file. The input file is forced to be inside "/storage/gpfs_data/foot/shared/SimulatedData". This option needs either the full path of the input directory or the relative one starting from the "/storage/gpfs_data/foot/shared/SimulatedData" folder.
# - outputFolder is the path to the output folder. This is forced to be in "/storage/gpfs_data/foot/${USER}". If you don't have a directory like this one already in the Tier1, it will be created.
# - campaign is the SHOE campaign as usual
# - run is the run number

# The script also allows for the possibility to merge the output files of each condor job in a single file through the "hadd" command of root, launched in a separate job. If you want this task to be performed, add the argument "-m 1" to the command line, as in:

# > ./path/to/runShoeBatchT1_MC.sh -i inputFile -o outputFolder -c campaign -r run -m 1

# NB: Keep in mind that this step erases the single job output files, so ony use it when you are 100% sure that SHOE will run without issues!

# When running on MC campaigns, the input folder could potentially contain more than one file. If the files only differ by a number in the format "_X_", the script has an additional option that can be used to process all files with one single command. Say that, for example, the inputFolder contains two files named "12C_200_1_shoereg.root" and "12C_200_2_shoereg.root". In this case, one can run on both files by launching:

# > ./path/to/runShoeBatchT1_MC.sh -i inputFolder/12C_200_1_shoereg.root -o outputFolder -c campaign -r run -m 1 -f 1

# The addition of the "-f 1" option tells the script to look for subsequent files and run all of them. A job is also created to handle the final merging of all output files.


############# MANDATORY!!!!! #################

# When the processing is done, you need to cleanup your job files from condor. To perform this operation, issue the command:

# > condor_rm -name sn-02 $USER

############# MANDATORY!!!!! #################


############# USEFUL!!!!! #################

# To check the status of your jobs, launch the command

# > condor_q -name sn-02 $USER

# All condor auxiliary files (.err/.out/.log) are not automatically downloaded. To download them launch

# > condor_transfer_data -name sn-02 $USER

############# USEFUL!!!!! #################


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

############# SHOE INSTALLATION GUIDE #################

# To signal any possible issue/missing feature, please contact zarrella@bo.infn.it

################ SCRIPT START ######################

INPUT_BASE_PATH="/storage/gpfs_data/foot/shared/SimulatedData"
OUTPUT_BASE_PATH="/storage/gpfs_data/foot/${USER}"
SHOE_BASE_PATH="/opt/exp_software/foot/${USER}"
SHOE_PATH=$(dirname $(realpath "$0"))
SHOE_PATH=${SHOE_PATH%Reconstruction/scripts}

if [[ ! "$SHOE_PATH" == *"$SHOE_BASE_PATH"* ]]; then
    echo "SHOE installation is not where it's supposed to be. Please move everything to ${SHOE_BASE_PATH} and re-run!"
    exit 0
fi

#Initialization of some variables
runNumber=-1
mergeFilesOpt=0
fullStat=0
fileNumber=0

while getopts i:o:c:r:m:f: flag
do
    case "${flag}" in
        i) inFile=${OPTARG};;
        o) outFolder=${OPTARG};;
        c) campaign=${OPTARG};;
        r) runNumber=${OPTARG};;
        m) mergeFilesOpt=${OPTARG};;
        f) fullStat=${OPTARG};;
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

#"full stat" required -> save the basename of the input file and its number
if [[ $fullStat -eq 1 ]]; then
    #switch on file merging forcibly when "full stat" is on
    mergeFilesOpt=1

    baseFileName=$(basename $inFile)
    for i in {1..9}
    do
        if [[ $baseFileName == *"_"$i"_"* ]]; then
            fileNumber=$i
            break
        fi
    done
fi

#I/O checks of output folder
if [[ ! "$outFolder" == *"$OUTPUT_BASE_PATH"* ]]; then
    outFolder=$OUTPUT_BASE_PATH"/"$outFolder
    echo "Output folder path set outside ${OUTPUT_BASE_PATH}. Changed to ${outFolder}"
fi

if [ ! -d "$outFolder" ]; then
    mkdir $outFolder
    echo "Directory ${outFolder} did not exist, created now!"
fi

#Remove slash from output folder if present
if [[ ${outFolder: -1} == "/" ]]; then
    outFolder=${outFolder::-1}
fi

#Handle the case in which the "full stat" is required: more sub-folders
if [[ ! $fileNumber -eq 0 ]]; then
    outFolder=${outFolder}"/"${fileNumber}
fi

if [ ! -d "$outFolder" ]; then
    mkdir $outFolder
    echo "Directory ${outFolder} did not exist, created now!"
fi


#Check that run number has been properly set
if [[ $runNumber -le 0 ]]; then
    echo "Run number not set properly!"
    exit 0
fi

#Create folder for condor auxiliary files if not present
HTCfolder="${outFolder}/HTCfiles"

if [ ! -d $HTCfolder ]; then
    mkdir $HTCfolder
    echo "Directory ${HTCfolder} did not exist, created now!"
fi

#Find number of events in the MC file using root
source /opt/exp_software/foot/root_shoe_foot.sh > /dev/null 2>&1

root -l $inFile <<-EOF > /dev/null 2>&1
std::ofstream ofs("dummy_evts.txt")
ofs << EventTree->GetEntries()
EOF

nTotEv=$(cat dummy_evts.txt)
rm dummy_evts.txt

# Set number of events per job and find number of jobs
nEvPerFile=20000

if [[ $(( $nTotEv % $nEvPerFile )) -eq 0 ]]; then
    nJobs=$(( $nTotEv / $nEvPerFile ))
else
    nJobs=$(( $nTotEv / $nEvPerFile + 1 ))
fi

#Set output merged file name
outMergedFile="${outFolder}/Merge_${campaign}_${runNumber}.root"

echo
echo "-----------------------------------------------------"
echo "Running on MC file = "$inFile
echo "Campaign = " $campaign
echo "Run = "$runNumber
echo "Number of events = "$nTotEv
echo 'Number of jobs = '$nJobs
echo 'Output folder = '$outFolder
if [[ $mergeFilesOpt -eq 1 ]]; then
    echo 'Output file = '$outMergedFile
fi
echo "-----------------------------------------------------"
echo

export _condor_SCHEDD_HOST=sn-02.cr.cnaf.infn.it

outFile_base="${outFolder}/output_${campaign}_run${runNumber}_Job"

#Cycle on jobs and launch them
for jobCounter in $(seq 1 $nJobs);
do
    #Cycle on files
    jobFilename="${HTCfolder}/runShoeInBatchMC_${campaign}_${runNumber}_${jobCounter}.sh"
    jobFilename_base=${jobFilename::-3}

    outFile="${outFile_base}${jobCounter}.root"
    skipEv=$(( $nEvPerFile * ($jobCounter - 1) ))

    # Create executable file for job
    cat <<EOF > $jobFilename
#!/bin/bash

SCRATCH="\$(pwd)"
outFile_temp="\${SCRATCH}/temp_${campaign}_${runNumber}_${jobCounter}.root"

source /opt/exp_software/foot/root_shoe_foot.sh 
source ${SHOE_PATH}/build/setupFOOT.sh
cd ${SHOE_PATH}/build/Reconstruction

../bin/DecodeGlb -in ${inFile} -out \${outFile_temp} -exp ${campaign} -run ${runNumber} -nsk ${skipEv} -nev ${nEvPerFile} -mc
retVal=\$?
if [ \$retVal -eq 0 ]; then
    if [ $jobCounter -eq 1 ]; then
        rootcp \${outFile_temp}:runinfo ${outFolder}/runinfo_${campaign}_${runNumber}.root
    fi
    rootrm \${outFile_temp}:runinfo
    mv \${outFile_temp} ${outFolder}
    mv ${outFolder}/\$(basename \${outFile_temp}) ${outFile}
else
    echo "Unexpected error in processing of file ${inFile} with options nsk=${skipEv} and nev=${nEvPerFile}"
fi
EOF

    # Create submit file for job
    filename_sub="${HTCfolder}/submitShoeMC_${campaign}_${runNumber}_${jobCounter}.sub"

    cat <<EOF > $filename_sub
executable            = ${jobFilename}
arguments             = \$(ClusterID) \$(ProcId)
error                 = ${jobFilename_base}.err
output                = ${jobFilename_base}.out
log                   = ${jobFilename_base}.log
+JobFlavour           = "longlunch"
max_retries           = 5
queue
EOF

    # Submit job
    chmod 754 ${jobFilename}
    condor_submit -spool ${filename_sub}
done


#Merge files if requested!!
if [[ $mergeFilesOpt -eq 1 ]]; then
    ##Create merge job -> merge all single output files in the correct order
    echo "Creating job for file merging!"
    mergeJobFileName="${HTCfolder}/MergeFiles_${campaign}_${runNumber}.sh"
    mergeJobFileName_base=${mergeJobFileName::-3}

    cat <<EOF > $mergeJobFileName
#!/bin/bash

source /opt/exp_software/foot/root_shoe_foot.sh
source ${SHOE_PATH}/build/setupFOOT.sh
SCRATCH="\$(pwd)"

#While loop that checks if all files have been processed
while true; do
    nCompletedFiles=\$(ls ${outFile_base}*.root | wc -l)

	if [ \${nCompletedFiles} -eq ${nJobs} ]; then

        command="\${SCRATCH}/Merge_temp.root"

        for iFile in \$(seq 1 $nJobs); do
            command="\${command} ${outFile_base}\${iFile}.root"
        done

        if [ $fullStat -eq 0 ] || [ $fileNumber -eq 1 ]; then
            command="\${command} ${outFolder}/runinfo_${campaign}_${runNumber}.root"
        fi
        
        LD_PRELOAD=/opt/exp_software/foot/root/setTreeLimit_C.so hadd -j -f \${command}
        mv \${SCRATCH}/Merge_temp.root \$(dirname ${outMergedFile})
        mv \$(dirname ${outMergedFile})/Merge_temp.root ${outMergedFile}

        rm ${outFile_base}*.root ${outFolder}/runinfo_${campaign}_${runNumber}.root
		break
	else
        echo "${campaign} run ${runNumber} -> Processed \${nCompletedFiles}/${nJobs} files. Waiting.."
		sleep 20
	fi
done
EOF

    # Create submit file for merge job
    merge_sub="${HTCfolder}/submitMerge_${campaign}_${runNumber}.sub"

    cat <<EOF > $merge_sub
executable            = ${mergeJobFileName}
arguments             = \$(ClusterID) \$(ProcId)
error                 = ${mergeJobFileName_base}.err
output                = ${mergeJobFileName_base}.out
log                   = ${mergeJobFileName_base}.log
request_cpus          = 8
+JobFlavour           = "longlunch"
queue
EOF

    # Submit merge job
    chmod 754 ${mergeJobFileName}
    condor_submit -spool ${merge_sub}
fi



#Merge output of different input files when full stat is on
if [[ ! $fileNumber -eq 0 ]]; then
    nextFileNumber=$((fileNumber+1))
    baseFileName="$(basename $inFile)"
    nextFileName="${baseFileName/_${fileNumber}_/_${nextFileNumber}_}"
    inFile="${inFile/$baseFileName/$nextFileName}"
    outFolder=${outFolder::-2}
    #Case1: next file found in folder, recall this script on the new file
    if [[ -e "$inFile" ]]; then
        echo "Moving to next file in the campaign"
        ./$0 -i $inFile -o $outFolder -c $campaign -r $runNumber -m 1 -f 1
    #Case2: next file does not exist, create and submit the job for the "full stat" merge
    else
        baseMergedSingleFile="$(basename $outMergedFile)"
        fullStatOutput="${outFolder}/MergeFullStat_${campaign}_${runNumber}.root"

        echo "Creating job for the merge of full statistics"
        echo
        echo "-----------------------------------------------------"
        echo "Campaign = " $campaign
        echo "Run = "$runNumber
        echo 'Number of input files = '$fileNumber
        echo 'Full statistics output file = '$fullStatOutput
        echo "-----------------------------------------------------"
        echo

        mergeJobFileName="${outFolder}/MergeFullStat_${campaign}_${runNumber}.sh"
        mergeJobFileName_base=${mergeJobFileName::-3}

        cat <<EOF > $mergeJobFileName
#!/bin/bash

source /opt/exp_software/foot/root_shoe_foot.sh
source ${SHOE_PATH}/build/setupFOOT.sh
SCRATCH="\$(pwd)"

#While loop that checks if all files have been processed
while true; do
    nCompletedFiles=\$(ls ${outFolder}/*/${baseMergedSingleFile} | wc -l)

    if [ \${nCompletedFiles} -eq ${fileNumber} ]; then
        command="\${SCRATCH}/Merge_temp.root"

        for iFile in \$(seq 1 $fileNumber); do
            command="\${command} ${outFolder}/\${iFile}/${baseMergedSingleFile}"
        done

        LD_PRELOAD=/opt/exp_software/foot/root/setTreeLimit_C.so hadd -j -f \${command}
        retVal=\$?
        if [ \$retVal -eq 0 ]; then
            mv \${SCRATCH}/Merge_temp.root ${fullStatOutput}
            rm ${outFolder}/*/${baseMergedSingleFile}
        fi
        break
    else
        echo "${campaign} run ${runNumber} full statistics -> Processed \${nCompletedFiles}/${fileNumber} files. Waiting.."
        sleep 20
    fi
done
EOF

        # Create submit file for full statistics merge job
        merge_sub="${outFolder}/submitMergeFullStat_${campaign}_${runNumber}.sub"

        cat <<EOF > $merge_sub
executable            = ${mergeJobFileName}
arguments             = \$(ClusterID) \$(ProcId)
error                 = ${mergeJobFileName_base}.err
output                = ${mergeJobFileName_base}.out
log                   = ${mergeJobFileName_base}.log
request_cpus          = 8
+JobFlavour           = "workday"
queue
EOF

        chmod 754 ${mergeJobFileName}
        condor_submit -spool ${merge_sub}
    fi
fi
