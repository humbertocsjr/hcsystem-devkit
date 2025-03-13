/*
 *	NMH's Simple C Compiler, 2013,2014
 *	DOS/8086 environment
 */

#define OS		"DOS"
#define AOUTNAME	"aout.exe"
#define ASCMD		"i86-as -o %s %s"
#define DOSLDCMD		"i86-ld -f com -o %s "
#define HCSLDCMD		"i86-ld -f hcsys -o %s "
#define SYSLIBC		""
