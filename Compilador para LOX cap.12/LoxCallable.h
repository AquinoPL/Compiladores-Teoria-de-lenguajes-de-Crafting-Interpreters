#pragma once

#include <any>
#include <string>
#include <vector>
using namespace std;
class Interpreter;

class LoxCallable {
public:
  virtual int arity() = 0;
  virtual any call(Interpreter& interpreter,
                        vector<any> arguments) = 0;
  virtual string toString() = 0;
  virtual ~LoxCallable() = default;
};