#!/bin/bash


print_help() {
	echo "Usage: $script_name [-a | --all] [-h | --help] [-n | --dry-run] [-v | --verbose] [header file]"
	echo ''
	echo 'This script fixes include guards in the project.'
	echo "The guards are fixed to be in compliance with clang-tidy's llvm-header-guard check."
	echo 'Example: The include guard for header'
	echo '    pigpio-remote/include/pigpio-remote/PiConnection.h'
	echo 'is'
	echo '    PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_PICONNECTION_H'
	echo ''
	echo 'By default, only the include guards passed as argument are fixed.'
	echo 'If the flag -a or --all is used, all include guards in the project are fixed.'
	echo ''
	echo 'Options:'
	echo '-h, --help: Prints this help text.'
	echo '-v, --verbose: Print extra info while renaming.'
	echo '-n, --dry-run: Only print what header guards would be placed without actually editing.'
	echo '-a, --all:  Fix all include guards in the project. Supercedes any given individual header file.'
}

print_verbose()
{
	if [ "$verbose" = true ]; then
		echo "$@";
	fi
}

all=false
script_name=`basename $0`
verbose=false
dry_run=false
filenames=( );

# Parse args
for i in "$@"; do
	case $i in
		-n|--dry-run)
			dry_run=true
			shift
			;;
		-v|--verbose)
			verbose=true
			shift
			;;
		-a|--all)
			all=true
			shift
			;;
		-h|--help)
			print_help
			exit
			;;
		*)
			# only add non empty strings
			[ -z "$1" ] || filenames+=("$1");
			shift
			;;
	esac
done

# Get git root
git rev-parse
exit_code=$?
if [ $exit_code -eq 128 ]; then
	>&2 echo 'Not in a git repository. You must be inside the git repository to run this script.';
	exit
elif [ $exit_code -ne 0 ]; then
	>&2 echo 'Unkown error, something is wrong with git:'
	>&2 echo "git rev-parse returned exit code $exit_code"
	exit
fi

git_root=`git rev-parse --show-toplevel`
print_verbose "Git root directory is $git_root."

if [ "$all" = true ]; then
	print_verbose "Flag --all is set. Finding all headers."
	filenames=()
	while IFS=  read -r -d $'\0'; do
		filenames+=("$REPLY")
	done < <(find "$git_root" -path ./external -prune -o -type f -name "*.h" -print0)
	print_verbose "Headers:"
	[ "$verbose" = true ] && printf '%s\n' "${filenames[@]}"
	print_verbose ""
fi

fix_header() {
	file_path=`realpath "$1"`
	print_verbose "Fixing file $file_path now."
	header_guard=`sed "s:^$git_root:pigpio-remote:" <<< "$file_path"`
	header_guard=`echo "$header_guard" | sed 's/.*/\U\0\E/' | sed 's:[-_/.]:_:g'`
	print_verbose "Header guard $header_guard."
	if [ "$dry_run" = false ]; then
		sed -i '/#ifndef/,/#define/ s/ .*$'"/ $header_guard/" "$file_path"
		sed -i "$(sed -n '/#endif/ =' $file_path | tail -n 1)"' s: .*$:'" // $header_guard:" "$file_path"
	else
		echo "Setting header guard '$header_guard' for file '$file_path'."
	fi
}

for file in "${filenames[@]}"; do
	fix_header "$file"
done

