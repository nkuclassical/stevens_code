#!/bin/bash
#export EC2_BACKUP_FLAGS_AWS="--key-name mykey --instance-type t2.micro"
#export EC2_BACKUP_FLAGS_SSH="-i /Users/yanqiaozhan/.ssh/ec2-key"
USER_NAME="ubuntu"
HOST_NAME=""
HOST_IP=""
IMG_ID="ami-d05e75b8"
INSTANCE_ID=""
SECURITY_KEY=""
SECURITY_GROUP=""
DIR=""
VOLUME_SIZE=""
VOLUME_ID=""
METHOD="dd"
DEVICE_NAME="/dev/xvdh"
ATTACH_POINT="/dev/sdh"
DEFAULT_VOLUME_DIR="/mnt/newvolume"
handle_keypair=""
EC2_BACKUP_VERBOSE=""
ARGS=("$@")
FILE_SYSTEM_TYPE="ext4"
EXITDIRECTLY=""
ec2backup_help() {
    echo "EC2-BACKUP(1)       BSD General Commands Manual        EC2-BACKUP(1)

NAME
     ec2-backup -- backup a directory into Elastic Block Storage (EBS)

SYNOPSIS
     ec2-backup [-h] [-m method] [-v volume-id] dir

DESCRIPTION
     The ec2-backup tool performs a backup of the given directory into Amazon
     Elastic Block Storage (EBS).  This is achieved by creating a volume of
     the appropriate size, attaching it to an EC2 instance and finally copying
     the files from the given directory into this volume.

OPTIONS
     ec2-backup accepts the following command-line flags:

     -h        Print a usage statement and exit.

     -m method     Use the given method to perform the backup.  Valid methods
           are 'dd' and 'rsync'; default is 'dd'.

     -v volume-id  Use the given volume instead of creating a new one.

DETAILS
     ec2-backup will perform a backup of the given directory to an ESB volume.
     The backup is done in one of two ways: via direct write to the volume as
     a block device (utilizing tar(1) on the local host and dd(1) on the
     remote instance), or via a (possibly incremental) filesystem sync (uti-
     lizing rsync(1)).

     Unless the -v flag is specified, ec2-backup will create a new volume, the
     size of which will be at least two times the size of the directory to be
     backed up.

     ec2-backup will create an instance suitable to perform the backup, attach
     the volume in question and then back up the data from the given directory
     using the specified method and then shut down and terminate the instance
     it created.

OUTPUT
     By default, ec2-backup prints the volume ID of the volume to which it
     backed up the data as the only output.  If the EC2_BACKUP_VERBOSE envi-
     ronment variable is set, it may also print out some useful information
     about what steps it is currently performing.

     Any errors encountered cause a meaningful error message to be printed to
     STDERR.

ENVIRONMENT
     ec2-backup assumes that the user has set up their environment for general
     use with the EC2 tools.  That is, it will not set or modify the variables
     AWS_CONFIG_FILE, EC2_CERT, EC2_HOME or EC2_PRIVATE_KEY.

     ec2-backup allows the user to add custom flags to the commands related to
     starting a new EC2 instance via the EC2_BACKUP_FLAGS_AWS environment
     variable.

     ec2-backup also assumes that the user has set up their ~/.ssh/config file
     to access instances in EC2 via ssh(1) without any additional settings.
     It does allow the user to add custom flags to the ssh(1) commands it
     invokes via the EC2_BACKUP_FLAGS_SSH environment variable.

     As noted above, the EC2_BACKUP_VERBOSE variable may cause ec2-backup to
     generate informational output as it runs.

EXIT STATUS
     The ec2-backup will exit with a return status of 0 under normal circum-
     stances.  If an error occurred, ec2-backup will exit with a value >0.

EXAMPLES
     The following examples illustrate common usage of this tool.

     To back up the entire filesystem using rsync(1):

       $ ec2-backup -m rsync /
       vol-a1b2c3d4
       $ echo $?
       0
       $

     To create a complete backup of the current working directory using
     defaults (and thus not requiring a filesystem to exist on the volume) to
     the volume with the ID vol-1a2b3c4d:

       ec2-backup -v vol-1a2b3c4d .

     Suppose a user has their ~/.ssh/config set up to use the private key
     ~/.ec2/stevens but wishes to use the key ~/.ssh/ec2-key instead:

       $ export EC2_BACKUP_FLAGS_SSH=\"-i ~/.ssh/ec2-key\"
       $ ec2-backup .
       vol-a1b2c3d4
       $

     To force creation of an instance type of t1.micro instead of whatever
     defaults might apply

       $ export EC2_BACKUP_FLAGS_AWS=\"--instance-type t1.micro\"
       $ ec2-backup .
       vol-a1b2c3d4
       $

SEE ALSO
     aws help, dd(1), tar(1), rsync(1)

HISTORY
     ec2-backup was originally assigned by Jan Schaumann
     <jschauma@cs.stevens.edu> as a homework assignment for the class \"Aspects
     of System Administration\" at Stevens Institute of Technology in the
     Spring of 2011.

BSD             March 17, 2014                 BSD"
    exit 0
}
detach_volume()
{
	echo "detach volume"$VOLUME_ID
	if [ ! -z $VOLUME_ID ]; then
		aws ec2 detach-volume --volume-id $VOLUME_ID 1>&2 2>/dev/null
	fi
}
terminate_instance()
{
	
	echo "terminate instance"$INSTANCE_ID
	if [ ! -z $INSTANCE_ID ]; then
		local temp=`aws ec2 terminate-instances --instance-ids $INSTANCE_ID`
	fi
}
validate_create_instance() {
	if [ ! -z $INSTANCE_ID ];
	then
		local instance_status=$(aws ec2 describe-instances --instance-ids $INSTANCE_ID --output text --filters "Name=instance-id, Values=$INSTANCE_ID" --query 'Reservations[0].[Instances[0].State.Code]')
		if [ $instance_status -eq 16 ];
		then
			return 0
		else
			return 1
		fi	
	else
		
		exit 1
	fi
}
validate_dir()
{
	if [ ! -e $DIR ]; then
		echo "The directory not exists!"
		exit 1
	elif [ -z $DIR ]; then
			echo "Empty directory path, forget to type in?"
			exit 1
	fi
}
validate_filesystem()
{
    if [ ! -z $HOST_NAME ]; then
        local filesystem_type=`ssh $EC2_BACKUP_FLAGS_SSH $USER_NAME@$HOST_NAME "sudo file -s $DEVICE_NAME|grep ext4"`
        if [ ! -z filesystem_type ]; then
            return 0
        else
            return 1
        fi
    else
        echo "Host not exists"
    fi
}
validate_volume_exist() {
	if [ ! -z $VOLUME_ID ];
	then
		local volume_status=$(aws ec2 describe-volumes --volume-id $VOLUME_ID --output text | awk '{print$6}')
		if [[  $volume_status == *available* ]];
		then
			return 0
		else
			return 1
		fi
	else
		echo "Volume ID not exist, do check"
        exit 1
	fi		
}

visual_sleep() {
	local max=$2
	local n
	local func=$1
	
	n=0
	while [ $n -lt ${max} ]; do
 		
 		if [ ! -z $EC2_BACKUP_VERBOSE ];
 		then
 			printf .		
 		fi
 		
		n=$(( n + 1 ))
		$func
		if [ $? -eq 0 ];
		then
			break
		fi
		sleep 1
	done
	if [ ! -z $EC2_BACKUP_VERBOSE ];
 	then
 		printf "\n"
	fi
 	
	
}

create_instance()
{
	echo "aws ec2 run-instances --image-id $IMG_ID --placement AvailabilityZone=us-east-1e $EC2_BACKUP_FLAGS_AWS"
	INSTANCE_ID=`aws ec2 run-instances --image-id $IMG_ID --placement AvailabilityZone=us-east-1e $EC2_BACKUP_FLAGS_AWS --output text --query 'Instances[0].[InstanceId]'`	
	if [ $? -ne 0 ]; then
		#statements
		terminate_instance
		exit 1
	fi
	while [ -z $HOST_NAME ]; do
		echo "sleep 5 seconds"
		sleep 5
		HOST_NAME=`aws ec2 describe-instances --filters "Name=instance-id, Values=$INSTANCE_ID" --output text --query 'Reservations[0].[Instances[0].PublicDnsName]'`
	done
	echo "aws ec2 describe-instances --filters 'Name=instance-id, Values=$INSTANCE_ID'"
	visual_sleep validate_create_instance 60
	
}



create_volume()
{

	size=`du -shm $DIR|cut -f1|sed s/M//`
	size=`expr $size + $size`
	size=`expr $size / 1024`
	if [[ $size -eq 0 ]]; then
		size=`expr $size + 1`
	fi
	
	VOLUME_ID=`aws ec2 create-volume --size 1 --availability us-east-1e --query 'VolumeId'|cut -d\" -f2`
	echo $VOLUME_ID
	visual_sleep validate_volume_exist 45
	
}
attach_volume()
{
    local temp=`aws ec2 attach-volume --volume-id $VOLUME_ID --instance $INSTANCE_ID --device $ATTACH_POINT`
    echo "aws ec2 attach-volume --volume-id $VOLUME_ID --instance $INSTANCE_ID --device $ATTACH_POINT"
}



dd_backup() {
	echo "tar -cf - $1 | ssh ${EC2_BACKUP_FLAGS_SSH} -o BatchMode=yes -o StrictHostKeyChecking=no $USER_NAME@$HOST_NAME 'sudo dd of=$DEVICE_NAME' 1>/dev/null 2>/dev/null"
	local dd_cmd="tar -cPf - $1 | ssh ${EC2_BACKUP_FLAGS_SSH} -o BatchMode=yes -o StrictHostKeyChecking=no $USER_NAME@$HOST_NAME 'sudo dd of=$DEVICE_NAME' 1>/dev/null 2>/dev/null"
	eval $dd_cmd
	echo "dd_backup finished"
	
}
create_filesystem()
{

    echo "create filesystem"
    ssh $EC2_BACKUP_FLAGS_SSH -o ConnectTimeout=30 -o BatchMode=yes -o StrictHostKeyChecking=no $USER_NAME@$HOST_NAME  "sudo mkfs -t $FILE_SYSTEM_TYPE $DEVICE_NAME" 1>&2 2>/dev/null
    visual_sleep validate_filesystem 30
    ssh $EC2_BACKUP_FLAGS_SSH -o ConnectTimeout=30 -o BatchMode=yes -o StrictHostKeyChecking=no $USER_NAME@$HOST_NAME  "sudo mkdir $DEFAULT_VOLUME_DIR" 1>&2 2>/dev/null
    ssh $EC2_BACKUP_FLAGS_SSH -o ConnectTimeout=30 -o BatchMode=yes -o StrictHostKeyChecking=no $USER_NAME@$HOST_NAME  "sudo mount $DEVICE_NAME $DEFAULT_VOLUME_DIR" 1>&2 2>/dev/null
    echo "finished create filesystem"
}
show_rsync_backup_result()
{

    $(ssh $EC2_BACKUP_FLAGS_SSH -o ConnectTimeout=10 -o BatchMode=yes -o StrictHostKeyChecking=no $USER_NAME@$HOST_NAME "sudo DEBIAN_FRONTEND=noninteractive apt-get install tree") 1>&2 2>/dev/null
    echo "directory structure of $DEFAULT_VOLUME_DIR"
    ssh $EC2_BACKUP_FLAGS_SSH -o ConnectTimeout=30 -o BatchMode=yes -o StrictHostKeyChecking=no $USER_NAME@$HOST_NAME "tree $DEFAULT_VOLUME_DIR"
}
rsync_backup()
{

    echo "rsync_backup start"
	create_filesystem
    local rsync_cmd="rsync -e \"ssh ${EC2_BACKUP_FLAGS_SSH}\" --rsync-path=\"sudo rsync\" -avzh $DIR $USER_NAME@$HOST_NAME:$DEFAULT_VOLUME_DIR"
    echo $rsync_cmd
    eval $rsync_cmd
    echo "rsync_backup finished"
    show_rsync_backup_result
}
validate_ssh_connection()
{
	ssh $EC2_BACKUP_FLAGS_SSH -o ConnectTimeout=10 -o BatchMode=yes -o StrictHostKeyChecking=no $USER_NAME@$HOST_NAME 'exit' 1>&2 2>/dev/null
}

usage() {
    echo "Usage: ec2-backup [-h] [-m dd/rsync] [-v volume-id] dir"
    exit 2
}
parse_command()
{
    for (( i = 0; i < $# - 1; i++ )); do
        case ${ARGS[i]} in
            -h )
                ec2backup_help
                EXITDIRECTLY="true"
                echo "hello";;
            -m )            #method
                i=`expr $i + 1`
                if [[ $i =  `expr $# - 1` ]]; then
                    echo "Missing option 'dir'"
                    usage
                elif [[ ${ARGS[i]} = 'dd' ]]; then
                    METHOD="dd"
                elif [[ ${ARGS[i]} = 'rsync' ]]; then
                    METHOD="rsync"
                else
                    echo "Unrecognized option -m ${ARGS[i]}"
                    usage
                fi;;
            -v )            #volume ID
                i=`expr $i + 1`
                if [[ $i =  `expr $# - 1` ]]; then
                    echo "Missing option 'dir'"
                    usage
                elif [[ ${ARGS[i]} == vol-* ]]; then
                    VOLUME_ID=${ARGS[i]}
                else
                    echo "Unrecognized parameter '${ARGS[i]}', the volume-id should start with 'vol-'"
                    usage
                fi;;
            * )
                echo "Unrecognized option ${ARGS[i]}"
                usage
        esac
    done
    #read dir parameter from user input
    DIR=${ARGS[$# - 1]}
    

}

parse_command $@
if [ ! -z $EXITDIRECTLY ]; then
    
    exit 0
fi
validate_dir
echo "METHOD: "$METHOD
echo "DIR: "$DIR
echo "DEFAULT ATTACH POINT: "$ATTACH_POINT
if [ ! -z $VOLUME_ID ]; then
    
    echo "VOLUME_ID: "$VOLUME_ID
fi
create_instance

echo "HOST NAME: "$HOST_NAME

visual_sleep validate_ssh_connection 45
if [ -z $VOLUME_ID ]; then #not given the volume id, program should create one
    create_volume
fi

attach_volume

if [ $METHOD = "dd" ]; then
	#statements
	dd_backup $DIR
else
		rsync_backup $DIR
fi
echo "Goodbye!"
exit 0

