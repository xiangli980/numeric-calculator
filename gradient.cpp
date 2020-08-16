#include "integral.h"

//partial derivative
//compute dy/dx for a specific delta for a parameter
double derive(string f,
              map<string, Function *> validFun,
              vector<double> start,
              double delta,
              size_t para) {
  //f(x)
  string exp = makeTestExp(start, f);
  const char * fcurr = exp.c_str();
  double f0 = testFun(&fcurr, validFun);
  //f(x+delta)
  vector<double> newp;
  newp.resize(start.size());
  newp = start;
  newp[para] = start[para] + delta;
  string exp1 = makeTestExp(newp, f);
  const char * fdelta = exp1.c_str();
  double f1 = testFun(&fdelta, validFun);
  //derivative
  double lim = (f1 - f0) / delta;
  return lim;
}

// compute gradient vector for f at point start
vector<double> grad(string f, map<string, Function *> validFun, vector<double> start) {
  //initial a gradient vector
  vector<double> dy;
  dy.resize(start.size());
  //compute gradient part for every parameter
  for (size_t i = 0; i < start.size(); i++) {
    //first assume delta to be 0.01, then reduce it to find the exact gradient
    double delta = 0.01;
    double dy0 = derive(f, validFun, start, delta, i);
    while (1) {
      delta = delta / 2;
      double dy1 = derive(f, validFun, start, delta, i);
      //when two partial derivative are close enough
      //it means it is the gradient part for this parameter
      double dis = abs(dy1 - dy0);
      if (dis < 0.001)
        break;
      dy0 = dy1;
    }
    //fill in the gradient vector for ith parameter
    dy[i] = dy0;
  }
  return dy;
}

//compute distance betweem points
double pDis(vector<double> pcur, vector<double> pnew) {
  double dis = 0;
  size_t n = pcur.size();
  for (size_t i = 0; i < n; i++) {
    dis += (pcur[i] - pnew[i]) * (pcur[i] - pnew[i]);
  }
  dis = sqrt(dis);
  return dis;
}

// find a max/min point
vector<double> gradAscent(string f,
                          double gamma,
                          double convDis,
                          map<string, Function *> validFun,
                          vector<double> start,
                          int maxStep,
                          int isMax,
                          int & isConv) {
  vector<double> pcur = start;
  vector<double> pnew;
  size_t n = start.size();
  pnew.resize(n);
  int cnt = 0;
  while (cnt < maxStep) {
    vector<double> df = grad(f, validFun, pcur);
    //pnew
    for (size_t i = 0; i < n; i++) {
      //ascent
      if (isMax)
        pnew[i] = pcur[i] + gamma * df[i];
      else
        //descent
        pnew[i] = pcur[i] - gamma * df[i];
    }
    // check converge
    double dis = pDis(pcur, pnew);
    if (dis < convDis)
      break;

    pcur = pnew;
    cnt++;
  }
  if (cnt == maxStep) {
    // not converge
    isConv = 0;
  }
  return pcur;
}

//parse gradient parameter, print result
void parseGradient(const char ** strp, map<string, Function *> validFun, int isMax) {
  // fname
  string f = findId(strp);
  if (!isValidFun(f, validFun)) {
    cerr << "not a valid function\n";
    exit(EXIT_FAILURE);
  }
  //gamma > 0
  double gamma = getNum(strp);
  if (gamma <= 0) {
    cerr << "gamma should > 0 \n";
    exit(EXIT_FAILURE);
  }
  // convergedDistance > 0
  double convDis = getNum(strp);
  if (convDis <= 0) {
    cerr << "convDis should > 0 \n";
    exit(EXIT_FAILURE);
  }
  //get start point
  size_t n = validFun[f]->getNum();
  vector<double> start;
  start.resize(n);
  for (size_t i = 0; i < n; i++) {
    double parai = getNum(strp);
    start[i] = parai;
  }
  //maxstep>0  should be integer
  double Step = getNum(strp);
  int maxStep = (int)Step;
  if (abs(maxStep - Step) > 0.000001 || maxStep <= 0) {
    cerr << "not a valid maxStep\n";
    exit(EXIT_FAILURE);
  }
  //check surplus
  skipSpace(strp);
  if (**strp != '#' && **strp != '\0') {
    cerr << "have surplus characters\n";
    exit(EXIT_FAILURE);
  }

  //if isMax = 1 ,find point for max value
  //if isMax = 0 , find point for min value

  //set a converge notice
  int isConv = 1;
  //return a point
  vector<double> maxP = gradAscent(f, gamma, convDis, validFun, start, maxStep, isMax, isConv);

  //if converge
  //compute max/min value at that point

  if (isConv) {
    string exp = makeTestExp(maxP, f);
    const char * s = exp.c_str();
    double fmax = testFun(&s, validFun);
    cout << "find a converge point: " << exp << " = " << fmax << endl;
  }
  else {
    // not converge, print stop point
    string exp = makeTestExp(maxP, f);
    cout << "not converge, stop at: " << exp << endl;
  }
  return;
}
