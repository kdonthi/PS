# PS
Partial implementation of the ```ps``` Linux command (https://man7.org/linux/man-pages/man1/ps.1.html) in C.

Compile using `gcc processid.c` and run using `./a.out`. </br>

Fields Outputted: </br>
 *```pid``` - Process ID </br>
 *```tty``` - Terminal ID </br>
 *```cmd``` - Gives information about the process </br>
 *```cwd``` - Gives the current working directory the excutable is in </br>

The PID's are sorted by 1) asc. terminal IDs and then 2) asc. PID values in addition to being filtered by user signals (```prochold.p_sigcatch```), tty process group ID (```eprochold.e_tpgid```), and saved effective user ID (```eprochold.e_pcred.p_svuid```) just like the actual ```ps``` command.  

