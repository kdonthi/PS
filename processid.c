#include <sys/sysctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#include <sys/proc.h>
#include <string.h>
#include <pwd.h>

int number_len (long number)
{
	//Finds length of a long number
	int len;
	if (number == 0)
		return (1);
	while (number != 0)
	{
		number /= 10;
		len++;
	}
	return (len);
}
char* ttycreator (long e_tdev) 
{
	//Creates a string in format "ttys..." from et_dev number
	long subtract = 268435456; 			// last three digits in ttys string follows formula (e_tdev  - subtract)
	long num_at_end = e_tdev - subtract;
	int numb_len = number_len(num_at_end);
	char* tty = malloc(sizeof(char) * 8); 		//7 chars + 1 for termination
	int i;

	tty[7] = '\0';
	tty[0] = 't';
	tty[1] = 't';
	tty[2] = 'y';
	tty[3] = 's';

	for (i = 4; i < 4 + (3 - numb_len); i++) 	//filling blanks with 0's
	{
		tty[i] = '0';
	}

	for (int j = 6; j >= i; j--) 			//inserting number into array backwards
	{
		tty[j] = (num_at_end - ((num_at_end / 10) * 10)) + '0';
		num_at_end /= 10;
	}
	return (tty);
}
void swap (struct kinfo_proc *a, struct kinfo_proc *b)
{
	//Swaps two kinfo_proc structures
	struct kinfo_proc temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void sortkinfo (struct kinfo_proc *klst, int length)
{
	//Sorts kinfo_proc structures by 1) asc. e_tdev values, then 2) asc. pid values, just like in actual ps command
	int minindex;

	for (int i = 0; i < length; i++)
	{
		minindex = i;
		for (int j = i; j < length; j++)
		{
			if (klst[j].kp_eproc.e_tdev < klst[minindex].kp_eproc.e_tdev || //lower e_tdev number
					(klst[j].kp_eproc.e_tdev == klst[minindex].kp_eproc.e_tdev && klst[j].kp_proc.p_pid < klst[minindex].kp_proc.p_pid)) //same et_dev but lower pid
				minindex = j;
		}
		if (minindex != i)
			swap(&klst[i], &klst[minindex]);
	}
}

int main (int argc, char *argv[])
{
	int			mib[4];			//"Managment Information Base" array
	size_t			lenmib;			//length of "Mangement Information Base" array
	struct kinfo_proc	*kp;			//points to array of kinfo_proc structures
	size_t			sizeneeded;		//holds size needed for kinfo_proc structures
	int			kp_number;		//number of kinfo_proc structures
	struct extern_proc	prochold;		//holds struct proc kp_proc
	struct eproc		eprochold;		//holds struct eproc kp_eproc
	char			cwdarray[PATH_MAX];	//holds current working directory

	lenmib = 4;
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_ALL;			
	mib[3] = 0;						//these options get entire process table
	sysctl(mib, lenmib, NULL, &sizeneeded, NULL, 0);	//getting size in sizeneeded
	kp_number = sizeneeded / sizeof(struct kinfo_proc); 	
	kp =  (struct kinfo_proc*) malloc(sizeneeded);
	sysctl(mib, lenmib, kp, &sizeneeded, NULL, 0);		//getting kinfo_proc structs
	sortkinfo(kp, kp_number);				//sorting structs based on e_tdev and pid #'s
	for (int i = 0; i < kp_number; i++)
	{
		prochold = kp[i].kp_proc;
		eprochold = kp[i].kp_eproc;
		getcwd(cwdarray,sizeof(cwdarray));		//gets current working directory
		if ((eprochold.e_tdev != NODEV && (prochold.p_flag & P_CONTROLT) != 0) 						//used to figure out if controlled by terminal
				&& prochold.p_sigcatch != 0 && eprochold.e_tpgid != 0 && eprochold.e_pcred.p_svuid != 0)	//additional filters to make it get same processes as ps
		{	printf("pid: %lu, tty: %s, cmd: %s, cwd: %s\n", (long) prochold.p_pid, ttycreator(kp[i].kp_eproc.e_tdev), prochold.p_comm, cwdarray);
			printf("cmd?: %s", kp[i].cmdline); }
	}
}

