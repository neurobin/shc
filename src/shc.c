/* shc.c */

/**
 * This software contains an ad hoc version of the 'Alleged RC4' algorithm,
 * which was anonymously posted on sci.crypt news by cypherpunks on Sep 1994.
 *
 * My implementation is a complete rewrite of the one found in
 * an unknown-copyright (283 characters) version picked up from:
 *    From: allen@gateway.grumman.com (John L. Allen)
 *    Newsgroups: comp.lang.c
 *    Subject: Shrink this C code for fame and fun
 *    Date: 21 May 1996 10:49:37 -0400
 * And it is licensed also under GPL.
 *
 *That's where I got it, now I am going to do some work on it
 *It will reside here: http://github.com/neurobin/shc
 */

static const char my_name[] = "shc";
static const char version[] = "Version 4.0.1";
static const char subject[] = "Generic Shell Script Compiler";
static const char cpright[] = "GNU GPL Version 3";
static const struct { const char * f, * s, * e; }
	provider = { "Md Jahidul", "Hamid", "<jahidulhamid@yahoo.com>" };          

/* 
static const struct { const char * f, * s, * e; }
	author = { "Francisco", "Garcia", "<frosal@fi.upm.es>" };
*/
/*This is the original author who first came up with this*/

static const char * copying[] = {
"Copying:",
"",
"    This program is free software; you can redistribute it and/or modify",
"    it under the terms of the GNU General Public License as published by",
"    the Free Software Foundation; either version 3 of the License, or",
"    (at your option) any later version.",
"",
"    This program is distributed in the hope that it will be useful,",
"    but WITHOUT ANY WARRANTY; without even the implied warranty of",
"    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
"    GNU General Public License for more details.",
"",
"    You should have received a copy of the GNU General Public License",
"    along with this program; if not, write to the Free Software",
"    @Neurobin, Dhaka, Bangladesh",
"",
"    Report problems and questions to:http://github.com/neurobin/shc",
"",
0};

static const char * abstract[] = {
"Abstract:",
"",
"    This tool generates a stripped binary executable version",
"    of the script specified at command line.",
"",
"    Binary version will be saved with a .x extension by default.",
"    You can specify output file name too with [-o filname] option.",
"",
"    You can specify expiration date [-e] too, after which binary will",
"    refuse to be executed, displaying \"[-m]\" instead.",
"",
"    You can compile whatever interpreted script, but valid [-i], [-x]",
"    and [-l] options must be given.",
"",
0};

static const char usage[] = 
"Usage: shc [-e date] [-m addr] [-i iopt] [-x cmnd] [-l lopt] [-o outfile] [-rvDSUHCABhs] -f script";

static const char * help[] = {
"",
"    -e %s  Expiration date in dd/mm/yyyy format [none]",
"    -m %s  Message to display upon expiration [\"Please contact your provider\"]",
"    -f %s  File name of the script to compile",
"    -i %s  Inline option for the shell interpreter i.e: -e",
"    -x %s  eXec command, as a printf format i.e: exec('%s',@ARGV);",
"    -l %s  Last shell option i.e: --",
"    -o %s  output filename",
"    -r     Relax security. Make a redistributable binary",
"    -v     Verbose compilation",
"    -S     Switch ON setuid for root callable programs [OFF]",
"    -D     Switch ON debug exec calls [OFF]",
"    -U     Make binary untraceable [no]",
"    -H     Hardening : extra security protection [no]",
"           untraceable, undumpable, etc. and root is not required",
"           * currently only works with bourne shell (sh)",
"           * does not work with positional parameters",
"    -s     Hardening : use a single process (no child) [no]",
"           option available only with -H otherwise its ignored",
"           experimental feature may hang...",
"           * currently only works with bourne shell (sh)",
"           * does not work with positional parameters",
"    -C     Display license and exit",
"    -A     Display abstract and exit",
"    -B     Compile for busybox",
"    -h     Display help and exit",
"",
"    Environment variables used:",
"    Name    Default  Usage",
"    CC      cc       C compiler command",
"    CFLAGS  <none>   C compiler flags",
"    LDFLAGS <none>   Linker flags",
"",
"    Please consult the shc man page.",
"",
0};

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SIZE 4096

static char * file;
static char * file2;
static char   date[21];
static char * mail = "Please contact your provider jahidulhamid@yahoo.com";
static char   rlax[1];
static char * shll;
static char * inlo;
static char * xecc;
static char * lsto;
static char * opts;
static char * text;
static int verbose;
static const char SETUID_line[] =
"#define SETUID %d	/* Define as 1 to call setuid(0) at start of script */\n";
static int SETUID_flag = 0;
static const char DEBUGEXEC_line[] =
"#define DEBUGEXEC	%d	/* Define as 1 to debug execvp calls */\n";
static int DEBUGEXEC_flag = 0;
static const char TRACEABLE_line[] =
"#define TRACEABLE	%d	/* Define as 1 to enable ptrace the executable */\n";
static int TRACEABLE_flag = 1;
static const char HARDENING_line[] =
"#define HARDENING	%d	/* Define as 1 to disable ptrace/dump the executable */\n";
static int HARDENING_flag = 0;
static const char HARDENINGSP_line[] =
"#define HARDENINGSP	%d	/* Define as 1 to disable bash child process */\n";
static int HARDENINGSP_flag = 0;
static const char BUSYBOXON_line[] =
"#define BUSYBOXON	%d	/* Define as 1 to enable work with busybox */\n";
static int BUSYBOXON_flag = 0;

static const char * RTC[] = {
"",
"/* rtc.c */",
"",
"#include <sys/stat.h>",
"#include <sys/types.h>",
"",
"#include <errno.h>",
"#include <stdio.h>",
"#include <stdlib.h>",
"#include <string.h>",
"#include <time.h>",
"#include <unistd.h>",
"",
"/* 'Alleged RC4' */",
"",
"static unsigned char stte[256], indx, jndx, kndx;",
"",
"/*",
" * Reset arc4 stte. ",
" */",
"void stte_0(void)",
"{",
"	indx = jndx = kndx = 0;",
"	do {",
"		stte[indx] = indx;",
"	} while (++indx);",
"}",
"",
"/*",
" * Set key. Can be used more than once. ",
" */",
"void key(void * str, int len)",
"{",
"	unsigned char tmp, * ptr = (unsigned char *)str;",
"	while (len > 0) {",
"		do {",
"			tmp = stte[indx];",
"			kndx += tmp;",
"			kndx += ptr[(int)indx % len];",
"			stte[indx] = stte[kndx];",
"			stte[kndx] = tmp;",
"		} while (++indx);",
"		ptr += 256;",
"		len -= 256;",
"	}",
"}",
"",
"/*",
" * Crypt data. ",
" */",
"void arc4(void * str, int len)",
"{",
"	unsigned char tmp, * ptr = (unsigned char *)str;",
"	while (len > 0) {",
"		indx++;",
"		tmp = stte[indx];",
"		jndx += tmp;",
"		stte[indx] = stte[jndx];",
"		stte[jndx] = tmp;",
"		tmp += stte[indx];",
"		*ptr ^= stte[tmp];",
"		ptr++;",
"		len--;",
"	}",
"}",
"",
"/* End of ARC4 */",
"",
"#if HARDENING",
"",
"#include <sys/ptrace.h>",
"#include <sys/wait.h>",
"#include <signal.h>",
"#include <sys/prctl.h>",
"#define PR_SET_PTRACER 0x59616d61",
"",
"/* Seccomp Sandboxing Init */",
"#include <stdlib.h>",
"#include <stdio.h>",
"#include <stddef.h>",
"#include <string.h>",
"#include <unistd.h>",
"#include <errno.h>",
"",
"#include <sys/types.h>",
"#include <sys/prctl.h>",
"#include <sys/syscall.h>",
"#include <sys/socket.h>",
"",
"#include <linux/filter.h>",
"#include <linux/seccomp.h>",
"#include <linux/audit.h>",
"",
"#define ArchField offsetof(struct seccomp_data, arch)",
"",
"#define Allow(syscall) \\",
"    BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, SYS_##syscall, 0, 1), \\",
"    BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW)",
"",
"struct sock_filter filter[] = {",
"    /* validate arch */",
"    BPF_STMT(BPF_LD+BPF_W+BPF_ABS, ArchField),",
"    BPF_JUMP( BPF_JMP+BPF_JEQ+BPF_K, AUDIT_ARCH_X86_64, 1, 0),",
"    BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_KILL),",
"",
"    /* load syscall */",
"    BPF_STMT(BPF_LD+BPF_W+BPF_ABS, offsetof(struct seccomp_data, nr)),",
"",
"    /* list of allowed syscalls */",
"    Allow(exit_group),  /* exits a processs */",
"    Allow(brk),         /* for malloc(), inside libc */",
"    Allow(mmap),        /* also for malloc() */",
"    Allow(munmap),      /* for free(), inside libc */",
"",
"    /* and if we don't match above, die */",
"    BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_KILL),",
"};",
"struct sock_fprog filterprog = {",
"    .len = sizeof(filter)/sizeof(filter[0]),",
"    .filter = filter",
"};",
"",
"/* Seccomp Sandboxing - Set up the restricted environment */",
"void seccomp_hardening() {",
"    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {",
"        perror(\"Could not start seccomp:\");",
"        exit(1);",
"    }",
"    if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &filterprog) == -1) {",
"        perror(\"Could not start seccomp:\");",
"        exit(1);",
"    }",
"} ",
"/* End Seccomp Sandboxing Init */",
"",
"void arc4_hardrun(void * str, int len) {",
"    //Decode locally",
"    char tmp2[len];",
"    memcpy(tmp2, str, len);",
"",
"	unsigned char tmp, * ptr = (unsigned char *)tmp2;",
"",
"    int lentmp = len;",
"",
"#if HARDENINGSP",
"    //Start tracing to protect from dump & trace",
"    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {",
"        printf(\"Operation not permitted\\n\");",
"        kill(getpid(), SIGKILL);",
"        exit(1);",
"    }",
"",     
"    //Decode Bash",
"    while (len > 0) {",
"        indx++;",
"        tmp = stte[indx];",
"        jndx += tmp;",
"        stte[indx] = stte[jndx];",
"        stte[jndx] = tmp;",
"        tmp += stte[indx];",
"        *ptr ^= stte[tmp];",
"        ptr++;",
"        len--;",
"    }",
"",
"    //Exec bash script",
"    system(tmp2);",
"",
"    //Empty script variable",
"    memcpy(tmp2, str, lentmp);",
"",
"    //Sinal to detach ptrace",
"    ptrace(PTRACE_DETACH, 0, 0, 0);",
"    exit(0);",
"",
"    /* Seccomp Sandboxing - Start */",
"    seccomp_hardening();",
"",
"    exit(0);",
"#endif /* HARDENINGSP Exit here anyway*/",
"",
"    int pid, status;",
"    pid = fork();",
"",     
"    if(pid==0) {",
"",
"        //Start tracing to protect from dump & trace",
"        if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {",
"            printf(\"Operation not permitted\\n\");",
"            kill(getpid(), SIGKILL);",
"            _exit(1);",
"        }",
"",     
"        //Decode Bash",
"        while (len > 0) {",
"            indx++;",
"            tmp = stte[indx];",
"            jndx += tmp;",
"            stte[indx] = stte[jndx];",
"            stte[jndx] = tmp;",
"            tmp += stte[indx];",
"            *ptr ^= stte[tmp];",
"            ptr++;",
"            len--;",
"        }",
"",
"        //Exec bash script",
"        system(tmp2);",
"",
"        //Empty script variable",
"        memcpy(tmp2, str, lentmp);",
"",
"        //Sinal to detach ptrace",
"        ptrace(PTRACE_DETACH, 0, 0, 0);",
"        exit(0);",
"    }",
"    else {",
"        wait(&status);",
"    }",
"",
"    /* Seccomp Sandboxing - Start */",
"    seccomp_hardening();",
"",
"    exit(0);",
"} ",
"#endif /* HARDENING */",
"",
"/*",
" * Key with file invariants. ",
" */",
"int key_with_file(char * file)",
"{",
"	struct stat statf[1];",
"	struct stat control[1];",
"",
"	if (stat(file, statf) < 0)",
"		return -1;",
"",
"	/* Turn on stable fields */",
"	memset(control, 0, sizeof(control));",
"	control->st_ino = statf->st_ino;",
"	control->st_dev = statf->st_dev;",
"	control->st_rdev = statf->st_rdev;",
"	control->st_uid = statf->st_uid;",
"	control->st_gid = statf->st_gid;",
"	control->st_size = statf->st_size;",
"	control->st_mtime = statf->st_mtime;",
"	control->st_ctime = statf->st_ctime;",
"	key(control, sizeof(control));",
"	return 0;",
"}",
"",
"#if DEBUGEXEC",
"void debugexec(char * sh11, int argc, char ** argv)",
"{",
"	int i;",
"	fprintf(stderr, \"shll=%s\\n\", sh11 ? sh11 : \"<null>\");",
"	fprintf(stderr, \"argc=%d\\n\", argc);",
"	if (!argv) {",
"		fprintf(stderr, \"argv=<null>\\n\");",
"	} else { ",
"		for (i = 0; i <= argc ; i++)",
"			fprintf(stderr, \"argv[%d]=%.60s\\n\", i, argv[i] ? argv[i] : \"<null>\");",
"	}",
"}",
"#endif /* DEBUGEXEC */",
"",
"void rmarg(char ** argv, char * arg)",
"{",
"	for (; argv && *argv && *argv != arg; argv++);",
"	for (; argv && *argv; argv++)",
"		*argv = argv[1];",
"}",
"",
"void chkenv_end(void);",
"",
"int chkenv(int argc)",
"{",
"	char buff[512];",
"	unsigned long mask, m;",
"	int l, a, c;",
"	char * string;",
"	extern char ** environ;",
"",
"	mask = (unsigned long)getpid();",
"	stte_0();",
"	 key(&chkenv, (void*)&chkenv_end - (void*)&chkenv);",
"	 key(&data, sizeof(data));",
"	 key(&mask, sizeof(mask));",
"	arc4(&mask, sizeof(mask));",
"	sprintf(buff, \"x%lx\", mask);",
"	string = getenv(buff);",
"#if DEBUGEXEC",
"	fprintf(stderr, \"getenv(%s)=%s\\n\", buff, string ? string : \"<null>\");",
"#endif",
"	l = strlen(buff);",
"	if (!string) {",
"		/* 1st */",
"		sprintf(&buff[l], \"=%lu %d\", mask, argc);",
"		putenv(strdup(buff));",
"		return 0;",
"	}",
"	c = sscanf(string, \"%lu %d%c\", &m, &a, buff);",
"	if (c == 2 && m == mask) {",
"		/* 3rd */",
"		rmarg(environ, &string[-l - 1]);",
"		return 1 + (argc - a);",
"	}",
"	return -1;",
"}",
"",
"void chkenv_end(void){}",
"",
"#if HARDENING",
"",
"static void gets_process_name(const pid_t pid, char * name) {",
"	char procfile[BUFSIZ];",
"	sprintf(procfile, \"/proc/%d/cmdline\", pid);",
"	FILE* f = fopen(procfile, \"r\");",
"	if (f) {",
"		size_t size;",
"		size = fread(name, sizeof (char), sizeof (procfile), f);",
"		if (size > 0) {",
"			if ('\\n' == name[size - 1])",
"				name[size - 1] = '\\0';",
"		}",
"		fclose(f);",
"	}",
"}",
"",
"void hardening() {",
"    prctl(PR_SET_DUMPABLE, 0);",
"    prctl(PR_SET_PTRACER, -1);",
"",
"    int pid = getppid();",
"    char name[256] = {0};",
"    gets_process_name(pid, name);",
"",
"    if (   (strcmp(name, \"bash\") != 0) ",
"        && (strcmp(name, \"/bin/bash\") != 0) ",
"        && (strcmp(name, \"sh\") != 0) ",
"        && (strcmp(name, \"/bin/sh\") != 0) ",
"        && (strcmp(name, \"sudo\") != 0) ",
"        && (strcmp(name, \"/bin/sudo\") != 0) ",
"        && (strcmp(name, \"/usr/bin/sudo\") != 0)",
"        && (strcmp(name, \"gksudo\") != 0) ",
"        && (strcmp(name, \"/bin/gksudo\") != 0) ",
"        && (strcmp(name, \"/usr/bin/gksudo\") != 0) ",
"        && (strcmp(name, \"kdesu\") != 0) ",
"        && (strcmp(name, \"/bin/kdesu\") != 0) ",
"        && (strcmp(name, \"/usr/bin/kdesu\") != 0) ",
"       )",
"    {",
"        printf(\"Operation not permitted\\n\");",
"        kill(getpid(), SIGKILL);",
"        exit(1);",
"    }",
"}",
"",
"#endif /* HARDENING */",
"",
"#if !TRACEABLE",
"",
"#define _LINUX_SOURCE_COMPAT",
"#include <sys/ptrace.h>",
"#include <sys/types.h>",
"#include <sys/wait.h>",
"#include <fcntl.h>",
"#include <signal.h>",
"#include <stdio.h>",
"#include <unistd.h>",
"",
"#if !defined(PT_ATTACHEXC) /* New replacement for PT_ATTACH */",
"   #if !defined(PTRACE_ATTACH) && defined(PT_ATTACH)",
"       #define PT_ATTACHEXC	PT_ATTACH",
"   #elif defined(PTRACE_ATTACH)",
"       #define PT_ATTACHEXC PTRACE_ATTACH",
"   #endif",
"#endif",
"",
"void untraceable(char * argv0)",
"{",
"	char proc[80];",
"	int pid, mine;",
"",
"	switch(pid = fork()) {",
"	case  0:",
"		pid = getppid();",
"		/* For problematic SunOS ptrace */",
"#if defined(__FreeBSD__)",
"		sprintf(proc, \"/proc/%d/mem\", (int)pid);",
"#else",
"		sprintf(proc, \"/proc/%d/as\",  (int)pid);",
"#endif",
"		close(0);",
"		mine = !open(proc, O_RDWR|O_EXCL);",
"		if (!mine && errno != EBUSY)",
"			mine = !ptrace(PT_ATTACHEXC, pid, 0, 0);",
"		if (mine) {",
"			kill(pid, SIGCONT);",
"		} else {",
/*"			fprintf(stderr, \"%s is being traced!\\n\", argv0);",*/
"			perror(argv0);",
"			kill(pid, SIGKILL);",
"		}",
"		_exit(mine);",
"	case -1:",
"		break;",
"	default:",
"		if (pid == waitpid(pid, 0, 0))",
"			return;",
"	}",
"	perror(argv0);",
"	_exit(1);",
"}",
"#endif /* !TRACEABLE */",
"",
"char * xsh(int argc, char ** argv)",
"{",
"	char * scrpt;",
"	int ret, i, j;",
"	char ** varg;",
"	char * me = argv[0];",
"	if (me == NULL) { me = getenv(\"_\"); }",
"	if (me == 0) { fprintf(stderr, \"E: neither argv[0] nor $_ works.\"); exit(1); }",
"",
"	ret = chkenv(argc);",
"	stte_0();",
"	 key(pswd, pswd_z);",
"	arc4(msg1, msg1_z);",
"	arc4(date, date_z);",
"	if (date[0] && (atoll(date)<time(NULL)))",
"		return msg1;",
"	arc4(shll, shll_z);",
"	arc4(inlo, inlo_z);",
"	arc4(xecc, xecc_z);",
"	arc4(lsto, lsto_z);",
"	arc4(tst1, tst1_z);",
"	 key(tst1, tst1_z);",
"	arc4(chk1, chk1_z);",
"	if ((chk1_z != tst1_z) || memcmp(tst1, chk1, tst1_z))",
"		return tst1;",
"	arc4(msg2, msg2_z);",
"	if (ret < 0)",
"		return msg2;",
"	varg = (char **)calloc(argc + 10, sizeof(char *));",
"	if (!varg)",
"		return 0;",
"	if (ret) {",
"		arc4(rlax, rlax_z);",
"		if (!rlax[0] && key_with_file(shll))",
"			return shll;",
"		arc4(opts, opts_z);",
"#if HARDENING",
"	    arc4_hardrun(text, text_z);",
"	    exit(0);",
"       /* Seccomp Sandboxing - Start */",
"       seccomp_hardening();",
"#endif",
"		arc4(text, text_z);",
"		arc4(tst2, tst2_z);",
"		 key(tst2, tst2_z);",
"		arc4(chk2, chk2_z);",
"		if ((chk2_z != tst2_z) || memcmp(tst2, chk2, tst2_z))",
"			return tst2;",
"		/* Prepend hide_z spaces to script text to hide it. */",
"		scrpt = malloc(hide_z + text_z);",
"		if (!scrpt)",
"			return 0;",
"		memset(scrpt, (int) ' ', hide_z);",
"		memcpy(&scrpt[hide_z], text, text_z);",
"	} else {			/* Reexecute */",
"		if (*xecc) {",
"			scrpt = malloc(512);",
"			if (!scrpt)",
"				return 0;",
"			sprintf(scrpt, xecc, me);",
"		} else {",
"			scrpt = me;",
"		}",
"	}",
"	j = 0;",
"#if BUSYBOXON",
"	varg[j++] = \"busybox\";",
"	varg[j++] = \"sh\";",
"#else",
"	varg[j++] = argv[0];		/* My own name at execution */",
"#endif",
"	if (ret && *opts)",
"		varg[j++] = opts;	/* Options on 1st line of code */",
"	if (*inlo)",
"		varg[j++] = inlo;	/* Option introducing inline code */",
"	varg[j++] = scrpt;		/* The script itself */",
"	if (*lsto)",
"		varg[j++] = lsto;	/* Option meaning last option */",
"	i = (ret > 1) ? ret : 0;	/* Args numbering correction */",
"	while (i < argc)",
"		varg[j++] = argv[i++];	/* Main run-time arguments */",
"	varg[j] = 0;			/* NULL terminated array */",
"#if DEBUGEXEC",
"	debugexec(shll, j, varg);",
"#endif",
"	execvp(shll, varg);",
"	return shll;",
"}",
"",
"int main(int argc, char ** argv)",
"{",
"#if SETUID",
"   setuid(0);",
"#endif",
"#if DEBUGEXEC",
"	debugexec(\"main\", argc, argv);",
"#endif",
"#if HARDENING",
"	hardening();",
"#endif",
"#if !TRACEABLE",
"	untraceable(argv[0]);",
"#endif",
"	argv[1] = xsh(argc, argv);",
"	fprintf(stderr, \"%s%s%s: %s\\n\", argv[0],",
"		errno ? \": \" : \"\",",
"		errno ? strerror(errno) : \"\",",
"		argv[1] ? argv[1] : \"<null>\"",
"	);",
"	return 1;",
"}",
0};

static int parse_an_arg(int argc, char * argv[])
{
	extern char * optarg;
	const char * opts = "e:m:f:i:x:l:o:rvDSUHCABhs";
	struct tm tmp[1];
	time_t expdate;
	int cnt, l;
	char ctrl;

	switch (getopt(argc, argv, opts)) {
	case 'e':
		memset(tmp, 0, sizeof(tmp));
		cnt = sscanf(optarg, "%2d/%2d/%4d%c",
			&tmp->tm_mday, &tmp->tm_mon, &tmp->tm_year, &ctrl);
		if (cnt == 3) {
			tmp->tm_mon--;
			tmp->tm_year -= 1900;
			expdate = mktime(tmp);
		}
		if (cnt != 3 || expdate <= 0) {
			fprintf(stderr, "%s parse(-e %s): Not a valid value\n",
				my_name,  optarg);
			return -1;
		}
		sprintf(date, "%lld", (long long)expdate);
		if (verbose) fprintf(stderr, "%s -e %s", my_name, ctime(&expdate));
		expdate = atoll(date);
		if (verbose) fprintf(stderr, "%s -e %s", my_name, ctime(&expdate));
		break;
	case 'm':
		mail = optarg;
		break;
	case 'f':
		if (file) {
			fprintf(stderr, "%s parse(-f): Specified more than once\n",
				my_name);
			return -1;
		}
		file = optarg;
		break;
	case 'i':
		inlo = optarg;
		break;
	case 'x':
		xecc = optarg;
		break;
	case 'l':
		lsto = optarg;
		break;
	case 'o':
		file2 = optarg;
		break;
	case 'r':
		rlax[0]++;
		break;
	case 'v':
		verbose++;
		break;
	case 'S':
		SETUID_flag = 1;
        break;
	case 'D':
		DEBUGEXEC_flag = 1;
		break;
	case 'U':
		TRACEABLE_flag = 0;
		break;
	case 'H':
		HARDENING_flag = 1;
		break;
	case 's':
        HARDENINGSP_flag = 1;
		break;
	case 'C':
		fprintf(stderr, "%s %s, %s\n", my_name, version, subject);
		fprintf(stderr, "%s %s %s %s %s\n", my_name, cpright, provider.f, provider.s, provider.e);
		fprintf(stderr, "%s ", my_name);
		for (l = 0; copying[l]; l++)
			fprintf(stderr, "%s\n", copying[l]);
		fprintf(stderr, "    %s %s %s\n\n", provider.f, provider.s, provider.e);
		exit(0);
		break;
	case 'A':
		fprintf(stderr, "%s %s, %s\n", my_name, version, subject);
		fprintf(stderr, "%s %s %s %s %s\n", my_name, cpright, provider.f, provider.s, provider.e);
		fprintf(stderr, "%s ", my_name);
		for (l = 0; abstract[l]; l++)
			fprintf(stderr, "%s\n", abstract[l]);
		exit(0);
		break;
	case 'h':
		fprintf(stderr, "%s %s, %s\n", my_name, version, subject);
		fprintf(stderr, "%s %s %s %s %s\n", my_name, cpright, provider.f, provider.s, provider.e);
		fprintf(stderr, "%s %s\n", my_name, usage);
		for (l = 0; help[l]; l++)
			fprintf(stderr, "%s\n", help[l]);
		exit(0);
		break;
	case -1:
		if (!file) {
			fprintf(stderr, "%s parse(-f): No source file specified\n", my_name);
			file = "";
			return -1;
		}
		return 0;
	case 'B':
		BUSYBOXON_flag = 1;
		break;
	case ':':
		fprintf(stderr, "%s parse: Missing parameter\n", my_name);
		return -1;
	case '?':
		fprintf(stderr, "%s parse: Unknown option\n", my_name);
		return -1;
	default:
		fprintf(stderr, "%s parse: Unknown return\n", my_name);
		return -1;
	}
	return 1;
}

static void parse_args(int argc, char * argv[])
{
	int err = 0;
	int ret;

#if 0
	my_name = strrchr(argv[0], '/');
	if (my_name)
		my_name++;
	else
		my_name = argv[0];
#endif

	do {
		ret = parse_an_arg(argc, argv);
		if (ret == -1)
			err++;
	} while (ret);
    
    if (HARDENING_flag == 0 && HARDENINGSP_flag == 1) {
        fprintf(stderr, "\n%s '-s' feature is only available with '-H'\n",my_name);
        err++;
    }
    
	if (err) {
		fprintf(stderr, "\n%s %s\n\n", my_name, usage);
		exit(1);
	}
}

/* 'Alleged RC4' */

static unsigned char stte[256], indx, jndx, kndx;

/*
 * Reset arc4 stte. 
 */
void stte_0(void)
{
	indx = jndx = kndx = 0;
	do {
		stte[indx] = indx;
	} while (++indx);
}

/*
 * Set key. Can be used more than once. 
 */
void key(void * str, int len)
{
	unsigned char tmp, * ptr = (unsigned char *)str;
	while (len > 0) {
		do {
			tmp = stte[indx];
			kndx += tmp;
			kndx += ptr[(int)indx % len];
			stte[indx] = stte[kndx];
			stte[kndx] = tmp;
		} while (++indx);
		ptr += 256;
		len -= 256;
	}
}

/*
 * Crypt data. 
 */
void arc4(void * str, int len)
{
	unsigned char tmp, * ptr = (unsigned char *)str;
	while (len > 0) {
		indx++;
		tmp = stte[indx];
		jndx += tmp;
		stte[indx] = stte[jndx];
		stte[jndx] = tmp;
		tmp += stte[indx];
		*ptr ^= stte[tmp];
		ptr++;
		len--;
	}
}

/* End of ARC4 */

/*
 * Key with file invariants.
 */
int key_with_file(char * file)
{
	struct stat statf[1];
	struct stat control[1];

	if (stat(file, statf) < 0)
		return -1;

	/* Turn on stable fields */
	memset(control, 0, sizeof(control));
	control->st_ino = statf->st_ino;
	control->st_dev = statf->st_dev;
	control->st_rdev = statf->st_rdev;
	control->st_uid = statf->st_uid;
	control->st_gid = statf->st_gid;
	control->st_size = statf->st_size;
	control->st_mtime = statf->st_mtime;
	control->st_ctime = statf->st_ctime;
	key(control, sizeof(control));
	return 0;
}

/*
 * NVI stands for Shells that complaint "Not Valid Identifier" on
 * environment variables with characters as "=|#:*?$ ".
 */
struct {
	char * shll;
	char * inlo;
	char * lsto;
	char * xecc;
} shellsDB[] = {
	{ "perl", "-e", "--", "exec('%s',@ARGV);" },
	{ "rc",   "-c", "",   "builtin exec %s $*" },
	{ "sh",   "-c", "",   "exec '%s' \"$@\"" }, /* IRIX_nvi */
	{ "dash", "-c", "",   "exec '%s' \"$@\"" },
	{ "bash", "-c", "",   "exec '%s' \"$@\"" },
	{ "zsh",  "-c", "",   "exec '%s' \"$@\"" },
	{ "bsh",  "-c", "",   "exec '%s' \"$@\"" }, /* AIX_nvi */
	{ "Rsh",  "-c", "",   "exec '%s' \"$@\"" }, /* AIX_nvi */
	{ "ksh",  "-c", "",   "exec '%s' \"$@\"" }, /* OK on Solaris, AIX and Linux (THX <bryan.hogan@dstintl.com>) */
	{ "tsh",  "-c", "--", "exec '%s' \"$@\"" }, /* AIX */
	{ "ash",  "-c", "--", "exec '%s' \"$@\"" }, /* Linux */
	{ "csh",  "-c", "-b", "exec '%s' $argv" }, /* AIX: No file for $0 */
	{ "tcsh", "-c", "-b", "exec '%s' $argv" },
	{ NULL,   NULL, NULL, NULL },
};

int eval_shell(char * text)
{
	int i;
	char * ptr;

	ptr = strchr(text, (int)'\n');
	if (!ptr)
		i = strlen(text);
	else
		i = ptr - text;
	ptr  = malloc(i + 1);
	shll = malloc(i + 1);
	opts = malloc(i + 1);
	if (!ptr || !shll || !opts)
		return -1;
	strncpy(ptr, text, i);
	ptr[i] = '\0';

	*opts = '\0';
	i = sscanf(ptr, " #!%s%s %c", shll, opts, opts);
	if (i < 1 || i > 2) {
		fprintf(stderr, "%s: invalid first line in script: %s\n", my_name, ptr);
		return -1;
	}
	free(ptr);

	shll = realloc(shll, strlen(shll) + 1);
	ptr  = strrchr(shll, (int)'/');
	if (*ptr == '/')
		ptr++;
	if (verbose) fprintf(stderr, "%s shll=%s\n", my_name, ptr);

	for(i=0; shellsDB[i].shll; i++) {
		if(!strcmp(ptr, shellsDB[i].shll)) {
			if (!inlo)
				inlo = strdup(shellsDB[i].inlo);
			if (!xecc)
				xecc = strdup(shellsDB[i].xecc);
			if (!lsto)
				lsto = strdup(shellsDB[i].lsto);
		}
	}
	if (!inlo || !xecc || !lsto) {
		fprintf(stderr, "%s Unknown shell (%s): specify [-i][-x][-l]\n", my_name, ptr);
		return -1;
	}
	if (verbose) fprintf(stderr, "%s [-i]=%s\n", my_name, inlo);
	if (verbose) fprintf(stderr, "%s [-x]=%s\n", my_name, xecc);
	if (verbose) fprintf(stderr, "%s [-l]=%s\n", my_name, lsto);

	opts = realloc(opts, strlen(opts) + 1);
	if (*opts && !strcmp(opts, lsto)) {
		fprintf(stderr, "%s opts=%s : Is equal to [-l]. Removing opts\n", my_name, opts);
		*opts = '\0';
	} else if (!strcmp(opts, "-")) {
		fprintf(stderr, "%s opts=%s : No real one. Removing opts\n", my_name, opts);
		*opts = '\0';
	}
	if (verbose) fprintf(stderr, "%s opts=%s\n", my_name, opts);
	return 0;
}

char * read_script(char * file)
{
	FILE * i;
	char * text;
	int cnt, l;

	text = malloc(SIZE);
	if (!text)
		return NULL;
	i = fopen(file, "r");
	if (!i)
		return NULL;
	for (l = 0;;) {
		text = realloc(text, l + SIZE);
		if (!text)
			return NULL;
		cnt = fread(&text[l], 1, SIZE, i);
		if (!cnt)
			break;
		l += cnt;
	}
	fclose(i);
	text = realloc(text, l + 1);
	if (!text)
		return NULL;
	text[l] = '\0';

	/* Check current System ARG_MAX limit. */
	if (l > 0.80 * (cnt = sysconf(_SC_ARG_MAX))) {
		fprintf(stderr, "%s: WARNING!!\n"
"   Scripts of length near to (or higher than) the current System limit on\n"
"   \"maximum size of arguments to EXEC\", could comprise its binary execution.\n"
"   In the current System the call sysconf(_SC_ARG_MAX) returns %d bytes\n"
"   and your script \"%s\" is %d bytes length.\n",
		my_name, cnt, file, l);
	}
	return text;
}

unsigned rand_mod(unsigned mod)
{
	/* Without skew */
	unsigned rnd, top = RAND_MAX;
	top -= top % mod;
	while (top <= (rnd = rand()))
		continue;
	/* Using high-order bits. */
	rnd = 1.0*mod*rnd/(1.0+top);
	return rnd;
}

char rand_chr(void)
{
	return (char)rand_mod(1<<(sizeof(char)<<3));
}

int noise(char * ptr, unsigned min, unsigned xtra, int str)
{
	if (xtra) xtra = rand_mod(xtra);
	xtra += min;
	for (min = 0; min < xtra; min++, ptr++)
		do
			*ptr = rand_chr();
		while (str && !isalnum((int)*ptr));
	if (str) *ptr = '\0';
	return xtra;
}

static int offset;

void prnt_bytes(FILE * o, char * ptr, int m, int l, int n)
{
	int i;

	l += m;
	n += l;
	for (i = 0; i < n; i++) {
		if ((i & 0xf) == 0)
			fprintf(o, "\n\t\"");
		fprintf(o, "\\%03o", (unsigned char)((i>=m) && (i<l) ? ptr[i-m] : rand_chr()));
		if ((i & 0xf) == 0xf)
			fprintf(o, "\"");
	}
	if ((i & 0xf) != 0)
		fprintf(o, "\"");
	offset += n;
}

void prnt_array(FILE * o, void * ptr, char * name, int l, char * cast)
{
	int m = rand_mod(1+l/4);		/* Random amount of random pre  padding (offset) */
	int n = rand_mod(1+l/4);		/* Random amount of random post padding  (tail)  */
	int a = (offset+m)%l;
	if (cast && a) m += l - a;		/* Type alignement. */
	fprintf(o, "\n");
	fprintf(o, "#define      %s_z	%d", name, l);
	fprintf(o, "\n");
	fprintf(o, "#define      %s	(%s(&data[%d]))", name, cast?cast:"", offset+m);
	prnt_bytes(o, ptr, m, l, n);
}

void dump_array(FILE * o, void * ptr, char * name, int l, char * cast)
{
	arc4(ptr, l);
	prnt_array(o, ptr, name, l, cast);
}

int write_C(char * file, char * argv[])
{
	char pswd[256];
	int pswd_z = sizeof(pswd);
	char* msg1 = strdup("has expired!\n");
	int msg1_z = strlen(msg1) + 1;
	int date_z = strlen(date) + 1;
	char* kwsh = strdup(shll);
	int shll_z = strlen(shll) + 1;
	int inlo_z = strlen(inlo) + 1;
	int xecc_z = strlen(xecc) + 1;
	int lsto_z = strlen(lsto) + 1;
	char* tst1 = strdup("location has changed!");
	int tst1_z = strlen(tst1) + 1;
	char* chk1 = strdup(tst1);
	int chk1_z = tst1_z;
	char* msg2 = strdup("abnormal behavior!");
	int msg2_z = strlen(msg2) + 1;
	int rlax_z = sizeof(rlax);
	int opts_z = strlen(opts) + 1;
	int text_z = strlen(text) + 1;
	char* tst2 = strdup("shell has changed!");
	int tst2_z = strlen(tst2) + 1;
	char* chk2 = strdup(tst2);
	int chk2_z = tst2_z;
	char* name = strdup(file);
	FILE * o;
	int indx;
	int numd = 0;
	int done = 0;

	/* Encrypt */
	srand((unsigned)time(NULL)^(unsigned)getpid());
	pswd_z = noise(pswd, pswd_z, 0, 0); numd++;
	stte_0();
	 key(pswd, pswd_z);
	msg1_z += strlen(mail);
	msg1 = strcat(realloc(msg1, msg1_z), mail);
	arc4(msg1, msg1_z); numd++;
	arc4(date, date_z); numd++;
	arc4(shll, shll_z); numd++;
	arc4(inlo, inlo_z); numd++;
	arc4(xecc, xecc_z); numd++;
	arc4(lsto, lsto_z); numd++;
	arc4(tst1, tst1_z); numd++;
	 key(chk1, chk1_z);
	arc4(chk1, chk1_z); numd++;
	arc4(msg2, msg2_z); numd++;
	indx = !rlax[0];
	arc4(rlax, rlax_z); numd++;
	if (indx && key_with_file(kwsh)) {
		fprintf(stderr, "%s: invalid file name: %s ", my_name, kwsh);
		perror("");
		exit(1);
	}
	arc4(opts, opts_z); numd++;
	arc4(text, text_z); numd++;
	arc4(tst2, tst2_z); numd++;
	 key(chk2, chk2_z);
	arc4(chk2, chk2_z); numd++;

	/* Output */
	name = strcat(realloc(name, strlen(name)+5), ".x.c");
	o = fopen(name, "w");
	if (!o) {
		fprintf(stderr, "%s: creating output file: %s ", my_name, name);
		perror("");
		exit(1);
	}
	fprintf(o, "#if 0\n");
	fprintf(o, "\t%s %s, %s\n", my_name, version, subject);
	fprintf(o, "\t%s %s %s %s\n\n\t", cpright, provider.f, provider.s, provider.e);
	for (indx = 0; argv[indx]; indx++)
		fprintf(o, "%s ", argv[indx]);
	fprintf(o, "\n#endif\n\n");
	fprintf(o, "static  char data [] = ");
	do {
		done = 0;
		indx = rand_mod(15);
		do {
			switch (indx) {
			case  0: if (pswd_z>=0) {prnt_array(o, pswd, "pswd", pswd_z, 0); pswd_z=done=-1; break;}
			case  1: if (msg1_z>=0) {prnt_array(o, msg1, "msg1", msg1_z, 0); msg1_z=done=-1; break;}
			case  2: if (date_z>=0) {prnt_array(o, date, "date", date_z, 0); date_z=done=-1; break;}
			case  3: if (shll_z>=0) {prnt_array(o, shll, "shll", shll_z, 0); shll_z=done=-1; break;}
			case  4: if (inlo_z>=0) {prnt_array(o, inlo, "inlo", inlo_z, 0); inlo_z=done=-1; break;}
			case  5: if (xecc_z>=0) {prnt_array(o, xecc, "xecc", xecc_z, 0); xecc_z=done=-1; break;}
			case  6: if (lsto_z>=0) {prnt_array(o, lsto, "lsto", lsto_z, 0); lsto_z=done=-1; break;}
			case  7: if (tst1_z>=0) {prnt_array(o, tst1, "tst1", tst1_z, 0); tst1_z=done=-1; break;}
			case  8: if (chk1_z>=0) {prnt_array(o, chk1, "chk1", chk1_z, 0); chk1_z=done=-1; break;}
			case  9: if (msg2_z>=0) {prnt_array(o, msg2, "msg2", msg2_z, 0); msg2_z=done=-1; break;}
			case 10: if (rlax_z>=0) {prnt_array(o, rlax, "rlax", rlax_z, 0); rlax_z=done=-1; break;}
			case 11: if (opts_z>=0) {prnt_array(o, opts, "opts", opts_z, 0); opts_z=done=-1; break;}
			case 12: if (text_z>=0) {prnt_array(o, text, "text", text_z, 0); text_z=done=-1; break;}
			case 13: if (tst2_z>=0) {prnt_array(o, tst2, "tst2", tst2_z, 0); tst2_z=done=-1; break;}
			case 14: if (chk2_z>=0) {prnt_array(o, chk2, "chk2", chk2_z, 0); chk2_z=done=-1; break;}
			}
			indx = 0;
		} while (!done);
	} while (numd+=done);
	fprintf(o, "/* End of data[] */;\n");
	fprintf(o, "#define      %s_z	%d\n", "hide", 1<<12);
	fprintf(o, SETUID_line, SETUID_flag);
	fprintf(o, DEBUGEXEC_line, DEBUGEXEC_flag);
	fprintf(o, TRACEABLE_line, TRACEABLE_flag);
	fprintf(o, HARDENING_line, HARDENING_flag);
	fprintf(o, HARDENINGSP_line, HARDENINGSP_flag);
    fprintf(o, BUSYBOXON_line, BUSYBOXON_flag);
	for (indx = 0; RTC[indx]; indx++)
		fprintf(o, "%s\n", RTC[indx]);
	fflush(o);
	fclose(o);

	return 0;
}

int make(void)
{
	char * cc, * cflags, * ldflags;
	char cmd[SIZE];

	cc = getenv("CC");
	if (!cc)
		cc = "cc";
	cflags = getenv("CFLAGS");
	if (!cflags)
		cflags = "";
	ldflags = getenv("LDFLAGS");
	if (!ldflags)
		ldflags = "";

if(!file2){
file2=(char*)realloc(file2,strlen(file)+3);
strcpy(file2,file);
file2=strcat(file2,".x");

}
	sprintf(cmd, "%s %s %s %s.x.c -o %s", cc, cflags, ldflags, file, file2);
	if (verbose) fprintf(stderr, "%s: %s\n", my_name, cmd);
	if (system(cmd))
		return -1;
	sprintf(cmd, "strip %s", file2);
	if (verbose) fprintf(stderr, "%s: %s\n", my_name, cmd);
	if (system(cmd))
		fprintf(stderr, "%s: never mind\n", my_name);
	sprintf(cmd, "chmod ug=rwx,o=rx %s", file2);
	if (verbose) fprintf(stderr, "%s: %s\n", my_name, cmd);
	if (system(cmd))
		fprintf(stderr, "%s: remove read permission\n", my_name);

	return 0;
}

void do_all(int argc, char * argv[])
{
	parse_args(argc, argv);
	text = read_script(file);
	if (!text)
		return;
	if (eval_shell(text))
		return;
	if (write_C(file, argv))
		return;
	if (make())
		return;
	exit(0);
}

int main(int argc, char * argv[])
{
	putenv("LANG=");
	do_all(argc, argv);
	/* Return on error */
	perror(argv[0]);
	exit(1);
	return 1;
}

