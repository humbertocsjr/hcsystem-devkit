/*
 *	NMH's Simple C Compiler, 2013,2014
 *	DOS/8086 environment
 */

#define OS		"DOS"
#define AOUTNAME	"aout.exe"
#define ASCMD		"i86-as -o %s %s"
#define LDCMD		"i86-ld -f com -o %s %s/lib/%scrt0.o"
#define SYSLIBC		""
