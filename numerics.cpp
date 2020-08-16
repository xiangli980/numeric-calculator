#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <queue>

//this header include "integral.h" and "expr.h"
#include "gradient.h"

void skipSpace(const char ** strp) {
  while (isspace(**strp)) {
    *strp = *strp + 1;
  }
}

//assume to be an operator : a sequence of characters before a space
string findOp(const char ** strp) {
  string newexp = "";  //)
  skipSpace(strp);
  while (!isspace(**strp)) {
    newexp += **strp;
    *strp = *strp + 1;
  }
  return newexp;
}

//id: only contain letters fellowed by a space or a ')'
string findId(const char ** strp) {
  string newid = "";
  skipSpace(strp);
  while (isalpha(**strp)) {  //)
    newid += **strp;
    *strp = *strp + 1;
  }
  if (**strp != ')' && !isspace(**strp)) {
    cout << "not a valid id" << endl;
    return "";
  }
  return newid;
}

// return biEexpression
Expression * makeBExpr(string op, Expression * lhs, Expression * rhs) {
  if (op == "+")
    return new PlusExpression(lhs, rhs);
  if (op == "-")
    return new MinusExpression(lhs, rhs);
  if (op == "*")
    return new TimesExpression(lhs, rhs);
  if (op == "/")
    return new DivExpression(lhs, rhs);
  if (op == "%")
    return new ModExpression(lhs, rhs);
  if (op == "pow")
    return new PowExpression(lhs, rhs);
  std::cerr << "Impossible op: " << op << "\n";
  abort();
}

// return unExpression
Expression * makeUExpr(string op, Expression * rhs) {
  if (op == "sin")
    return new SinExpression(rhs);
  if (op == "cos")
    return new CosExpression(rhs);
  if (op == "ln")
    return new LnExpression(rhs);
  if (op == "sqrt")
    return new SqrtExpression(rhs);

  std::cerr << "Impossible op: " << op << "\n";
  abort();
}

bool isValidOp(string s, vector<string> validOp) {
  return (find(validOp.begin(), validOp.end(), s) != validOp.end());
}

bool isValidFun(string s, map<string, Function *> validFun) {
  return (validFun.find(s) != validFun.end());
}

//check if ids in expression all belong to this new function
bool checkId(const char ** check,
             vector<string> ids,
             map<string, Function *> validFun,
             vector<string> validOp) {
  //if read #, expression ends
  while (**check != '\0' && **check != '#') {
    if (!isalpha(**check)) {
      *check = *check + 1;
      continue;
    }
    //if read a letter, then find an id
    string newid = findId(check);
    if (!isValidFun(newid, validFun) && !isValidOp(newid, validOp)) {
      if (find(ids.begin(), ids.end(), newid) == ids.end()) {
        return false;
      }
    }
  }

  return true;
}

//return a expression of this prev-defined function with ids of new function
Expression * parseFun(string fname,
                      const char ** strp,
                      string fcur,
                      map<string, Function *> validFun,
                      vector<string> validOp) {
  Function * prevf = validFun[fname];
  size_t cnt = prevf->getNum();
  size_t i = 0;

  //collect new expr for ids in prev function
  //prev define: (fname x y)
  //when read (fname exp0 exp1)
  //fill in map <"x","exp0"> and  <"y","exp1">
  while (i < cnt) {
    Expression * expi = parse(strp, fcur, validFun, validOp);
    //if lack expi for this function's ids[i]
    if (expi == NULL) {
      cerr << "lack expr for some id\n";
      return NULL;
    }
    string substr = expi->toString();
    prevf->setSub(prevf->getIdi(i), substr);
    delete expi;
    i++;
  }
  skipSpace(strp);
  if (**strp != ')') {
    cerr << "expr need a ), but find " << *strp << "\n";
    return NULL;
  }
  *strp = *strp + 1;

  //parse this function, using substr
  string fexp = prevf->getExpr();
  const char * fexpr = fexp.c_str();
  //here we pass in fname for this prev-defineed function
  //it means : we will use substr for this function's ids
  Expression * newExp = parse(&fexpr, fname, validFun, validOp);
  //return new exp for new function
  return newExp;
}

//return an opExpression or an Expression for a function
Expression * parseOp(const char ** strp,
                     string fname,
                     map<string, Function *> validFun,
                     vector<string> validOp) {
  skipSpace(strp);
  //find the first expr after '('
  //can be an op or a fname
  string op = findOp(strp);
  if (!isValidOp(op, validOp)) {
    //if not an op
    //check if it is a fname
    if (isValidFun(op, validFun))
      //go to parse function
      return parseFun(op, strp, fname, validFun, validOp);

    std::cerr << "Invalid op: " << op << "\n";
    return NULL;
  }

  //op with one parameter
  if (op == "sin" || op == "cos" || op == "ln" || op == "sqrt") {
    Expression * rhs = parse(strp, fname, validFun, validOp);
    if (rhs == NULL) {
      return NULL;
    }
    skipSpace(strp);
    //parse successful
    if (**strp == ')') {
      *strp = *strp + 1;
      return makeUExpr(op, rhs);
    }
    std::cerr << "Expected ) but found " << *strp << "\n";
    delete rhs;
    return NULL;
  }
  else {
    //op with two parameter
    Expression * lhs = parse(strp, fname, validFun, validOp);
    if (lhs == NULL) {
      return NULL;
    }
    Expression * rhs = parse(strp, fname, validFun, validOp);
    if (rhs == NULL) {
      delete lhs;
      return NULL;
    }
    skipSpace(strp);
    // parse successful
    if (**strp == ')') {
      *strp = *strp + 1;
      return makeBExpr(op, lhs, rhs);
    }
    std::cerr << "Expected ) but found " << *strp << "\n";
    delete lhs;
    delete rhs;
    return NULL;
  }
}

//parse a whole expression
//if fname is " ", then id is in a normal condition
//else , id will be replaced with substr
Expression * parse(const char ** strp,
                   string fname,
                   map<string, Function *> validFun,
                   vector<string> validOp) {
  skipSpace(strp);
  // check condition of the exp
  if (**strp == '\0' || **strp == '#') {
    std::cerr << "End of line found mid expression!\n";
    return NULL;
  }
  else if (**strp == '(') {
    // (op E E) or (fname id id)
    *strp = *strp + 1;
    return parseOp(strp, fname, validFun, validOp);
  }
  else if (isalpha(**strp)) {
    // id
    string newid = findId(strp);
    //not valid id
    if (newid == "") {
      return NULL;
    }

    //is in a prev function substitute condition
    if (!fname.empty()) {
      //generate substitute expression from substr
      if (isValidFun(fname, validFun)) {
        string substr = validFun[fname]->getSub(newid);
        const char * subs = substr.c_str();
        Expression * sub = parse(&subs, "", validFun, validOp);
        return sub;
      }
    }
    //not in a replace condition, just return newid
    return new IdExpression(newid);
  }
  else {
    //constant number
    char * endp;
    double num = strtod(*strp, &endp);
    if (endp == *strp) {
      std::cerr << "Expected a number, but found " << *strp << "\n";
      return NULL;
    }
    *strp = endp;
    //check if there is other characters after number
    if (!isspace(**strp) && **strp != '\0' && **strp != ')') {
      cerr << "not a valid number\n";
      return NULL;
    }
    return new NumExpression(num);
  }
}

//find repeat id when define
bool existId(string id, vector<string> ids) {
  if (find(ids.begin(), ids.end(), id) == ids.end()) {
    return false;
  }
  return true;
}

//bild up a function object(fname+ids+Expression*+expression string)
Function * defFun(const char ** temp, map<string, Function *> validFun, vector<string> validOp) {
  skipSpace(temp);
  if (**temp != '(') {
    cerr << "need a (" << endl;
    return NULL;
  }
  *temp = *temp + 1;
  //fname
  string fname = findId(temp);
  if (fname.empty()) {
    cerr << "invalid fname" << endl;
    return NULL;
  }
  if (isValidFun(fname, validFun)) {
    cerr << "redefined fname" << endl;
    return NULL;
  }
  //collect all valid variables
  vector<string> ids;
  string id;
  // if find an identifier
  while (!(id = findId(temp)).empty()) {
    if (existId(id, ids)) {
      cerr << "id can't be repeated" << endl;
      return NULL;
    }
    ids.push_back(id);
  }
  //read line at ")"
  if (ids.size() == 0) {
    cerr << "need at least one id" << endl;
    return NULL;
  }
  skipSpace(temp);
  if (**temp != ')') {
    cerr << "define need a )" << endl;
    return NULL;
  }
  // read "="
  *temp = *temp + 1;
  skipSpace(temp);
  if (**temp != '=') {
    cerr << "define need a  =" << endl;
    return NULL;
  }
  *temp = *temp + 1;

  //parse expression for this function
  string fexpr = *temp;
  //check if id in expression belongs to this new function
  const char * check = *temp;
  if (!checkId(&check, ids, validFun, validOp)) {
    cerr << " invalid id or surplus character" << endl;
    return NULL;
  }

  // parse expression
  Expression * newf = parse(temp, fname, validFun, validOp);

  //check if there is other characters after a whole expression
  skipSpace(temp);
  //find invalid character
  if (**temp != '#' && **temp != '\0') {
    delete newf;
    newf = NULL;
    cerr << "surplus characters after whole expression";
  }
  if (newf != NULL) {
    return new Function(fname, ids, fexpr, newf);
  }
  else {
    //cout << "fail to parse expression\n";
    return NULL;
  }
}

double calcuFval(const char ** strp, map<string, Function *> validFun) {
  //get fname
  string fname = findOp(strp);
  if (!isValidFun(fname, validFun)) {
    cout << "not a valid function" << endl;
    exit(EXIT_FAILURE);
  }
  Function * f = validFun[fname];
  size_t len = f->getNum();
  size_t i = 0;
  //collect all values for function's ids
  //(can only be constant or f(constant))
  map<string, double> subval;
  while (i < len) {
    double val = testFun(strp, validFun);
    subval[f->getIdi(i)] = val;
    i++;
  }
  skipSpace(strp);
  //go to ')'
  if (**strp != ')') {
    cout << "expect ) but found " << *strp << endl;
    exit(EXIT_FAILURE);
  }
  *strp = *strp + 1;
  //calculate answer for this function
  double ans = f->getFreal()->evaluate(subval);
  return ans;
}

double testFun(const char ** strp, map<string, Function *> validFun) {
  skipSpace(strp);
  if (**strp == '(') {
    // is f, return ans of the f
    *strp = *strp + 1;
    return calcuFval(strp, validFun);
  }
  else {
    //is constant, return the constant double
    char * endp;
    double num = strtod(*strp, &endp);
    if (endp == *strp) {
      std::cerr << "expect constant but find  " << *strp << "\n";
      exit(EXIT_FAILURE);
    }

    *strp = endp;
    return num;
  }
}

vector<string> initOp() {
  vector<string> validOp;
  validOp.push_back("+");
  validOp.push_back("-");
  validOp.push_back("*");
  validOp.push_back("/");
  validOp.push_back("%");
  validOp.push_back("sin");
  validOp.push_back("cos");
  validOp.push_back("pow");
  validOp.push_back("sqrt");
  validOp.push_back("ln");
  return validOp;
}

void printFUN(Function * newf) {
  cout << newf->getName() << "(";
  size_t i = 0;
  while (i < newf->getNum() - 1) {
    cout << newf->getIdi(i) << " ";
    i++;
  }
  cout << newf->getIdi(i);
  cout << ")" << endl;
}

void printTEST(const char ** ans, const char ** end, double actual, double expect) {
  skipSpace(ans);
  //if the first argument is a constant
  if (**ans != '(') {
    string value = findOp(ans);
    cout << value << " = " << expect;
  }
  // if the first argument is function
  else {
    *ans = *ans + 1;
    string ff = findOp(ans);
    cout << ff << "(";
    while (*ans != *end) {
      cout << **ans;
      *ans = *ans + 1;
    }
    cout << " = " << expect;
  }
  if (abs(actual - expect) < 0.0000000000001) {
    cout << " [correct]" << endl;
  }
  else {
    cout << " [INCORRECT: expected " << actual << "]" << endl;
    exit(EXIT_FAILURE);
  }
}
int main() {
  //initial valid operation and pre-defined function
  vector<string> validOp = initOp();
  //initial a space for user-defined function
  map<string, Function *> validFun;
  int exit = 1;
  //read command line by line
  char * line = NULL;
  size_t sz;
  while (getline(&line, &sz, stdin) != -1) {
    const char * tem = line;
    const char ** temp = &tem;
    skipSpace(temp);
    // if this line begin with # or is nothing, then skip
    if (**temp == '#' || **temp == '\0')
      continue;

    // find a command
    string arg = findId(temp);
    //define a function
    if (arg.compare("define") == 0) {
      Function * newf = defFun(temp, validFun, validOp);
      //get a real Function object
      if (newf != NULL) {
        validFun[newf->getName()] = newf;
        //print result
        printFUN(newf);
      }
      else {
        // if newf is NULL,exit
        exit = 0;
        break;
      }
    }
    //test an expression
    else if (arg.compare("test") == 0) {
      const char * ans = *temp;
      double actual = testFun(temp, validFun);
      //record the end of first argument
      const char * end = *temp;
      skipSpace(temp);
      //second argument
      //get a constant(expected value)
      char * endp;
      double expect = strtod(*temp, &endp);
      if (endp == *temp) {
        std::cerr << "expected a constant\n";
        exit = 0;
        break;
      }
      *temp = endp;
      skipSpace(temp);
      //check if there is something after the constant
      if (**temp != '#' && **temp != '\0') {
        cerr << "not a valid test argument\n";
        exit = 0;
        break;
      }
      //print out result
      printTEST(&ans, &end, actual, expect);
    }
    else if (arg.compare("numint") == 0) {
      double numI = parseIntegral(temp, validFun, 0);
      cout << "numint " << numI << endl;
    }
    else if (arg.compare("mcint") == 0) {
      double mcI = parseIntegral(temp, validFun, 1);
      cout << "mcint " << mcI << endl;
    }
    else if (arg.compare("max") == 0) {
      parseGradient(temp, validFun, 1);
    }
    else if (arg.compare("min") == 0) {
      parseGradient(temp, validFun, 0);
    }
    else {
      cerr << "not a valid command\n";
      exit = 0;
      break;
    }
  }

  free(line);
  //delete all Expression object
  for (map<string, Function *>::iterator it = validFun.begin(); it != validFun.end(); ++it) {
    delete it->second;
  }
  //wrong input , exit failure
  if (!exit)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
