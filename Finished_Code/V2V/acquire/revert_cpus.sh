#!/bin/bash

cd /cgroup/cpuset

# move tasks back from sys-cpuset to root cpuset
for T in `cat sys/tasks`;
    do
	echo $T > tasks;
done
# try again
for T in `cat sys/tasks`;
    do
	echo $T > tasks;
done
rmdir sys

# move tasks back from rt-cpuset to root cpuset
for T in `cat acquire/tasks`;
    do
	echo $T > tasks;
done
# try again
for T in `cat acquire/tasks`;
    do
	echo $T > tasks;
done
rmdir acquire

echo 0-3 > libvirt/cpuset.cpus
echo 0-3 > libvirt/lxc/cpuset.cpus
echo 0-3 > libvirt/qemu/cpuset.cpus

exit 0
