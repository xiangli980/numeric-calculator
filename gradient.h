#include "integral.h"

double derive(string f,
              map<string, Function *> validFun,
              vector<double> start,
              double delta,
              size_t para);

vector<double> grad(string f, map<string, Function *> validFun, vector<double> start);
double pDis(vector<double> pcur, vector<double> pnew);
vector<double> gradAscent(string f,
                          double gamma,
                          double convDis,
                          map<string, Function *> validFun,
                          vector<double> start,
                          int maxStep,
                          int isMax,
                          int & isConv);
void parseGradient(const char ** temp, map<string, Function *> validFun, int isMax);
