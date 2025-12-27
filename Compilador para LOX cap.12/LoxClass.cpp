#include "LoxClass.h"
#include <utility>
using namespace std;

LoxClass::LoxClass(string name,
    shared_ptr<LoxClass> superclass,
    map<string, shared_ptr<LoxFunction>> methods)
  : superclass{superclass}, name{move(name)},
    methods{move(methods)}
{}

shared_ptr<LoxFunction> LoxClass::findMethod(
    const string& name) {
  auto elem = methods.find(name);
  if (elem != methods.end()) {
      return elem->second;
  }

  if (superclass != nullptr) {
    return superclass->findMethod(name);
  }

  return nullptr;
}

string LoxClass::toString() {
  return name;
}

any LoxClass::call(Interpreter& interpreter,
                        vector<any> arguments) {
  auto instance = make_shared<LoxInstance>(shared_from_this());
  shared_ptr<LoxFunction> initializer = findMethod("init");
  if (initializer != nullptr) {
    initializer->bind(instance)->call(interpreter,
                                      move(arguments));
  }

  return instance;
}

int LoxClass::arity() {
  shared_ptr<LoxFunction> initializer = findMethod("init");
  if (initializer == nullptr) return 0;
  return initializer->arity();
}
