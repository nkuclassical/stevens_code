//
//  hw.cpp
//  hw2_mac
//
//  Created by Yanqiao Zhan on 9/28/2014.
//  Finished on 10/03/2014
//  Copyright (c) 2014 Yanqiao Zhan. All rights reserved.
//

//#include "hw.h"

#include <iostream>
#include <fstream>
#include <queue>
#include <cstdlib>
#include <ctime>
using namespace std;


int seed ;//seed for random number generation
int buses;//the number of buses
int stops;//the number of stops
int drivetime;//the driving time between stops
int boardingtime;//the boarding time
int stoptime;//the stop time
int mean_passenger_rate;//the mean arrival rate of passenger
int currenttime;//the current time, and in over program it will increment by min(boarding time, drive time between two contiguous stops)
int incrementtime;//min(boarding time, drive time between two contiguous stops)
int A,M,C;//used in myrandom
int totalpassenger;//calculate the number of total passengers

int boardtime;
int testbigger;
int testless;
int maxpassenger;

int myrandom()//generate a random, range [0,M].
{
    if(M==0)return 0;
    else
    {
        seed=(A*seed+C)%M;
        return seed;
    }
}

int limitrandom(int max)//return a random number within a limit range 0~max-1
{
    if (max == 0)return 0;//deal with the situation of max=0
    return myrandom() % max;
}
int passengernumber(int leavetime)//return a number for passengers [0.9*expect_number,1.1*expect_number]
{
    int tem = mean_passenger_rate * leavetime / 60;
    return limitrandom(tem*0.2)+tem*0.9;//random the number of people in the stop, the mean passenger number is mean_passenger_rate*interval_time

}
struct Bus
{
    int time;//to inditifiy whether all the passengers have boarded
    int busid;//0,1,2,3,...,n-1
    bool stopped;//for example if the bus stops at station 1, then station 1 will have a struct bus which stopped value is true, otherwise the value turns false
};
struct Node//The node structure of double linked list
{
    int stopid;//from 1~m(m is the total stops number)
    int stoptime;//the time when last bus leave, it can help random function to count how many new comers to the station from the last bus leave.
    Bus bus;
    Node*nextNode, *frontNode;
    Node()
    {
        nextNode = NULL;
        frontNode = NULL;
        bus.stopped = false;
        bus.time = 0;
        bus.busid = -1;
        stoptime = 0;
        stopid=-1;//head and tail will only have stopid=-1
    }
    Node(int n)
    {
        nextNode = NULL;
        frontNode = NULL;
        stopid = n;// node except head and tail will have stopid the same with its number.
        bus.stopped = false;
        bus.time = 0;
        bus.busid = -1;
        stoptime = 0;
    }
};
class doublelinkedlist//it is a circle double linked list
{
    Node*head, *tail;
    int*passenger;
public:
    doublelinkedlist()//initialization, and link the tail and head nodes
    {
        head = new Node();
        tail = new Node();
        head->frontNode = tail;
        head->nextNode = tail;
        tail->frontNode = head;
        tail->nextNode = head;
        //passenger = new int[stops];
    }
    void initbusid();
    void addnode(int number);
    bool event();
    void display();
    void gotonextstation(Node*a);
    int restpassenger();
    void displaystopid();
};
void doublelinkedlist::displaystopid()//can display all the stopid and bus id, if necessary.
{
    Node*node=head;
    while(node!=tail->nextNode)
    {
        cout<<node->stopid<<" bus stopped"<<node->bus.stopped<<"  ";
        
        if(node!=tail)node=node->nextNode;
    }
    cout<<endl;
}
void doublelinkedlist::gotonextstation(Node*a)// achieve a bus to the next station, it deals with the tail and head specially.
{
    
    Node*node = a;
    if (node->nextNode != tail)
    {
        a->nextNode->bus.busid = a->bus.busid;//the station with a positive busid means a bus is at the station, and the bus id is just as this positive number.
        a->nextNode->bus.stopped = true;
        a->nextNode->bus.time = a->bus.time + drivetime;//plus drive time, bus.time only deals with the boarding time and past driving time.
        a->nextNode->stoptime = currenttime;//modify the stoptime.
    }
    else//deal with the tail and head situation
    {
        head->nextNode->bus.busid = a->bus.busid;
        head->nextNode->bus.stopped = true;
        head->nextNode->bus.time = a->bus.time + drivetime;
        head->nextNode->stoptime = currenttime;
        
    }
    a->bus.stopped = false;//set the front bus station as empty
    a->bus.busid = -1;//set the front bus station as no bus.
}
int doublelinkedlist::restpassenger()//return the number of unaboard passengers after a loop is finished.
{
    int people=0;
    int totalpeople=0;
    int i=0;
    Node *node=head->nextNode;
    while(node!=tail)
    {
        if(!node->bus.stopped)
        {
            people=passengernumber(currenttime-node->stoptime);
            cout<<" No."<<node->stopid<<" station left "<<people<<" passengers"<<endl;
            totalpeople+=people;
        }
        i++;
        node=node->nextNode;
    }
    
    return totalpeople;
}
bool doublelinkedlist::event()//deal with passengers aborad & buses move
{
    bool returnvalue = false;//returnvalue turns to true when any bus moves to the next station
    Node *temnode = head->nextNode;
    for (int i = 0; i < stops; i++)
    {
        if (temnode->bus.stopped)
        {
            if (temnode->bus.time <= currenttime && (!temnode->nextNode->bus.stopped) && temnode->nextNode != tail)//move condition is satisified, and the bus moves to the next stop
            {
                passenger[(i + 1) % stops] = passengernumber(currenttime-temnode->nextNode->stoptime);//calculate how many passengers are waiting at the next station. mean_rate*waiting_time
                totalpassenger+=passenger[(i+1)%stops];//renew total passengers record
                boardtime++;//renew total board time
                temnode->bus.time += passenger[(i + 1) % stops] * boardingtime;//add the new passengers boarding time
                
                //print the key information that updates
                cout <<"current time: "<< currenttime<<" stopid: "<<temnode->stopid << " last stop time: " <<temnode->nextNode->stoptime<<" bus no:"<< temnode->bus.busid <<"  passengers " << passenger[(i + 1) % stops] << " time to boarding " << passenger[(i + 1) % stops] * boardingtime <<" Expect people:"<<mean_passenger_rate*(currenttime-temnode->nextNode->stoptime)/60;
                cout <<" wait time:" <<currenttime - temnode->nextNode->stoptime << endl;
               
                if(passenger[(i+1)%stops]>maxpassenger)maxpassenger=passenger[(i+1)%stops];//renew the max passengers record
                gotonextstation(temnode);//move to the next station
                returnvalue = true;//change the return value, but the loop is not end, program will go on to see the next station!
            }
            else if (temnode->bus.time <= currenttime && (temnode->nextNode==tail)&&(!head->nextNode->bus.stopped))//deal with the tail, for the tail=null, so we point to the head->nextnode the bus will go to the number 1 stops
            {
                passenger[(i + 1) % stops] = passengernumber(currenttime-head->nextNode->stoptime);//we use head->nextNode replace the temnode->nextnode
                if(passenger[(i+1)%stops]>maxpassenger)maxpassenger=passenger[(i+1)%stops];//renew the max passengers record
                totalpassenger+=passenger[(i+1)%stops];//renew the total passengers record
                boardtime++;//renew board time record
                temnode->bus.time += passenger[(i + 1) % stops] * boardingtime;

                
                //print the key information that updates
                cout <<"current time1: "<< currenttime<<" stopid: "<<temnode->stopid << " last stop time: " <<head->nextNode->stoptime<<" bus no:"<< temnode->bus.busid <<"  passengers " << passenger[(i + 1) % stops] << " time to boarding " << passenger[(i + 1) % stops] * boardingtime <<" Expect people:"<<mean_passenger_rate*(currenttime-head->nextNode->stoptime)/60;
                cout <<" wait time:" <<currenttime - head->nextNode->stoptime << endl;
                gotonextstation(temnode);//move to the next station
                returnvalue = true;//the loop not finished!
            }
            
            
          else if (temnode->bus.time<=currenttime&&temnode->nextNode!=tail&&temnode->nextNode->bus.stopped)//it is the part of improved model! you can refer to section 'improved model' in my report
            {
                
                Node*tem = temnode;
                int j = i;//j is the index of first station without parking a bus
                while (tem->nextNode->bus.stopped&&tem->nextNode!=tail)
                {

                    j++;
                    tem = tem->nextNode;
                }
                if (tem->nextNode == tail&&head->nextNode->bus.stopped)//deals with the tail!
                {
                    tem = head;
                    while (tem->nextNode->bus.stopped)
                    {
                        tem = tem->nextNode;
                        j++;
                    }
                }
                while (tem != temnode)//move all the next bus to the next station first, and then move current bus to the next station, to avoid conflict
                {
                    int existtime = currenttime - tem->stoptime;//by calculate how many passengers have enter the bus
                    int noboardtime = passenger[j % stops] * boardingtime - existtime;//passenger[i] subtract the exist time is the information that passengers are still in the station
                    if(tem->nextNode!=tail)//renew the passenger[i+1]
                    {
                        passenger[(j + 1) % stops] = passengernumber(currenttime - tem->nextNode->stoptime);
                        if(passenger[(i+1)%stops]>maxpassenger)maxpassenger=passenger[(i+1)%stops];//renew max passengers record, if neccessary
                        totalpassenger+=passenger[(j+1)%stops];
                    }
                    else//deals with the tail
                    {
                        passenger[(j+1)%stops]=passengernumber(currenttime-head->nextNode->stoptime);
                        if(passenger[(i+1)%stops]>maxpassenger)maxpassenger=passenger[(i+1)%stops];
                        totalpassenger+=passenger[(j+1)%stops];//renew the total passengers record, if necessary
                    }
                    
                    tem->bus.time += passenger[(j + 1) % stops]-noboardtime;
                    gotonextstation(tem);//move the first bus to the next station
                    boardtime++;
                    j--;//go back
                    tem = tem->frontNode;//go back to the front station
                    if (tem == head)// when meet the head, change tem to the tail->frontnode
                    {
                        tem = tail->frontNode;
                    }
                }
                return true;
            }
        }
        temnode = temnode->nextNode;//one loop is finished, go to the next station.
    }
    return returnvalue;//whole loops are finished, return back to even()function out the class.
}

void doublelinkedlist::initbusid()//initialize all buses' id.
{
    int interval = stops / buses;
    int id=0;
    Node*tem = head->nextNode;//get the head
    tem->bus.busid = id;
    tem->bus.stopped = true;
    passenger = new int[stops];
    for (int i = 0; i < stops; i++)
        passenger[i] = 1;
    while (tem != tail)
    {
        for (int i = 0; i < interval; i++)
            tem = tem->nextNode;
        tem->bus.busid = ++id;//set the id
        tem->bus.stopped = true;//set node's stopped, to indicate a bus is stopping at this station
    }
}

void doublelinkedlist::addnode(int number)//add node at the tail, and reset the tail, double linked list
{
    
    Node *newNode = new Node(number);
    newNode->frontNode = tail->frontNode;
    newNode->nextNode = tail;
    tail->frontNode = newNode;
    newNode->frontNode->nextNode = newNode;
}
void doublelinkedlist::display()//cout the important information
{
    Node*node = new Node();
    node = head->nextNode;
    cout << "-------------------------------------------------" << endl;
    cout << currenttime << "  ";
    while (node != tail)
    {
        if (node->bus.stopped)
        {
            cout << "("<<node->stopid << ",";
            if (node->bus.busid != -1)
            {
                cout << node->bus.busid+1<<")" << '\t';
            }
            cout << '\t';
        }
        node = node->nextNode;
    }
    cout << endl;
    
}

doublelinkedlist list;
int min(int a, int b)//return a miner number
{
    if (a > b)return b;
    else return a;
}
void initialization()//for system initialization
{
    ifstream in("initialization.txt");//read initialization data from text file
    A=25173;//used for the random
    C=13849;
    M=65536;
    in >> buses>>stops>>drivetime>>boardingtime>>stoptime>>mean_passenger_rate;//set the variable number
    drivetime *= 60;
    stoptime *= 60*60;
    
    for (int i = 1; i <=stops ; i++)//initialize the double linked list
    {
        list.addnode(i);
    }
    list.initbusid();
    currenttime = 0;
    incrementtime = boardingtime;
    long system=time(0);
    seed=system%M;//set the seed
    totalpassenger=0;
    boardtime=0;
    maxpassenger=0;
    cout<<"Simulation Start!!"<<endl;
    list.display();//display the begin status
    
    
}

void event()
{
    int i = 1;
    currenttime = incrementtime;
    for (; currenttime < stoptime; currenttime += incrementtime)//simulate the program to the stop time
    {
        if(list.event())//main process
        {
            list.display();//any bus will produce a display
        }
        if (currenttime % 3600 == 0)
        {
            cout << i++ << " hour passed! " << endl;//every hour, display the all the information
            list.display();
        }
    }
    list.display();
    cout<<"Left Passengers List for each station: "<<endl;
    int tem=list.restpassenger();
    cout<<"In sum, "<<tem<<" passengers are still on the station!"<<endl;

    
}
int main()
{
    testbigger = 0;
    testless = 0;
    initialization();//initialize modul
    
    event();//simulation modul
    
    cout<<"Total passenger: "<<totalpassenger<<endl<<"Total boarding time: "<<boardtime<<endl<<"Everage passengers per board: "<<totalpassenger/boardtime<<endl;
    cout<<"Max passenger: "<<maxpassenger<<endl;
    cout<<"----------------------------------------"<<endl<<"Simulation Finished!"<<endl;
    return 0;
}