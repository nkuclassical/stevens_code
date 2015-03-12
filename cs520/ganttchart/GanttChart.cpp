//
//  main.cpp
//  HW3_2_test
//
//  Created by Yanqiao Zhan on 10/15/14.
//  Copyright (c) 2014 Yanqiao Zhan. All rights reserved.
//
//  The code for the problem 2, produce gantt chart
#include <iostream>
#include <string>
using namespace std;
double globetime;
double interval;
double aroundtime;
const int N=5;
struct Time
{
    double arrivaltime;
    double bursttime;
    string name;
    bool done=false;
    int priority;
}t[N];
void swap(Time *a,Time *b)
{
    Time tem;
    tem=*a;
    *a=*b;
    *b=tem;
}
void execute(Time info)
{
    cout<<info.name;
    
    double starttime=globetime;
    double finishtime=globetime+info.bursttime;
    
    for(double i=0;i<starttime;i+=interval)
    {
        cout<<" ";
    }
    double tt=0;
    while(tt+starttime<finishtime)
    {
        tt+=interval;
        cout<<"*";
    }
    double localaroundtime=finishtime-info.arrivaltime;
    aroundtime+=localaroundtime;
    cout<<" waiting time:"<<starttime<<" ";
    cout<<"  time: "<<localaroundtime<<endl;
    
}

void init()
{
    globetime=0.0;
    interval=0.2;
    aroundtime=0.0;
    t[0].arrivaltime=0.0;
    t[0].bursttime=10;
    t[0].name="P1";
    t[0].priority=3;
    
    t[1].arrivaltime=0.0;
    t[1].bursttime=1;
    t[1].name="P2";
    t[1].priority=1;
    
    t[2].arrivaltime=0.0;
    t[2].bursttime=2;
    t[2].name="P3";
    t[2].priority=3;
    
    t[3].arrivaltime=0.0;
    t[3].bursttime=1;
    t[3].name="P4";
    t[3].priority=4;
    
    t[4].arrivaltime=0.0;
    t[4].bursttime=5;
    t[4].name="P5";
    t[4].priority=2;
    for(int i=0;i<N;i++)
        t[i].done=false;
    
}

void sjf(double lefttime)
{
    init();
    globetime+=lefttime;
    double mintime;
    for(int i=0;i<N;i++)
    {
        
        mintime=0x3fffffff;
        int j;
        int jj=0;
        for(j=0;j<N;j++)
        {
            if(t[j].arrivaltime<=globetime&&t[j].bursttime<mintime&&(t[j].done==false))
            {
                jj=j;
                mintime=t[j].bursttime;
            }
        }
        execute(t[jj]);
        t[jj].done=true;
        globetime+=t[jj].bursttime;
        cout<<"globe "<<globetime<<endl;
        
    }
    
    
    cout<<"TurnaroundTime "<<aroundtime/N<<endl;

}
void fcfs()
{
    //FCFS

    for(int i=0;i<N;i++)
    {
        for(int j=i;j<N;j++)
        {
            if(t[i].arrivaltime>t[j].arrivaltime)
            {
                swap(t[i],t[j]);
            }
        }
    }
    for(int i=0;i<N;i++)
    {
        execute(t[i]);
        globetime+=t[i].bursttime;
    }
    cout<<"TurnaroundTime "<<aroundtime/N<<endl;
}
void priority()
{
    //Priority
    init();
    for(int i=0;i<N;i++)
    {
        for(int j=i;j<N;j++)
        {
            if(t[i].priority>t[j].priority)
            {
                swap(&t[i],&t[j]);
            }
        }
    }
    for(int i=0;i<N;i++)
    {
        execute(t[i]);
//        cout<<t[i].name<<endl;
        globetime+=t[i].bursttime;
    }
     cout<<"TurnaroundTime "<<aroundtime/N<<endl;
}
void rr()
{
    init();
    Time tt[N];
    for(int i=0;i<N;i++)
    {
        tt[i]=t[i];
        cout<<t[i].name<<" ";
    }
    
    cout<<endl;
    int test=0;
    while(true)
    {
        test=0;
        for(int i=0;i<N;i++)
        {
            if(t[i].bursttime>0)
            {
                globetime+=1;

                for(int j=0;j<i;j++)
                    cout<<"   ";
                cout<<"*";
                test++;
                t[i].bursttime-=1;
                if(t[i].bursttime==0)
                {
                    int waittime=globetime-tt[i].bursttime;
                    aroundtime+=globetime;
                    cout<<globetime<<" "<<waittime;
                }
                //cout<<globetime;
                cout<<endl;
                
                
            }
    
         
       
        }
        if(test==0)break;
        
    }
    cout<<"TurnaroundTime "<<aroundtime/N<<endl;
}
int main()
{
    init();

    
    cout<<"FCFS model"<<endl;
    fcfs();
    //SJF
    cout<<endl<<"SJF model"<<endl;
    sjf(0);
    
    //Left Idle
    cout<<endl<<"Nonpreemptive priority"<<endl;
    priority();
    
    cout<<endl<<"RR"<<endl;
    
    rr();
    return true;
}