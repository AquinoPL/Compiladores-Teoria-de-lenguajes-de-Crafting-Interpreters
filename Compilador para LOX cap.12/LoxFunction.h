#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>
#include "LoxCallable.h"
using namespace std; 

class Environment;
class Function;
class LoxInstance;

class LoxFunction: public LoxCallable {
  shared_ptr<Function> declaration;
  shared_ptr<Environment> closure;

  bool isInitializer;

public:
  LoxFunction(shared_ptr<Function> declaration,
              shared_ptr<Environment> closure,
              bool isInitializer);
  shared_ptr<LoxFunction> bind(
      shared_ptr<LoxInstance> instance);
  string toString() override;
  int arity() override;
  any call(Interpreter& interpreter,
                vector<any> arguments) override;
};
