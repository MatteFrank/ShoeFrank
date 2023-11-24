#!/bin/bash

# This bash script launches SHOE on raw data using the HTCondor suite on Tier1. It is conceived to run on RAW DATA. The script launches a number of jobs in condor equal to the number of files present for that run.

# SINCE JOBS RUN IN PARALLEL, INPUT FILES NEED TO BE THE SYNCHRONIZED ONES!!!!!

# Before launching this script, a working SHOE installation and a properly set FootGlobal.par file is needed for the campaign you want to process. The SHOE installation must be placed in the "/opt/exp_software/foot/${USER}" folder, where "${USER}" is your username on the tier1 machine.
# IMPORTANT: make sure to compile SHOE using the shared installation of ROOT, which is placed in the "/opt/exp_software/foot/root" folder. A set of basic commands to compile SHOE on the Tier1 is given below in this file.

# To launch this script, issue the command:

# > ./path/to/runShoeBatchT1.sh -i inputFolder -o outputFolder -c campaign -r run

# where:
# - inputFolder is the path of the input folder. The input folder is forced to be inside "/storage/gpfs_data/foot/". This option needs either the full path of the input directory or the relative one starting from the "/storage/gpfs_data/foot/" folder.
# - outputFolder is the path to the output folder. This is forced to be in "/storage/gpfs_data/foot/${USER}". If you don't have a directory like this one already in the Tier1, it will be created.
# - campaign is the SHOE campaign as usual
# - run is the run number

# An optional argument can be given also to launch multiple runs of the campaign with a single command. In this case use:

# > ./path/to/runShoeBatchT1.sh -i inputFolder -o outputFolder -c campaign -r run -l lastRun

# where "lastRun" represents the last run to be processed (included!!)

# The script also allows for the possibility to merge the output files of each condor job in a single file through the "hadd" command of root, launched in a separate job. If you want this task to be performed, add the argument "-m 1" to the command line, as in:

# > ./path/to/runShoeBatchT1.sh -i inputFolder -o outputFolder -c campaign -r run -l lastRun -m 1

# NB: Keep in mind that this step erases the single job output files, so ony use it when you are 100% sure that SHOE will run without issues!

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

echo "Start job submission!"

INPUT_BASE_PATH="/storage/gpfs_data/foot"
OUTPUT_BASE_PATH="${INPUT_BASE_PATH}/${USER}"
SHOE_BASE_PATH="/opt/exp_software/foot/${USER}"
SHOE_PATH=$(dirname $(realpath "$0"))
SHOE_PATH=${SHOE_PATH%Reconstruction/scripts}

if [[ ! "$SHOE_PATH" == *"$SHOE_BASE_PATH"* ]]; then
    echo "SHOE installation is not where it's supposed to be. Please move everything to ${SHOE_BASE_PATH} and re-run!"
    exit 0
fi

lastRunNumber=-1
mergeFilesOpt=0

while getopts i:o:c:r:l:m: flag
do
    case "${flag}" in
        i) inFolder=${OPTARG};;
        o) outFolder=${OPTARG};;
        c) campaign=${OPTARG};;
        r) firstRunNumber=${OPTARG};;
        l) lastRunNumber=${OPTARG};;
        m) mergeFilesOpt=${OPTARG};;
    esac
done

#I/O checks of input folder
if [[ ! "$inFolder" == *"$INPUT_BASE_PATH"* ]]; then
    inFolder=$INPUT_BASE_PATH"/"$inFolder
    echo "Input folder path set outside ${INPUT_BASE_PATH}. Changed to ${inFolder}"
fi

if [ ! -d "$inFolder" ]; then
    echo "Input folder ${inFolder} not found!"
    exit 0
fi

#Remove slash from input folder if present
if [[ ${inFolder: -1} == "/" ]]; then
    inFolder=${inFolder::-1}
fi

#I/O checks of output folder
if [[ ! "$outFolder" == *"$OUTPUT_BASE_PATH"* ]]; then
    outFolder=$OUTPUT_BASE_PATH"/"$outFolder
    echo "Output folder path set outside ${OUTPUT_BASE_PATH}. Changed to ${outFolder}"
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

if [ $lastRunNumber -lt $firstRunNumber ]; then
    lastRunNumber=$firstRunNumber
else
    echo "Submitting runs from ${firstRunNumber} to ${lastRunNumber}"
fi

#Cycle on runs to process
for runNumber in $(seq $firstRunNumber $lastRunNumber);
do
    #Set output merged file name
    outMergedFile="${outFolder}/Merge_${campaign}_${runNumber}.root"

    #Count number of files == nJobs
    inputFilesRegex=$inFolder'/data*'$runNumber'*'
    nFiles=($(ls -1 $inputFilesRegex | wc -l))

    if [ $nFiles -eq 0 ]; then
        echo "No files found for run ${runNumber} in folder ${inFolder}!"
        continue
        # exit 0
    fi

    echo
    echo "-----------------------------------------------------"
    echo "Running on files in folder = "$inFolder
    echo "Campaign = " $campaign
    echo "Run = "$runNumber
    echo 'Number of files = '$nFiles
    echo 'Output folder = '$outFolder
    if [[ $mergeFilesOpt -eq 1 ]]; then
        echo 'Output file = '$outMergedFile
    fi
    echo "-----------------------------------------------------"
    echo

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

    export _condor_SCHEDD_HOST=sn-02.cr.cnaf.infn.it

    outFile_base="${outFolder}/output_${campaign}_run${runNumber}_Job"

    #Create executable
    # - par[1] = process Id -> condor $(Process) variable + 1
    # - par[2] = input file full path
    #
    # The exectuable launches SHOE on a file of the current run and returns the output file
    jobExec="${HTCfolder}/runShoeInBatch_${campaign}_${runNumber}.sh"
    jobExec_base=${jobExec::-3}

    # Create executable file for jobs
    cat <<EOF > $jobExec
#!/bin/bash

SCRATCH="\$(pwd)"
outFile_temp="\${SCRATCH}/temp_${campaign}_run${runNumber}_Job\${1}.root"

source /opt/exp_software/foot/root_shoe_foot.sh 
source ${SHOE_PATH}/build/setupFOOT.sh
cd ${SHOE_PATH}/build/Reconstruction

../bin/DecodeGlb -in \${2} -out \${outFile_temp} -exp ${campaign} -run ${runNumber} -subfile

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
    echo "Unexpected error in processing of file \${2}"
fi
EOF

    # Create submit file for jobs and submit them to one single cluster for all input files
    # This submission calls the above executable once per input file thorugh the "queue" command
    filename_sub="${HTCfolder}/submitShoe_${campaign}_${runNumber}.sub"

    cat <<EOF > $filename_sub
plusone = \$(Process) + 1
FileNum = \$INT(plusone,%d)

executable            = ${jobExec}
arguments             = \$(FileNum) \$(inputFileName)
error                 = ${jobExec_base}_Job\$(FileNum).err
output                = ${jobExec_base}_Job\$(FileNum).out
log                   = ${jobExec_base}_Job\$(FileNum).log

queue inputFileName from (
$(ls -1 $inputFilesRegex)
)
EOF

    # Submit all jobs for current run
    chmod 754 ${jobExec}
    condor_submit -spool ${filename_sub}

    #Merge files if requested!!
    if [[ $mergeFilesOpt -eq 1 && $nFiles -ge 1 ]]; then
        ##Create merge job -> merge all single output files in the correct order
        echo "Creating job for file merging!"
        mergeJobExec="${HTCfolder}/MergeFiles_${campaign}_${runNumber}.sh"
        mergeJobExec_base=${mergeJobExec::-3}

        cat <<EOF > $mergeJobExec
#!/bin/bash

source /opt/exp_software/foot/root_shoe_foot.sh
source ${SHOE_PATH}/build/setupFOOT.sh

echo "Merging files of run ${runNumber}!!"
SCRATCH="\$(pwd)"

#While loop that checks if all files have been processed
while true; do
    nCompletedFiles=\$(ls ${outFile_base}*.root | wc -l)

	if [ \${nCompletedFiles} -eq ${nFiles} ]; then
        command="\${SCRATCH}/Merge_temp.root"

        suffix=".root"
        if [ ! -f ${outFile_base}${nFiles}.root ]; then
            base="${outFile_base}${nFiles}"
            out_list=(\$(ls \${base}*))
            if [ \${#out_list[@]} -ne 1 ]; then
                echo "Unexpected error in processing of run ${runNumber}: wrong number of output files after processing"
            else
                outFile_temp=\${out_list[0]}
                suffix=\$(cut -c \$((\${#base}+1))-\${#outFile_temp} <<< \${outFile_temp})
            fi
        fi

        for iFile in \$(seq 1 $nFiles); do
            command="\${command} ${outFile_base}\${iFile}\${suffix}"
        done
        command="\${command} ${outFolder}/runinfo_${campaign}_${runNumber}.root"
        LD_PRELOAD=/opt/exp_software/foot/root/setTreeLimit_C.so hadd -j -f \${command}

        fileOut="Merge_${campaign}_${runNumber}\${suffix}"
        mv \${SCRATCH}/Merge_temp.root ${outFolder}/\${fileOut}
        rm ${outFile_base}*.root ${outFolder}/runinfo_${campaign}_${runNumber}.root
		break
	else
        echo "${campaign} run ${runNumber} -> Processed \${nCompletedFiles}/${nFiles} files. Waiting..."
		sleep 20
	fi
done
EOF

        # Create submit file for merge job, set to lower priority wrt file processing
        merge_sub="${HTCfolder}/submitMerge_${campaign}_${runNumber}.sub"

        cat <<EOF > $merge_sub
executable            = ${mergeJobExec}
error                 = ${mergeJobExec_base}.err
output                = ${mergeJobExec_base}.out
log                   = ${mergeJobExec_base}.log
request_cpus          = 8
priority              = -5
queue
EOF

        # Submit merge job
        chmod 754 ${mergeJobExec}
        condor_submit -spool ${merge_sub}
        echo "Submitted jobs for run ${runNumber}"
        
        if [ ${runNumber} -eq ${lastRunNumber} ]; then
            echo "All runs submitted!"
            exit 1
        fi
    fi
done
