//
//  main.cpp
//  week 10
//
//  Created by Yanqiao Zhan on 10/30/14.
//  Copyright (c) 2014 Yanqiao Zhan. All rights reserved.
//

#include <iostream>

using namespace std;

class Tool {
    /* Fill in */
public:
    void setStrength(int s)
    {
        strength=s;
    }
    char getType()
    {
        return type;
    }
    int getStrength()
    {
        return strength;
    }
    virtual bool fight(Tool tool){return false;}
protected:
    int strength;
    char type;
};
/*
	Implement class Scissors
 */

class Scissors:public Tool
{
public:
    Scissors(int i)
    {
        strength=i;
        type='s';
    }
    bool fight(Tool tool)
    {
        if(tool.getType()=='p')
        {
            if(strength*2>=tool.getStrength())
            {
                return true;
            }
            else return false;
        }
        else if(tool.getType()=='r')
        {
            if(static_cast<double>(strength/2)>=tool.getStrength())
            {
                return true;
            }
            else return false;
        }
        else return true;
    }
};
/*
	Implement class Paper
 */
class Paper:public Tool
{
public:
    Paper(int i)
    {
        strength=i;
        type='p';
    }
    bool fight(Tool tool)
    {
        if(tool.getType()=='r')
        {
            if(strength*2>=tool.getStrength())
            {
                return true;
            }
            else return false;
        }
        else if(tool.getType()=='s')
        {
            if(static_cast<double>(strength/2)>=tool.getStrength())
            {
                return true;
            }
            else return false;
        }
        else return true;
    }
    
    
};

/*
	Implement class Rock
 */
class Rock:public Tool
{
public:
    Rock(int i)
    {
        strength=i;
        type='r';
    }
    bool fight(Tool tool)
    {
        if(tool.getType()=='s')
        {
            if(strength*2>=tool.getStrength())
            {
                return true;
            }
            else return false;
        }
        else if(tool.getType()=='p')
        {
            if(static_cast<double>(strength/2)>=tool.getStrength())
            {
                return true;
            }
            else return false;
        }
        else return true;
    }
    
};

int main() {
    // Example main function
    // You may add your own testing code if you like
    
    Scissors s1(5);
    Paper p1(7);
    Rock r1(15);
    cout << s1.fight(p1) << p1.fight(s1) << endl;
    cout << p1.fight(r1) << r1.fight(p1) << endl;
    cout << r1.fight(s1) << s1.fight(r1) << endl;
    
    return 0;
}