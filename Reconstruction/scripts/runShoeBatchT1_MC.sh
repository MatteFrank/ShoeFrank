#!/bin/bash

# This bash script launches SHOE on Monte Carlo simulated data using the HTCondor suite on Tier1. It is conceived to run on MC FILES. The script processes a single MC file launching a number of jobs in condor so that each of them processes ~20k events.

# Before launching this script, a working SHOE installation and a properly set FootGlobal.par file is needed for the campaign you want to process. The SHOE installation must be placed in the "/opt/exp_software/foot/${USER}" folder, where "${USER}" is your username on the tier1 machine.
# IMPORTANT: make sure to compile SHOE using the shared installation of ROOT, which is placed in the "/opt/exp_software/foot/root" folder. A set of basic commands to compile SHOE on the Tier1 is given below in this file.

# To launch this script, issue the command:

# > ./path/to/runShoeBatchT1_MC.sh -i inputFile -o outputFolder

# where:
# - inputFile is the path of the input file. The input file is forced to be inside "/storage/gpfs_data/foot/shared/SimulatedData".
# - outputFolder is the path to the output folder. This is forced to be in "/storage/gpfs_data/foot/${USER}". If you don't have a directory like this one already in the Tier1, it will be created.

# By default, the script also merges the output files of each input in a single file through the "hadd" command of root. This also deletes the single job output files once the merge is completed. If you want to disable this feature and keep the single output files, add the argument "-m 0" to the command line, as in::

# > ./path/to/runShoeBatchT1_MC.sh -i inputFile -o outputFolder -m 0

# NB: Keep in mind that this step erases the single job output files, so ony use it when you are 100% sure that SHOE will run without issues!

# When running on MC campaigns, the input folder could potentially contain more than one file. If the files only differ by a number in the format "_X_", the script has an additional option that can be used to process all files with one single command. Say that, for example, the inputFolder contains two files named "12C_200_1_shoereg.root" and "12C_200_2_shoereg.root". In this case, one can run on both files by launching:

# > ./path/to/runShoeBatchT1_MC.sh -i inputFolder/12C_200_1_shoereg.root -o outputFolder -f 1

# The addition of the "-f 1" option tells the script to look for subsequent files and run all of them. A job is also created to handle the final merging of all output files. If the "-f 1" option is specified, the file merging is forced.


############# IMPORTANT!!!!! #################

# To check the status of your jobs, launch the command

# > condor_q -name sn-02 $USER

# All condor auxiliary files (.err/.out/.log) are automatically downloaded and the completed jobs removed when the file merge is requested. If one turns off the file merging, it might be needed to download the auxiliarry files for debug purposes. If needed, this operation can be performed thorugh the command: 

# > condor_transfer_data -name sn-02 $USER

# If anything were to fail in the processing chain, you might need to remove your jobs by hand. To perform this operation, issue the command:

# > condor_rm -name sn-02 $USER

############# IMPORTANT!!!!! #################


############# SHOE TIER-1 INSTALLATION GUIDE #################

# To properly set-up the SHOE installation and have it working on Tier1, follow the steps given below:

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

# - CMAKE_BUILD_TYPE    (default = Debug)   Type of build, usually either "Debug" of "Release"
# - FILECOPY            (default = OFF)     Turn ON/OFF the config/calib/cammaps file copy
# - GENFIT_DIR          (default = ON)      Turn ON/OFF the global reconstruction w/ genfit
# - TOE_DIR             (default = OFF)     Turn ON/OFF the global reconstruction w/ TOE
# - ANC_DIR             (default = OFF)     Turn ON/OFF the ancillary directory
# - GEANT4_DIR          (default = OFF)     Turn ON/OFF the G4 simulations

############# SHOE TIER-1 INSTALLATION GUIDE #################

# To signal any possible issue/missing feature, please contact zarrella@bo.infn.it

################ SCRIPT START ######################

echo "Start job submission!"

INPUT_BASE_PATH="/storage/gpfs_data/foot/shared/SimulatedData"
OUTPUT_BASE_PATH="/storage/gpfs_data/foot/"
SHOE_BASE_PATH="/opt/exp_software/foot/${USER}"
SHOE_PATH=$(dirname $(realpath "$0"))
SHOE_PATH=${SHOE_PATH%Reconstruction/scripts}

if [[ ! "$SHOE_PATH" == *"$SHOE_BASE_PATH"* ]]; then
    echo "SHOE installation is not where it's supposed to be. Please move everything to ${SHOE_BASE_PATH} and re-run!"
    exit 0
fi

#Initialization of some variables
runNumber=-1
mergeFilesOpt=1
fullStat=0
fileNumber=0

while getopts i:o:c:r:m:f: flag
do
    case "${flag}" in
        i) inFile=${OPTARG};;
        o) outFolder=${OPTARG};;
        m) mergeFilesOpt=${OPTARG};;
        f) fullStat=${OPTARG};;
    esac
done

inFile=$(realpath ${inFile})
outFolder=$(realpath ${outFolder})

#I/O checks of input file
if [ ! -e "$inFile" ]; then
    echo "Input file ${inFile} not found!"
    exit 0
fi

if [[ ! "$inFile" == *"$INPUT_BASE_PATH"* ]]; then
    echo "Input file path set outside ${INPUT_BASE_PATH}! Please, choose from files inside this directory."
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
    echo "Output folder path set outside ${OUTPUT_BASE_PATH}. Please, choose another output path"
    exit 0
fi

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

#Handle the case in which the "full stat" is required: more sub-folders
if [[ ! $fileNumber -eq 0 ]]; then
    outFolder=${outFolder}"/"${fileNumber}
    if [ ! -d "$outFolder" ]; then
        mkdir $outFolder
        if [ $? -ne 0 ]; then
            echo "Failed to create output directory. Exiting"
            exit 0
        fi
        echo "Directory ${outFolder} did not exist, created now!"
    fi
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

#Find number of events in the MC file using root
source /opt/exp_software/foot/root_shoe_foot.sh > /dev/null 2>&1

cd ${SHOE_PATH}/build/Reconstruction
root -l $inFile <<-EOF > /dev/null 2>&1
std::ofstream ofs("${HTCfolder}/temp_evts.txt")
std::ofstream ofsCamp("${HTCfolder}/temp_campaign.txt")
std::ofstream ofsRun("${HTCfolder}/temp_runNumber.txt")
ofs << EventTree->GetEntries()
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

outFile_base="${outFolder}/output_${campaign}_${runNumber}_Job"

#Spawn total number jobs equal to number of file to process
jobExec="${HTCfolder}/runShoeInBatchMC_${campaign}_${runNumber}.sh"
jobExec_base=${jobExec::-3}

# Create executable
# - par[1] = Process Id -> condor $(Process) variable + 1
# - par[2] = Number of events to skip in current process
#
# This executable processes a portion of the current input file, i.e. "nEvPerFile" events from a certain event onwards
cat <<EOF > $jobExec
#!/bin/bash

SCRATCH="\$(pwd)"
outFile_temp="\${SCRATCH}/temp_${campaign}_${runNumber}_Job\${1}.root"

source /opt/exp_software/foot/root_shoe_foot.sh 
source ${SHOE_PATH}/build/setupFOOT.sh
cd ${SHOE_PATH}/build/Reconstruction

../bin/DecodeGlb -in ${inFile} -out \${outFile_temp} -exp ${campaign} -run ${runNumber} -nsk \${2} -nev ${nEvPerFile} -mc

if [ \$? -eq 0 ]; then
    out_list=(\$(ls \${SCRATCH}/*.root))
    if [ ! -f \${outFile_temp} ]; then
        outFile_temp=\${out_list[0]}
    fi
    
    if [ \${1} -eq 1 ]; then
        rootcp \${outFile_temp}:runinfo ${outFolder}/runinfo_${campaign}_${runNumber}.root
    fi
    rootrm \${outFile_temp}:runinfo
    outFile=\${outFile_temp/temp/output}
    mv \${outFile_temp} ${outFolder}/\$(basename \${outFile})
else
    echo "Unexpected error in processing of file ${inFile} with options nsk=\${2} and nev=${nEvPerFile}"
fi
EOF

# Create single submit file for all jobs
# Spawn "nJobs" jobs to a single cluster with one submit file
filename_sub="${HTCfolder}/submitShoeMC_${campaign}_${runNumber}.sub"

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

# Make SHOE processing executable
chmod 754 ${jobExec}

#Merge files if requested!!
if [[ $mergeFilesOpt -eq 1 ]]; then
    ##Create merge job -> merge all single output files in the correct order
    echo "Creating job for file merging!"
    mergeJobExec="${HTCfolder}/MergeFiles_${campaign}_${runNumber}.sh"
    mergeJobExec_base=${mergeJobExec::-3}

    cat <<EOF > $mergeJobExec
#!/bin/bash

source /opt/exp_software/foot/root_shoe_foot.sh
source ${SHOE_PATH}/build/setupFOOT.sh
SCRATCH="\$(pwd)"

#Checks if all files have been processed, else exit
nCompletedFiles=\$(ls ${outFile_base}*.root | wc -l)

if [ \${nCompletedFiles} -eq ${nJobs} ]; then
    command="\${SCRATCH}/Merge_temp.root"

    suffix=".root"
    if [ ! -f ${outFile_base}${nJobs}.root ]; then
        base="${outFile_base}${nJobs}"
        out_list=(\$(ls \${base}*))
        if [ \${#out_list[@]} -ne 1 ]; then
            echo "Unexpected error in processing of MC campaign ${campaign} run ${runNumber}: wrong number of output files after processing"
        else
            outFile_temp=\${out_list[0]}
            suffix=\$(cut -c \$((\${#base}+1))-\${#outFile_temp} <<< \${outFile_temp})
        fi
    fi

    for iFile in \$(seq 1 $nJobs); do
        command="\${command} ${outFile_base}\${iFile}\${suffix}"
    done
    if [ $fullStat -eq 0 ] || [ $fileNumber -eq 1 ]; then
        command="\${command} ${outFolder}/runinfo_${campaign}_${runNumber}.root"
    fi
    
    fileOut_temp="${outMergedFile}"
    fileOut=\${fileOut_temp/.root/\${suffix}}
    LD_PRELOAD=/opt/exp_software/foot/root/setTreeLimit_C.so hadd -j -f \${command}
    mv \${SCRATCH}/Merge_temp.root \$(dirname ${outMergedFile})
    mv \$(dirname ${outMergedFile})/Merge_temp.root \${fileOut}

    rm ${outFile_base}*.root ${outFolder}/runinfo_${campaign}_${runNumber}.root
    break
else
    echo "ERROR:: ${campaign} run ${runNumber} -> Processed \${nCompletedFiles}/${nJobs} files. Exiting.."
    exit 0
fi
EOF

    # Create submit file for merge job
    merge_sub="${HTCfolder}/submitMerge_${campaign}_${runNumber}.sub"

    cat <<EOF > $merge_sub
executable            = ${mergeJobExec}
error                 = ${mergeJobExec_base}.err
output                = ${mergeJobExec_base}.out
log                   = ${mergeJobExec_base}.log
request_cpus          = 8

periodic_hold = time() - jobstartdate > 10800
periodic_hold_reason = "Merge of output from ${inFile} exceeded maximum runtime allowed, check presence of single files in the output folder"

queue
EOF

    # Make merge job executable
    chmod 754 ${mergeJobExec}

    # Create DAG job for single file if not running on full statistics
    # 1. Process
    # 2. Merge
    if [ $fullStat -eq 0 ]; then
        dag_sub="${HTCfolder}/submitDAG_${campaign}_${runNumber}.sub"

        cat <<EOF > $dag_sub
JOB process ${filename_sub}
JOB merge ${merge_sub}
PARENT process CHILD merge
EOF

        #submit DAG for single file merging
        cd ${HTCfolder}
        condor_submit_dag -force ${dag_sub}
        cd -
    fi
else #Merge files not requested -> run processing alone
    condor_submit -spool ${filename_sub}
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
        ./$0 -i $inFile -o $outFolder -f 1
    #Case2: next file does not exist, create and submit the job for the "full stat" merge
    else
        baseMergedSingleFile="$(basename $outMergedFile)"
        fullStatOutput="${outFolder}/MergeFullStat_${campaign}_${runNumber}.root"
        HTCfolder="${outFolder}/HTCfiles"
        if [ ! -d $HTCfolder ]; then
            mkdir $HTCfolder
            if [ $? -ne 0 ]; then
                echo "Failed to create condor files directory. Exiting"
                exit 0
            fi
            echo "Directory ${HTCfolder} did not exist, created now!"
        fi

        echo "Creating job for the merge of full statistics"
        echo
        echo "-----------------------------------------------------"
        echo "Campaign = " $campaign
        echo "Run = "$runNumber
        echo 'Number of input files = '$fileNumber
        echo 'Full statistics output file = '$fullStatOutput
        echo "-----------------------------------------------------"
        echo

        mergeJobExec="${HTCfolder}/MergeFullStat_${campaign}_${runNumber}.sh"
        mergeJobExec_base=${mergeJobExec::-3}

        cat <<EOF > $mergeJobExec
#!/bin/bash

source /opt/exp_software/foot/root_shoe_foot.sh
source ${SHOE_PATH}/build/setupFOOT.sh
SCRATCH="\$(pwd)"

#While loop that checks if all files have been processed, else exit
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
    echo "ERROR:: ${campaign} run ${runNumber} full statistics -> Processed \${nCompletedFiles}/${fileNumber} files. Exiting.."
    exit 0
fi
EOF

        # Create submit file for full statistics merge job
        merge_sub="${HTCfolder}/submitMergeFullStat_${campaign}_${runNumber}.sub"

        cat <<EOF > $merge_sub
executable            = ${mergeJobExec}
error                 = ${mergeJobExec_base}.err
output                = ${mergeJobExec_base}.out
log                   = ${mergeJobExec_base}.log
request_cpus          = 8

periodic_hold = time() - jobstartdate > 14400
periodic_hold_reason = "Merge of full stat output exceeded maximum runtime allowed, check presence of files in the output folder"

queue
EOF

        chmod 754 ${mergeJobExec}

        # Create DAG job for full statistics merge
        # 1. Process all files
        # 2. Merge of single files
        # 3. Final merge
        dag_sub="${HTCfolder}/submitDAG_fullStat_${campaign}_${runNumber}.sub"
        if [ -e "$dag_sub" ]; then
            rm ${dag_sub}
        fi

        #Define PARENT-CHILD job relation
        lastDAGline="PARENT"
        touch ${dag_sub}
        for iFile in $(seq 1 ${fileNumber}); do
        cat <<EOF >> $dag_sub
JOB process_${campaign}_${runNumber}_file${iFile} ${outFolder}/${iFile}/HTCfiles/submitShoeMC_${campaign}_${runNumber}.sub
JOB merge_${campaign}_${runNumber}_file${iFile} ${outFolder}/${iFile}/HTCfiles/submitMerge_${campaign}_${runNumber}.sub
PARENT process_${campaign}_${runNumber}_file${iFile} CHILD merge_${campaign}_${runNumber}_file${iFile}
EOF
        lastDAGline="${lastDAGline} merge_${campaign}_${runNumber}_file${iFile}"
        done

        cat <<EOF >> $dag_sub
JOB full_merge_${campaign}_${runNumber} ${merge_sub}
EOF
        lastDAGline="${lastDAGline} CHILD full_merge_${campaign}_${runNumber}"
        echo ${lastDAGline} >> ${dag_sub}

        #Submit DAG for final output merge
        cd ${HTCfolder}
        condor_submit_dag -force ${dag_sub}
        cd -

    fi
fi
