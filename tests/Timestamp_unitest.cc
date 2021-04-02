#include"../Timestamp.h"
#include<iostream>
#include<ctime>
int main(){
    //conex::Timestamp now = conex::now();
    //std::time_t t = std::chrono::system_clock::to_time_t(now);
    //std::cout<<"time is"<<std::ctime(&t)<<std::endl;
    std::cout<<conex::time::toString(conex::time::now())<<std::endl;
    conex::Timestamp ti = conex::time::now();
    std::cout<<conex::time::toString(ti)<<std::endl;
    return 0;
}