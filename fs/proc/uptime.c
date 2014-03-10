#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <linux/kernel_stat.h>
#include <asm/cputime.h>

static int uptime_proc_show(struct seq_file *m, void *v)
{
	struct timespec uptime;
	struct timespec idle;
	int i;
	cputime_t idletime = cputime_zero;

	for_each_possible_cpu(i)
		idletime = cputime64_add(idletime, kstat_cpu(i).cpustat.idle);

	do_posix_clock_monotonic_gettime(&uptime);
	monotonic_to_bootbased(&uptime);
	cputime_to_timespec(idletime, &idle);
	seq_printf(m, "%lu.%02lu %lu.%02lu\n",
			(unsigned long) uptime.tv_sec,
			(uptime.tv_nsec / (NSEC_PER_SEC / 100)),
			(unsigned long) idle.tv_sec,
			(idle.tv_nsec / (NSEC_PER_SEC / 100)));
	return 0;
}

static int uptime_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, uptime_proc_show, NULL);
}

static const struct file_operations uptime_proc_fops = {
	.open		= uptime_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

#ifdef CONFIG_ZTE_PLATFORM
extern void zte_get_total_suspend(struct timespec *ts);

static int sleeptime_proc_show(struct seq_file *m, void *v)
{
	struct timespec total_suspendtime;//zte
	
	zte_get_total_suspend(&total_suspendtime);
	
	seq_printf(m, "%lu.%02lu \n",
			(unsigned long) total_suspendtime.tv_sec,
			(total_suspendtime.tv_nsec / (NSEC_PER_SEC / 100))
			);
	return 0;
}

static int sleeptime_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, sleeptime_proc_show, NULL);
}

static const struct file_operations sleeptime_proc_fops = {
	.open		= sleeptime_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif

static int __init proc_uptime_init(void)
{
	proc_create("uptime", 0, NULL, &uptime_proc_fops);
	#ifdef CONFIG_ZTE_PLATFORM
	proc_create("sleeptime", 0, NULL, &sleeptime_proc_fops);
	#endif
	return 0;
}
module_init(proc_uptime_init);
