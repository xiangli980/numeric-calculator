#include <math.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class Expression
{
 public:
  virtual string toString() const = 0;
  virtual double evaluate(map<string, double> val) const = 0;
  virtual ~Expression() {}
};

class NumExpression : public Expression
{
  double num;

 public:
  NumExpression(double n) : num(n) {}
  virtual string toString() const {
    stringstream ss;
    ss << " " << num << " ";
    return ss.str();
  }
  virtual double evaluate(map<string, double> val) const {
    double ans = num;
    if (val.size() > 0) {
      return ans;
    }
    return ans;
  }
};

class IdExpression : public Expression
{
  string id;

 public:
  IdExpression(string id) : id(id) {}
  virtual string toString() const {
    stringstream ss;
    ss << " " << id << " ";
    return ss.str();
  }
  virtual double evaluate(map<string, double> val) const { return val[id]; }
};

class biExpression : public Expression
{
 protected:
  const char * op;
  Expression * lhs;
  Expression * rhs;

 public:
  biExpression(const char * op, Expression * lhs, Expression * rhs) : op(op), lhs(lhs), rhs(rhs) {}
  virtual string toString() const {
    stringstream ss;
    // output for substitute
    ss << " ( " << op << " " << lhs->toString() << " " << rhs->toString() << " ) ";
    return ss.str();
  }
  virtual ~biExpression() {
    delete lhs;
    delete rhs;
  }
};

class unExpression : public Expression
{
 protected:
  const char * op;
  Expression * rhs;

 public:
  unExpression(const char * op, Expression * rhs) : op(op), rhs(rhs) {}
  virtual string toString() const {
    stringstream ss;
    ss << " ( " << op << " " << rhs->toString() << " ) ";
    return ss.str();
  }
  virtual ~unExpression() { delete rhs; }
};

class PlusExpression : public biExpression
{
 public:
  PlusExpression(Expression * lhs, Expression * rhs) : biExpression("+", lhs, rhs) {}
  virtual double evaluate(map<string, double> val) const {
    return lhs->evaluate(val) + rhs->evaluate(val);
  }
};

class MinusExpression : public biExpression
{
 public:
  MinusExpression(Expression * lhs, Expression * rhs) : biExpression("-", lhs, rhs) {}
  virtual double evaluate(map<string, double> val) const {
    return lhs->evaluate(val) - rhs->evaluate(val);
  }
};

class TimesExpression : public biExpression
{
 public:
  TimesExpression(Expression * lhs, Expression * rhs) : biExpression("*", lhs, rhs) {}
  virtual double evaluate(map<string, double> val) const {
    return lhs->evaluate(val) * rhs->evaluate(val);
  }
};

class DivExpression : public biExpression
{
 public:
  DivExpression(Expression * lhs, Expression * rhs) : biExpression("/", lhs, rhs) {}
  virtual double evaluate(map<string, double> val) const {
    return lhs->evaluate(val) / rhs->evaluate(val);
  }
};

class ModExpression : public biExpression
{
 public:
  ModExpression(Expression * lhs, Expression * rhs) : biExpression("%", lhs, rhs) {}
  virtual double evaluate(map<string, double> val) const {
    return fmod(lhs->evaluate(val), rhs->evaluate(val));  //fmod
  }
};

class PowExpression : public biExpression
{
 public:
  PowExpression(Expression * lhs, Expression * rhs) : biExpression("pow", lhs, rhs) {}
  virtual double evaluate(map<string, double> val) const {
    return pow(lhs->evaluate(val), rhs->evaluate(val));
  }
};

class SinExpression : public unExpression
{
 public:
  SinExpression(Expression * rhs) : unExpression("sin", rhs) {}
  virtual double evaluate(map<string, double> val) const { return sin(rhs->evaluate(val)); }
};
class CosExpression : public unExpression
{
 public:
  CosExpression(Expression * rhs) : unExpression("cos", rhs) {}
  virtual double evaluate(map<string, double> val) const { return cos(rhs->evaluate(val)); }
};
class SqrtExpression : public unExpression
{
 public:
  SqrtExpression(Expression * rhs) : unExpression("sqrt", rhs) {}
  virtual double evaluate(map<string, double> val) const { return sqrt(rhs->evaluate(val)); }
};
class LnExpression : public unExpression
{
 public:
  LnExpression(Expression * rhs) : unExpression("ln", rhs) {}
  virtual double evaluate(map<string, double> val) const { return log(rhs->evaluate(val)); }
};

class Function
{
  string fname;
  vector<string> ids;
  string fexpr;
  Expression * freal;
  map<string, string> substr;
  map<string, double> subval;

 public:
  Function(string fname, vector<string> ids, string fexpr, Expression * f) :
      fname(fname),
      ids(ids),
      fexpr(fexpr),
      freal(f) {}
  size_t getNum() { return ids.size(); }
  string getIdi(int i) { return ids[i]; }
  string getExpr() { return fexpr; }
  string getName() { return fname; }
  void setSub(string x, string expx) { substr[x] = expx; }
  string getSub(string x) { return substr[x]; }
  Expression * getFreal() { return freal; }
  ~Function() { delete freal; }
};

void skipSpace(const char ** strp);
string findOp(const char ** strp);
string findId(const char ** strp);
Expression * makeBExpr(string op, Expression * lhs, Expression * rhs);
Expression * makeUExpr(string op, Expression * rhs);
bool isValidOp(string s, vector<string> validOp);
bool isValidFun(string s, map<string, Function *> validFun);
bool checkId(const char ** check,
             vector<string> ids,
             map<string, Function *> validFun,
             vector<string> validOp);
Expression * getEx(const char ** strp,
                   string fcur,
                   map<string, Function *> validFun,
                   vector<string> validOp);
Expression * parseFun(string fname,
                      const char ** strp,
                      string fcur,
                      map<string, Function *> validFun,
                      vector<string> validOp);

Expression * parseOp(const char ** strp,
                     string fname,
                     map<string, Function *> validFun,
                     vector<string> validOp);
Expression * parse(const char ** strp,
                   string fname,
                   map<string, Function *> validFun,
                   vector<string> validOp);
bool existId(string id, vector<string> ids);
Function * defFun(const char ** temp, map<string, Function *> validFun, vector<string> validOp);
double parseFval(const char ** strp, map<string, Function *> validFun);
double testFun(const char ** strp, map<string, Function *> validFun);
