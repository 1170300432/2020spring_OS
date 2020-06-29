#include <unistd.h>  
#include <stdio.h>  
#include <sys/types.h>
#include <sys/wait.h> 
#include <time.h>
#include <sys/times.h>

#define HZ	100

void cpuio_bound(int last, int cpu_time, int io_time);

int main()
{
    pid_t fpid;
    int count, i; 
    int status = -1; 
    for(count = 0; count < 10; count++){
        if ((fpid = fork()) < 0)
            printf("Error!");
        else if (fpid == 0) {
            printf("I'm child process, my pid is: %d\n", getpid());
            i = count % 5;
            cpuio_bound(5, i, 5-i);
            return 0;
        }
        else { 
            printf("I'm father process, my child's pid is: %d\n",fpid);     
        }
    }
    for(count = 0; count < 10; count++){
        wait(&status);
    }
    printf("There are %d process.\n",count+1);   
    return 0;  
}

/*
 * 此函数按照参数占用CPU和I/O时间
 * last: 函数实际占用CPU和I/O的总时间，不含在就绪队列中的时间，>=0是必须的
 * cpu_time: 一次连续占用CPU的时间，>=0是必须的
 * io_time: 一次I/O消耗的时间，>=0是必须的
 * 如果last > cpu_time + io_time，则往复多次占用CPU和I/O
 * 所有时间的单位为秒
 */
void cpuio_bound(int last, int cpu_time, int io_time)
{
	struct tms start_time, current_time;
	clock_t utime, stime;
	int sleep_time;

	while (last > 0)
	{
		/* CPU Burst */
		times(&start_time);
		/* 其实只有t.tms_utime才是真正的CPU时间。但我们是在模拟一个
		 * 只在用户状态运行的CPU大户，就像“for(;;);”。所以把t.tms_stime
		 * 加上很合理。*/
		do
		{
			times(&current_time);
			utime = current_time.tms_utime - start_time.tms_utime;
			stime = current_time.tms_stime - start_time.tms_stime;
		} while ( ( (utime + stime) / HZ )  < cpu_time );
		last -= cpu_time;

		if (last <= 0 )
			break;

		/* IO Burst */
		/* 用sleep(1)模拟1秒钟的I/O操作 */
		sleep_time=0;
		while (sleep_time < io_time)
		{
			sleep(1);
			sleep_time++;
		}
		last -= sleep_time;
	}
}


