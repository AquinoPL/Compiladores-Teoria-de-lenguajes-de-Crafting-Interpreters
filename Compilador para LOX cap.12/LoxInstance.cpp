#include "LoxInstance.h"
#include <utility>
#include "Error.h"
using namespace std;

LoxInstance::LoxInstance(shared_ptr<LoxClass> klass)
  : klass{move(klass)}
{}

any LoxInstance::get(const Token& name) {
  auto elem = fields.find(name.lexeme);
  if (elem != fields.end()) {
    return elem->second;
  }

  shared_ptr<LoxFunction> method =
      klass->findMethod(name.lexeme);
  if (method != nullptr) return method->bind(shared_from_this());

  throw RuntimeError(name,
      "Undefined property '" + name.lexeme + "'.");
}

void LoxInstance::set(const Token& name, any value) {
  fields[name.lexeme] = move(value);
}

string LoxInstance::toString() {
  return klass->name + " instance";
}
