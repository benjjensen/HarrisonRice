#!/bin/bash

cd /cgroup/cpuset

echo 0-1 > libvirt/lxc/cpuset.cpus
echo 0-1 > libvirt/qemu/cpuset.cpus
echo 0-1 > libvirt/cpuset.cpus

mkdir -p sys
if [ $? -ne 0 ]
then
    exit 1
fi
mkdir acquire
if [ $? -ne 0 ]
then
    rmdir sys
    exit 1
fi

echo 0-1 > sys/cpuset.cpus

echo 0 > sys/cpuset.mems

echo 2-3 > acquire/cpuset.cpus

echo 1 > acquire/cpuset.cpu_exclusive
echo 0 > acquire/cpuset.mems
echo 1 > acquire/cpuset.sched_load_balance
echo 1 > acquire/cpuset.mem_hardwall

for T in `cat tasks`;
    do
        echo $T > sys/tasks;
done

exit 0
