#
# Regular cron jobs for the meebible package
#
0 4	* * *	root	[ -x /usr/bin/meebible_maintenance ] && /usr/bin/meebible_maintenance
