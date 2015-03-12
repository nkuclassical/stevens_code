//
//  main.cpp
//  HW3
//
//  Created by Yanqiao Zhan on 10/16/14.
//  Copyright (c) 2014 Yanqiao Zhan. All rights reserved.
//

#include <iostream>
using namespace std;
int globetime;
int ioroundtime;
int seed;
int A,M,C;
const int jobnum=10;
int cpuburst[]={30,35,40,45,50,55,60,65,70,75};//The cpu burst time for ten jobs respectly
int waitingtime[10];
int turnaroundtime[10];
int jobtotal;
int cpurunningtime;
int cpuwaitingtime;
double alpha;
int quantum;
int context_switch;
struct job
{
    int bursttime;//30,35,40,45...
    int entertime;//the time job enter the ready queue.
    bool start=false;
    bool finish=false;
    int id;//count from 0
    int length;
    int runningtime;
    int waittime;
    int tt;//predicted cpu burst time
    int t;//actual cpu burst time
   // int rrrest;
}jobs[jobnum];

int myrandom()
{
    if(M==0)return 0;
    else
    {
        seed=(A*seed+C)%M;
        return seed;
    }
}
int limitrandom(int max)//return a random number, range [0,max-1]
{
    if(max==0)return 0;
    return myrandom()%max;
}
int meanrandom(int mean)
{
    return limitrandom(mean*0.2)+mean*0.9;
}
void display()
{
    cout<<"-------------------------"<<endl;
    for(int i=0;i<jobnum;i++)
    {
        cout<<"Global time "<<globetime<<"  Job ID "<<jobs[i].id<<"  burst time"<<jobs[i].bursttime<<"  expect next enter time"<<jobs[i].entertime<<"  length "<<jobs[i].length<<endl;
    }
}
void init()
{
    int average=0;
    A=25173;//used for the random
    C=13849;
    M=65536;
    long system=time(0);
    seed=system%M;//set the seed
    for(int i=0;i<jobnum;i++)
    {
        jobs[i].start=true;
        jobs[i].entertime=0;
        ioroundtime=60;
  //     jobs[i].length=limitrandom(2*60)*1000+2*60*1000;
        jobs[i].length=3*60*1000;
        average+=jobs[i].length;
        jobs[i].id=i;
        jobs[i].bursttime=meanrandom(cpuburst[jobs[i].id]);
        jobs[i].runningtime=0;
        jobs[i].waittime=0;
        jobs[i].tt=(jobs[i].t=0);
        cout<<i<<"  "<<jobs[i].length<<endl;
    }
    cout<<average/jobnum<<endl;
    cpurunningtime=0;
    cpuwaitingtime=0;
    jobtotal=0;
    alpha=0;
    quantum=60;
    context_switch=quantum*quantum/200;
}
void executejob(int J)//execute the job, and modify the global time, and change the status of the execute job.
{
    jobs[J].waittime+=(globetime-jobs[J].entertime);
    jobtotal++;
    if(jobs[J].length>=jobs[J].bursttime)//if the left length of task is longer than the cpu burst time.
    {
        globetime+=jobs[J].bursttime;//CPU burst time for this time is calculate at the end of last time, just for simplify the program, so program will not use this time burst time before the process actually execute
        cpurunningtime+=jobs[J].bursttime;
        jobs[J].length-=jobs[J].bursttime;
        jobs[J].runningtime+=jobs[J].bursttime;
        jobs[J].t=jobs[J].bursttime;//this line is use for SJF only
        jobs[J].tt=alpha*jobs[J].t+(1-alpha)*jobs[J].tt;//this line is use for SJF only.
        jobs[J].entertime=globetime+ioroundtime;//enter time for the next loop, means the time IO is finished
        if(jobs[J].length<=0)jobs[J].finish=true;
        jobs[J].bursttime=meanrandom(cpuburst[jobs[J].id]);//CPU burst time for the next time
    }
    else//it is the last cpu burst, as the left length of task is less than the cpu burst time.
    {
        globetime+=jobs[J].length;
        cpurunningtime+=jobs[J].length;
        jobs[J].runningtime+=jobs[J].length;
        jobs[J].length=0;
        jobs[J].finish=true;
    }
}
void fcfs()
{
    init();
    while(true){
        int runid=-1;
        int minenter=globetime+1;//The job with enter time later than globel time is still under IO process.
        bool finish=true;
        for(int i=0;i<jobnum;i++)
        {
            if(!jobs[i].finish)finish=false;
            if(jobs[i].start&&!jobs[i].finish&&jobs[i].entertime<minenter)
            {
                runid=i;
                minenter=jobs[i].entertime;//If the first come job change, the minuim enter time should also change.
            }
        }
        if(runid>=0)
        {
            executejob(runid);
        }
        else
        {
            if(!finish)//It means still some jobs have not finished, just wait for 5ms, and try again.
            {
                globetime+=5;
                cpuwaitingtime+=5;
            }
            else break;//All the jobs have finished, FCFS algorithm over.
        }
    }
    int sumturnaroundtime=0;
    for(int i=0;i<jobnum;i++)
    {
        cout<<"Job ID "<<jobs[i].id<<"  running time "<<jobs[i].runningtime/1000<<"S  waitingtime "<<jobs[i].waittime/1000<<"S"<<endl;
        cout<<"Turnaround Time "<<(jobs[i].runningtime+jobs[i].waittime)/1000<<"s"<<endl;
        sumturnaroundtime+=(jobs[i].runningtime+jobs[i].waittime)/1000;
    }
    cout<<"CPU running time "<<cpurunningtime/1000<<"  CPU utilization "<<static_cast<double>(cpurunningtime)/globetime*100<<"%"<<endl;
    cout<<"CPU waiting time "<<cpuwaitingtime/1000<<"s"<<endl;
    cout<<"throughput "<<static_cast<double>(jobtotal)/globetime*1000<<" jobs/s"<<endl;
    cout<<"MeanTurnaround time "<<sumturnaroundtime/jobnum<<endl;

    
}
void sjf()
{
    init();
    while(true){
        int runid=-1;
        int shortestjob=10000000;//Initialize the shortest job length as 1000000, since no job cpu burst time will as large as 10000000.
        bool finish=true;
        for(int i=0;i<jobnum;i++)
        {
            if(!jobs[i].finish)finish=false;
            if(jobs[i].start&&!jobs[i].finish&&jobs[i].tt<shortestjob&&jobs[i].entertime<=globetime)//Get the shortest job
            {
                runid=i;
                shortestjob=jobs[i].bursttime;//change the shortest job id.
            }
        }
        if(runid>=0)
        {
            executejob(runid);
        }
        else
        {
            if(!finish)//It means still some jobs have not finished, just wait for 5ms, and try again.
            {
                globetime+=5;//Globe time should add 5
                cpuwaitingtime+=5;//CPU waiting time should add 5, as cpu now is empty wait
            }
            else break;//All the jobs have finished, SJF algorithm over.
        }
    }
     int sumturnaroundtime=0;
    for(int i=0;i<jobnum;i++)
    {
        cout<<"Job ID "<<jobs[i].id<<"  running time "<<jobs[i].runningtime<<"  waitingtime "<<jobs[i].waittime<<endl;
        cout<<"Turnaround Time "<<(jobs[i].runningtime+jobs[i].waittime)/1000<<"s"<<endl;
        sumturnaroundtime+=(jobs[i].runningtime+jobs[i].waittime)/1000;
    }
    cout<<"CPU running time "<<cpurunningtime/1000<<"  CPU utilization "<<static_cast<double>(cpurunningtime)/globetime*100<<"%"<<endl;
    cout<<"CPU waiting time "<<cpuwaitingtime/1000<<"s"<<endl;
    cout<<"throughput "<<static_cast<double>(jobtotal)/globetime*1000<<"jobs/s"<<endl;
    cout<<"MeanTurnaround time"<<sumturnaroundtime/jobnum<<endl;
}
void executerr(int jobid)
{
    
    jobs[jobid].waittime+=(globetime-jobs[jobid].entertime);
    if(jobs[jobid].bursttime<quantum)//job can get the cpu resource and do the I/O operation later
    {
        
        if(jobs[jobid].length>jobs[jobid].bursttime)
        {
            globetime+=jobs[jobid].bursttime;
            cpurunningtime+=jobs[jobid].bursttime;
            jobs[jobid].length-=jobs[jobid].bursttime;
            jobs[jobid].entertime=globetime+ioroundtime;
            jobs[jobid].runningtime+=jobs[jobid].bursttime;
            jobs[jobid].bursttime=meanrandom(cpuburst[jobid]);//set the next recurisive loop
        }
        else//the job is over, due to the time consume end.
        {
            cpurunningtime+=jobs[jobid].length;
            globetime+=jobs[jobid].length;
            jobs[jobid].runningtime+=jobs[jobid].length;
            jobs[jobid].length=0;
            jobs[jobid].finish=true;
        }
    }
    else//wait here
    {
        cpurunningtime+=quantum;
        globetime+=quantum;
        jobs[jobid].bursttime-=quantum;
        jobs[jobid].length-=quantum;
        jobs[jobid].runningtime+=quantum;
        jobs[jobid].entertime=globetime;
    }
}

void rr()
{
    init();
    while(true)
    {
        int runid=-1;
        bool finish=true;
        for(int i=0;i<jobnum;i++)
        {
            if(!jobs[i].finish)finish=false;
            if(!jobs[i].finish&&jobs[i].entertime<=globetime)
            {
                globetime+=context_switch;
                cpuwaitingtime+=context_switch;
                runid=i;
                executerr(i);
                
            }
        }
        if(runid==-1)
        {
            if(!finish)
            {
                globetime+=5;
                cpuwaitingtime+=5;
            }
            else
                break;
        }
    }
    int sumturnaroundtime=0;
    int sumwaitingtime=0;
    for(int i=0;i<jobnum;i++)
    {
        cout<<"Job ID "<<jobs[i].id<<"  running time "<<jobs[i].runningtime<<"  waitingtime "<<jobs[i].waittime<<endl;
        cout<<"Turnaround Time "<<(jobs[i].runningtime+jobs[i].waittime)/1000<<"s"<<endl;
        sumturnaroundtime+=(jobs[i].runningtime+jobs[i].waittime)/1000;
        sumwaitingtime+=jobs[i].waittime/1000;
    }
    
    cout<<"CPU running time "<<cpurunningtime/1000<<"  CPU utilization "<<static_cast<double>(cpurunningtime)/globetime*100<<"%"<<endl;
    cout<<"CPU waiting time "<<cpuwaitingtime/1000<<"s"<<endl;
    cout<<"MeanTurnaround time "<<sumturnaroundtime/jobnum<<endl;
    cout<<"Average waiting time "<<sumwaitingtime/jobnum<<endl;

}

int main(int argc, const char * argv[]) {
  
  //  display();
  //  fcfs();
  //  sjf();
    rr();
    return 0;
}
