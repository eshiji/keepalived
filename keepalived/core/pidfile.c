/*
 * Soft:        Keepalived is a failover program for the LVS project
 *              <www.linuxvirtualserver.org>. It monitor & manipulate
 *              a loadbalanced server pool using multi-layer checks.
 *
 * Part:        pidfile utility.
 *
 * Version:     $Id: pidfile.c,v 1.1.16 2009/02/14 03:25:07 acassen Exp $
 *
 * Author:      Alexandre Cassen, <acassen@linux-vs.org>
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *              See the GNU General Public License for more details.
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 * Copyright (C) 2001-2009 Alexandre Cassen, <acassen@freebox.fr>
 */

#include "logger.h"
#include "pidfile.h"
extern char *main_pidfile;
extern char *checkers_pidfile;
extern char *vrrp_pidfile;

/* Create the runnnig daemon pidfile */
int
pidfile_write(char *pid_file, int pid)
{
	FILE *pidfile = fopen(pid_file, "w");

	if (!pidfile) {
		log_message(LOG_INFO, "pidfile_write : Can not open %s pidfile",
		       pid_file);
		return 0;
	}
	fprintf(pidfile, "%d\n", pid);
	fclose(pidfile);
	return 1;
}

/* Remove the running daemon pidfile */
void
pidfile_rm(char *pid_file)
{
	unlink(pid_file);
}

/* return the daemon running state */
int
process_running(char *pid_file)
{
	FILE *pidfile = fopen(pid_file, "r");
	pid_t pid;

	/* No pidfile */
	if (!pidfile)
		return 0;

	fscanf(pidfile, "%d", &pid);
	fclose(pidfile);

	/* If no process is attached to pidfile, remove it */
	if (kill(pid, 0)) {
		log_message(LOG_INFO, "Remove a zombie pid file %s", pid_file);
		pidfile_rm(pid_file);
		return 0;
	}

	return 1;
}

/* Return parent process daemon state */
int
keepalived_running(int mode)
{
	if (process_running(main_pidfile))
		return 1;
	else if (mode & 1 || mode & 2)
		return process_running((mode & 1) ? vrrp_pidfile :
				       checkers_pidfile);

	if (process_running(vrrp_pidfile) ||
	    process_running(checkers_pidfile))
		return 1;
	return 0;
}

/* Return VRRP child process state */
int
vrrp_running(void)
{
	return process_running(vrrp_pidfile);
}

/* Return VRRP child process state */
int
checkers_running(void)
{
	return process_running(checkers_pidfile);
}
