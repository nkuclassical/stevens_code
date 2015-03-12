//
//  main.cpp
//  week 11
//
//  Created by Yanqiao Zhan on 11/6/14.
//  Copyright (c) 2014 Yanqiao Zhan. All rights reserved.
//

#include <iostream>
using namespace std;

template<class T>
class my_init_arry
{
private:
    
    size_t Size;
    T *array;
public:
    my_init_arry(const my_init_arry &other,size_t size)
    {
        array=new T[size];
        Size=size;
        for(int i=0;i<Size;i++)
        {
            *(array+i)=*(other.array+i);
        }
        Size=size;
    }
    my_init_arry(){Size=0;}
    my_init_arry(size_t size)
    {
        Size=size;
        array=new T[size];
        for(size_t i=0;i<size;i++)
        {
            *(array+i)=0;
        }
    }
    my_init_arry(const my_init_arry &other)
    {
        
        Size = other.size();
        array=new T[Size];
        for(size_t i=0;i<Size;i++)
        {
            *(array+i)=other.array[i];
        }
    }
    
    T& at(size_t pos)
    {
        if(pos>=Size||pos<0)
            throw "error";
        else
            return array[pos];
    }
    size_t size()const{return Size;}
    ~my_init_arry(){if(array!=nullptr)delete array;}
    
};

int main(int argc, const char * argv[]) {
    
    my_init_arry<int> arr(10);
    
    for(int i=0;i<arr.size();i++)
        arr.at(i)=(i+1)*2;
    cout<<"------Lab of simple Array-----"<<endl;
    cout<<"a has 10 elements, 2,4,6,...,20"<<endl;
    cout<<"assgin b as a's first two elemets"<<endl;
    my_init_arry<int> brr(arr,2);
    for(int i=0;i<brr.size();i++)
        cout<<brr.at(i)<<endl;
    cout<<"--------"<<endl;
    cout<<"construct c by copy a"<<endl;
    my_init_arry<int> crr(arr);
    for(int i=0;i<crr.size();i++)
        cout<<crr.at(i)<<endl;
    
    return 0;
}

