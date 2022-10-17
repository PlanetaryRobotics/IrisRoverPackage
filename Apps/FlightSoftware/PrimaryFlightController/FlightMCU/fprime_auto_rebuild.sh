# First thing, *immediately* grab the directory containing this file (not where it's being called from):
SOURCE=${BASH_SOURCE[0]}
while [ -L "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )
  SOURCE=$(readlink "$SOURCE")
  [[ $SOURCE != /* ]] && SOURCE=$DIR/$SOURCE # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRIPT_DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )

# Instructions before running:
echo -e "\033[1;37;44m MAKE SURE YOU COMMIT ALL UNSAVED GIT CHANGES FIRST! \033[0m"
echo "If you haven't, please don't continue."
echo -e "\033[1;37;44m Press 'y' to confirm all git changes are committed and continue . . . \033[0m"
read -r -n 1 -s answer
if [[ $answer = [Yy] ]]; then

	echo -e "Building FPrime. "
	echo -e "\033[1;37;44m Note: \033[0m Only abbreviated outputs will be shown. Full outputs can be found in 'fprime_build_full_outputs'."
	echo -e "\033[1;37;44m Note: \033[0m Some errors will be shown. Only error codes that matter are any that indicate that an \033[1mentire file\033[0m failed to build (there are plenty of places where it doesn't like a line because a patch hasn't been applied yet)."

	echo -e "\033[1;37;44m  \033[0m"
	echo -e "\033[1;37;44m Performing Setup . . . \033[0m"
	echo -e "\033[1;37;44m  \033[0m"

	# Save absolute path to starting directory:
	STARTING_DIR=$(pwd)
	# Create outputs dir:
	OUTPUTS_DIR=$SCRIPT_DIR/fprime_build_full_outputs
	# Setup common grep search string:
	GREP_SEARCH_STR="Building\|[Cc]omplete\|[Ee]rror[^_=]\|Fail\|[Ff]ailure\|[Ww]arning[^s_=]\|291\|136"
	# Lines for grep to exclude (known "not a problem" errors):
	GREP_EXCLUDE_STR="[Ee]rror 127\|[Ee]rror 2\|error #291\|error #136\|error #249"
	# Make sure output directory exists
	sudo mkdir $OUTPUTS_DIR > /dev/null 2>&1
	# Make sure all the appropriate pre-reqs are installed:
	sudo apt-get install python-lxml | grep --color=always "[Ee]rr\|[Ff]ail\|[Ww]arn"
	sudo apt-get install python-cheetah | grep --color=always "[Ee]rr\|[Ff]ail\|[Ww]arn"



	# Go to FPrime:
	cd $SCRIPT_DIR/../../fprime


	echo -e "\033[1;37;44m  \033[0m"
	echo -e "\033[1;37;44m CONFIGURING defines_common . . . \033[0m"
	echo -e "\033[1;37;44m  \033[0m"
	set -x
	sudo git config --global --add safe.directory $PWD/../../..
	sudo ./configure.sh
	{ set +x; } 2>/dev/null


	echo " "
	echo -e "\033[1;37;44m  \033[0m"
	echo -e "\033[1;37;44m Rebuilding Libraries . . . \033[0m"
	echo -e "\033[1;37;44m  \033[0m"
	set -x


	# Remove old libs directory:
	sudo rm -rf ./Libs


	# Generate Python FPrime build files:
	cd ./Os

	{ set +x; } 2>/dev/null
	echo "** sudo make gen_make"
	sudo make gen_make 2>&1 | sudo tee "${OUTPUTS_DIR}/Os_make_gen_make__out.txt" | grep -v "${GREP_EXCLUDE_STR}" | grep --color=always "${GREP_SEARCH_STR}"
	set -x

	cd ../CubeRover

	{ set +x; } 2>/dev/null
	echo "** sudo make gen_make"
	sudo make gen_make 2>&1 | sudo tee "${OUTPUTS_DIR}/CubeRover_make_gen_make__out.txt" | grep -v "${GREP_EXCLUDE_STR}" | grep --color=always "${GREP_SEARCH_STR}"
	set -x

	# Actually build the libraries:
	cd ..

	{ set +x; } 2>/dev/null
	echo "** sudo ./build_libs.sh"
	sudo ./build_libs.sh 2>&1 | sudo tee "${OUTPUTS_DIR}/build_libs__out.txt" | grep -v "${GREP_EXCLUDE_STR}" | grep --color=always "${GREP_SEARCH_STR}"


	echo -e "\033[1;37;44m  \033[0m"
	echo -e "\033[1;37;44m Installing Libraries . . . \033[0m"
	echo -e "\033[1;37;44m  \033[0m"
	echo "++ sudo ./install_libs"
	sudo ./install_libs.sh 2>&1 | sudo tee "${OUTPUTS_DIR}/install_libs__1__out.txt"


	echo -e "\033[1;37;44m  \033[0m"
	echo -e "\033[1;37;44m Building CubeRover . . . \033[0m"
	echo -e "\033[1;37;44m  \033[0m"
	set -x
	cd ./CubeRover

	{ set +x; } 2>/dev/null
	echo "** sudo ./clean.sh"
	sudo ./clean.sh 2>&1 | sudo tee "${OUTPUTS_DIR}/CubeRover_clean__out.txt" | grep -v "${GREP_EXCLUDE_STR}" | grep --color=always "${GREP_SEARCH_STR}"
	set -x

	{ set +x; } 2>/dev/null
	echo "** sudo ./build.sh"
	echo -e "\033[1;37;45m NOTE: It's okay and expected for Topology.o to fail to build *once* (this is before a patch is applied). Twice is a problem. \033[0m"
	sudo ./build.sh 2>&1 | sudo tee "${OUTPUTS_DIR}/CubeRover_build__out.txt" | grep -v "${GREP_EXCLUDE_STR}" | grep --color=always "${GREP_SEARCH_STR}"
	set -x

	cd ..
	{ set +x; } 2>/dev/null

	echo -e "\033[1;37;44m  \033[0m"
	echo -e "\033[1;37;44m Redoing Library Install . . . \033[0m"
	echo -e "\033[1;37;44m  \033[0m"
	echo "** sudo ./install_libs"
	sudo ./install_libs.sh 2>&1 | sudo tee "${OUTPUTS_DIR}/install_libs__2__out.txt"


	# Return Home:
	cd $STARTING_DIR


	# Closing Message:
	echo -e " "
	echo -e "\033[1;37;42m  \033[0m"
	echo -e "\033[1;37;42m FPrime Rebuild Done! \033[0m"
	echo -e "\033[1;37;42m  \033[0m"
	echo -e "Check the above output for any errors that indicate that an \033[1mentire file\033[0m failed to build (there are plenty of places where it doesn't like a line because a patch hasn't been applied yet)."
	echo -e " * If there was a problem, look at the corresponding log file in the \033[1mfprime_build_full_outputs\033[0m folder for more details and consult \033[1msetup_new_fprime_branch__or_fresh_install.md\033[0m for insights on some common problems."
	echo -e " * If there was only 1 error about compiling \033[1m'Topology.o'\033[0m, that's fine - it's from before a patch is applied. It gets redone. \033[1mIf there are 2 such errors, then there's a problem.\033[0m"

fi

