#pragma once

#include <any>
#include <map>
#include <memory>
#include <string>
using namespace std;

class LoxClass;
class Token;

class LoxInstance: public enable_shared_from_this<LoxInstance> {
  shared_ptr<LoxClass> klass;
  map<string, any> fields;

public:
  LoxInstance(shared_ptr<LoxClass> klass);
  any get(const Token& name);
  void set(const Token& name, any value);
  string toString();
};
