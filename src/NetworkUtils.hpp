#ifndef NETWORK_UTILS_HPP
#define NETWORK_UTILS_HPP

#include <cstdint>
#include <arpa/inet.h>
#include <cstring>
#include <vector>
#include <type_traits>


using namespace std;

template<template<typename...> class Template, typename... Args>
struct is_specialization : std::false_type {};

template<template<typename...> class Template, typename... Args>
struct is_specialization<Template, Template<Args...>> : std::true_type {};

template<class T1, class T2>
void WriteToNetwork(vector<T1> &v, T2 value) {

    if constexpr (is_specialization<std::vector, T2>::value) {
        v.insert(v.end(), value.begin(), value.end());
    } 
    else {
         int op =  sizeof(value);
            if(op == 1)
                v.push_back(value);
            else if(op == 2) {
                v.push_back((value >> 8));
                v.push_back(value & 0xFF);
            }
            else if(op == 4) {
                v.push_back((value >> 24));
                v.push_back((value >> 16) & 0xFF);
                v.push_back((value >> 8) & 0xFF);
                v.push_back((value & 0xFF));
            }
     }
}

#include <netinet/in.h>
#include <string>

struct IPEndPoint {
    string Address;
    int Port;

    IPEndPoint() : Address(""), Port(0) {}  // Constructor por defecto

    IPEndPoint(const string& addr, int port)
        : Address(addr), Port(port) {}
};

#endif
