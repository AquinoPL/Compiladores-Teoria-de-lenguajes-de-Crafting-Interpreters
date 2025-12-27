#pragma once

#include <string>
#include <vector>
#include <any>
#include <map>
#include <memory>
#include "LoxCallable.h"
using namespace std;

class Interpreter;
class LoxFunction;

class LoxClass: public LoxCallable,
                public enable_shared_from_this<LoxClass> {
  friend class LoxInstance;
  const string name;
  const shared_ptr<LoxClass> superclass;
  map<string, shared_ptr<LoxFunction>> methods;

public:
  LoxClass(string name, shared_ptr<LoxClass> superclass,
      map<string, shared_ptr<LoxFunction>> methods);

  shared_ptr<LoxFunction> findMethod(const string& name);
  string toString() override;
  any call(Interpreter& interpreter,
                vector<any> arguments) override;
  int arity() override;
};

