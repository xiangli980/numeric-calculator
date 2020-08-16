#include "integral.h"

#include <stdlib.h>

#include <iostream>
#include <vector>

using namespace std;

//get a double number
double getNum(const char ** temp) {
  skipSpace(temp);
  char * endp;
  double expect = strtod(*temp, &endp);
  if (endp == *temp) {
    std::cerr << "expected a number"
              << "\n";
    exit(EXIT_FAILURE);
  }
  *temp = endp;
  return expect;
}

//turn the argument into a expr string for the test function in step 1
string makeTestExp(vector<double> var, string f) {
  string test = "(";
  test += f;
  for (size_t i = 0; i < var.size(); ++i) {
    test += " ";
    test += to_string(var[i]);
  }
  test += ")";
  return test;
}

//calculate the volume of one prism starting from one point
double prism(vector<double> ipoint, string f, double step, map<string, Function *> validFun) {
  size_t nbit = ipoint.size();
  double prismH = 0;

  //find all neibor points and calculate function value(every side)
  for (size_t b = 0; b < pow(2, nbit); b++) {
    vector<double> neipoint = ipoint;
    // use bit method to get neibor point(00 01 10 11)
    // if the bit is "1" , then add step
    for (size_t k = 0; k < nbit; k++) {
      if (b & (1 << k)) {
        neipoint[k] += step;
      }
    }
    string s = makeTestExp(neipoint, f);
    const char * testExp = s.c_str();
    // use test function in step 1 to calculate the value of function at neipoint
    prismH += testFun(&testExp, validFun);
  }
  //get average height of each side and multiple base area
  //this is the volume
  double ans = prismH / pow(2, nbit) * pow(step, nbit);
  return ans;
}

// add all prisms
// use recursion to run an N-dimension loop
void sumPrism(size_t loop,
              double step,
              vector<pair<double, double> > range,
              vector<double> var,
              double & sum,
              string f,
              map<string, Function *> validFun) {
  //loop is the layer number, start from 0 -- the most outside layer
  // in every layer we get the value of one parameter
  for (double i = range[loop].first; i < range[loop].second; i = i + step) {
    //get value of (loop)th parameter
    //put it in var (the point to compute)
    var[loop] = i;

    if (loop < range.size() - 1) {
      sumPrism(loop + 1, step, range, var, sum, f, validFun);
    }

    if (loop != range.size() - 1)
      continue;
    // if in last loop, we get the complete point- var
    //then compute the volume of prism start from this point
    sum += prism(var, f, step, validFun);
  }
}

//Monte Carlo Integration
double sumMonte(string f,
                int trial,
                vector<pair<double, double> > range,
                map<string, Function *> validFun) {
  double avg = 0;
  //the point to compute
  vector<double> point;
  point.resize(range.size());
  //under largest allowed trial
  for (int i = 0; i < trial; i++) {
    // get random point for every variable
    for (size_t j = 0; j < range.size(); j++) {
      double low = range[j].first;
      double high = range[j].second;
      double varj = (rand() / (RAND_MAX / (high - low))) + low;
      point[j] = varj;
    }
    //compute fvalue at this point
    string s = makeTestExp(point, f);
    const char * testExp = s.c_str();
    avg += testFun(&testExp, validFun);
  }
  //average height
  avg = avg / trial;
  double sum = avg;
  // compute volume
  for (size_t j = 0; j < range.size(); j++) {
    sum = sum * (range[j].second - range[j].first);
  }

  return sum;
}

double parseIntegral(const char ** strp, map<string, Function *> validFun, int isMC) {
  //get function
  string f = findId(strp);
  if (!isValidFun(f, validFun)) {
    cout << "no such function" << endl;
    exit(EXIT_FAILURE);
  }

  //get step size/trials
  double step = getNum(strp);
  vector<pair<double, double> > range;
  //get range for all parameters
  for (size_t i = 0; i < validFun[f]->getNum(); i++) {
    double begin = getNum(strp);
    double end = getNum(strp);
    if (begin >= end) {
      cout << "bound error" << endl;
      exit(EXIT_FAILURE);
    }
    range.push_back(make_pair(begin, end));
    skipSpace(strp);
  }
  // find surplus character,exit
  if (**strp != '#' && **strp != '\0') {
    cerr << "find invlid characters\n";
    exit(EXIT_FAILURE);
  }

  //Numerical Integration
  if (!isMC) {
    if (step <= 0) {
      cerr << "step value should > 0\n";
      exit(EXIT_FAILURE);
    }
    double sum = 0;
    //var is current point, value of parameters are put in var in order
    //range is the low and up bound of every parameter
    vector<double> var;
    var.resize(range.size());
    sumPrism(0, step, range, var, sum, f, validFun);
    return sum;
  }
  else {
    //Monte carlo
    int trial = int(step);
    //if trial is not int or <=0 ,exit
    if (trial <= 0 || abs(step - trial) > 0.0000001) {
      cerr << "not valid trial number\n";
      exit(EXIT_FAILURE);
    };

    double sum = sumMonte(f, trial, range, validFun);
    return sum;
  }
}
