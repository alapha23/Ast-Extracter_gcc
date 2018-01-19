FILENAME=$1
SCOPENAME=$2
foo=".001t.tu"
# yell() { echo "$0: $*" >&2; }
# die() { yell "$*"; exit 111; }
# try() { "$@" || die "cannot $*"; }

rm tu_eater *.*.*

if [ -z "$SCOPENAME" ]; then
	echo "Usage: bash ./gen_ast.sh <file name> <scope name>"
	echo "test files are supposed to be placed under ./test"
	echo "example: bash ./gen_ast.sh test/helloworld.c main"
	exit 1;
fi

gcc -o tu_eater src/tu_eater.c src/tu_eater.h
gcc -std=c99 -fdump-translation-unit -fno-builtin -ffreestanding -c  $FILENAME 
./tu_eater "${FILENAME:5}$foo" $SCOPENAME > result/ast.dot
dot -Tpdf -o result/ast.pdf result/ast.dot 
atril result/ast.pdf &

rm *.o #*.*.* tu_eater
unset FILENAME SCOPENAME

# fno-builtin: Don't recognize built-in functions that do not begin with __builtin_ as prefix.
#
# ffreestanding: Assert that compilation targets a freestanding environment.  This
#        implies -fno-builtin.  A freestanding environment is one in which
#        the standard library may not exist, and program startup may not
#        necessarily be at "main".  The most obvious example is an OS
#        kernel.  This is equivalent to -fno-hosted.
#
# fdump-translation-unit-options (C++ only)
#        Dump a representation of the tree structure for the entire
#        translation unit to a file.

