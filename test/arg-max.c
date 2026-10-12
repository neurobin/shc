/* Exercise the real reader with controlled sysconf results. */
#define main shc_main
#define sysconf test_sysconf
#include "../src/shc.c"
#undef sysconf
#undef main
#include <limits.h>

static long test_limit;
static int test_errno;

long test_sysconf(int name)
{
	assert(name == _SC_ARG_MAX);
	errno = test_errno;
	return test_limit;
}

int main(int argc, char **argv)
{
	char *script;

	if (argc != 3)
		return 2;
	if (!strcmp(argv[1], "large"))
		test_limit = LONG_MAX;
	else if (!strcmp(argv[1], "reported"))
		/* 4611686018427387903 on a system with 64-bit long. */
		test_limit = LONG_MAX / 2;
	else if (!strcmp(argv[1], "unbounded"))
		test_limit = -1;
	else if (!strcmp(argv[1], "error")) {
		test_limit = -1;
		test_errno = EINVAL;
	} else if (!strcmp(argv[1], "zero"))
		test_limit = 0;
	else if (!strcmp(argv[1], "below"))
		test_limit = 1000;
	else if (!strcmp(argv[1], "boundary"))
		test_limit = 125;
	else if (!strcmp(argv[1], "above"))
		test_limit = 124;
	else
		return 2;

	script = read_script(argv[2]);
	if (!script)
		return 1;
	fputs(script, stdout);
	free(script);
	return 0;
}
