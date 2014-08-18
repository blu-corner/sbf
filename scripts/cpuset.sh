#!/bin/sh

do_mkdir() {
	mkdir -p $1
	chgrp -R wheel $1
	chmod -R g+w $1
}

add_usage() {
	echo "usage: $0 add set processor-list memory-list" >&2
	exit 1
}

remove_usage() {
	echo "usage: $0 remove set" >&2
	exit 1
}

remove_all_usage() {
	echo "usage: $0 remove-all" >&2
	exit 1
}

move_usage() {
	echo "usage: $0 move set process" >&2
	exit 1
}

move_all_usage() {
	echo "usage: $0 move-all set" >&2
	exit 1
}

list_usage() {
	echo "usage: $0 list" >&2
	exit 1
}

usage() {
	echo "usage: $0 [add|remove|remove-all|move|move-all|run|list] [arguments]" >&2
	exit 1;
}

need_root() {
	if [ "$(id -u)" != "0" ]; then
		echo $1 >&2
		exit 1
	fi
}

if ! fgrep -q '/cpusets' /proc/mounts; then
	need_root "must be root to mount /cpusets"
	do_mkdir /cpusets
	mount -tcpuset cpusets /cpusets
	do_mkdir /cpusets
fi

[ -n "$1" ] || usage
COMMAND=$1
shift

if [ "$COMMAND" != "remove-all" -a "$COMMAND" != "list" ]; then
	[ -n "$1" ] || usage
	SET=/cpusets/$1
	[ "$1" = "root" ] && SET=/cpusets
	shift
fi

case "$COMMAND" in
	add)
		# Add a set.
		[ -n "$1" ] || add_usage
		[ -n "$2" ] || add_usage
		[ -z "$3" ] || add_usage
		do_mkdir $SET
		echo $1 >$SET/cpus
		echo $2 >$SET/mems
		;;
	remove)
		# Remove a set.
		[ -z "$1" ] || remove_usage
		rmdir $SET
		;;
	remove-all)
		# Remove all sets.
		[ -z "$1" ] || remove_all_usage
		need_root "must be root for remove-all"
		for i in $(ps -Leopid); do
			[ ! -f /proc/$i/exe ] && continue
			echo $i >>/cpusets/tasks
		done
		for i in /cpusets/*; do
			[ -d $i ] && rmdir $i
		done
		;;
	move)
		# Move a single task.
		[ -n "$1" ] || move_usage
		[ -z "$2" ] || move_usage
		echo $1 >>$SET/tasks
		;;
	move-all)
		# Move all tasks.
		[ -z "$1" ] || move_all_usage
		need_root "must be root for move-all"
		for i in $(ps -Heopid); do
			[ ! -f /proc/$i/exe ] && continue
			echo $i >>$SET/tasks
		done
		;;
	run)
		# Run a command.
		echo $$ >>$SET/tasks
		exec "$@"
		;;
	list)
		# List all sets.
		[ -z "$1" ] || list_usage
		printf "%-20s %-5s %-3s %s\n" "" "C" "M" "TASKS"
		for i in /cpusets/*; do
			[ ! -d $i ] && continue
			printf "%-20s %-5s %-3s %s\n" \
				$(basename $i) $(cat $i/cpus) $(cat $i/mems) \
				"$(wc -l $i/tasks|cut -d\  -f1)"
		done
		;;
	*)
		usage
		;;
esac
exit 0
