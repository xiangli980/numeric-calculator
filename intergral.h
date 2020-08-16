#include <iostream>
#include <vector>

#include "expr.h"
using namespace std;

string makeTestExp(vector<double> var, string f);
double prism(vector<double> vrange, string f, double step, map<string, Function *> validFun);
double getNum(const char ** temp);

void sumPrism(size_t loop,
              double step,
              vector<pair<double, double> > range,
              vector<double> var,
              double & sum,
              string f,
              map<string, Function *> validFun);
double sumMonte(string f,
                int trial,
                vector<pair<double, double> > range,
                map<string, Function *> validFun);

double parseIntegral(const char ** strp, map<string, Function *>, int isMC);
