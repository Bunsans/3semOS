#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    printf("PID %d, PPID %d\n", getpid(), getppid());
    printf("\tPGID %d, SID %d\n", getpgrp(), getsid(0));

    const char *unknown = "?";

    uid_t uid = getuid();
    gid_t gid = getgid();
    //чтобы не было разыменования нулевого указателя
    struct passwd *pwuid = getpwuid(uid);
    const char *pwname = pwuid ? pwuid->pw_name : unknown;

    struct group *grid = getgrgid(gid);
    const char *grname = grid ? grid->gr_name : unknown;

    printf("\tUID %d(%s), GID %d(%s)\n", uid, pwname, gid, grname);

    uid_t euid = geteuid();
    gid_t egid = getegid();
    //чтобы не было разыменования нулевого указателя
    struct passwd *epwuid = getpwuid(euid);
    const char *epwname = epwuid ? epwuid->pw_name : unknown;

    struct group *egrid = getgrgid(egid);
    const char *egrname = egrid ? egrid->gr_name : unknown;

    printf("\tEUID %d(%s), EGID %d(%s)\n", euid, epwname, egid, egrname);

    // get supplementary groups
    //  If size is 0, the total number of supplementary group IDs for the process is returned.
    int ngroups = getgroups(0, NULL);

    gid_t list[ngroups];
    int num_groups;
    if ((num_groups = getgroups(ngroups + 1, list)) == -1)
    {
        perror("getgroups");
        return -1;
    }

    printf("Supplementary groups(%d): ", num_groups);
    for (int i = 0; i < num_groups; i++)
    {
        //чтобы не было разыменования нулевого указателя
        struct group *ngrid = getgrgid(list[i]);
        const char *grname = ngrid ? ngrid->gr_name : unknown;
        printf("%d(%s)%c ", list[i], (ngrid == NULL) ? "?" : ngrid->gr_name, (i == num_groups - 1) ? '\n' : ',');
    }

    printf("\n");
    return 0;
}