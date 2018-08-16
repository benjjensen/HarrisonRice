#!/bin/bash

cd /cgroup/cpuset

/bin/echo 0-1 > libvirt/lxc/cpuset.cpus
/bin/echo 0-1 > libvirt/qemu/cpuset.cpus
/bin/echo 0-1 > libvirt/cpuset.cpus

mkdir sys
/bin/echo 0-1 > sys/cpuset.cpus

/bin/echo 0 > sys/cpuset.mems

mkdir acquire
/bin/echo 2-3 > acquire/cpuset.cpus

/bin/echo 1 > acquire/cpuset.cpu_exclusive
/bin/echo 0 > acquire/cpuset.mems
/bin/echo 1 > acquire/cpuset.sched_load_balance
/bin/echo 1 > acquire/cpuset.mem_hardwall

for T in `cat tasks`; do
	/bin/echo $T > sys/tasks;
done

exit 0
