#!/bin/bash
# delete all aws ec2 volumes that are available in your default region
v=`aws ec2 describe-volumes --filters Name=status,Values=available|grep VolumeId|cut -d: -f2|cut -d\" -f2`
for i in $v; do
	`aws ec2 delete-volume --volume-id $i &`
done