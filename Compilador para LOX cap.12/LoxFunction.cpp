#include "LoxFunction.h"
#include <utility>
#include "LoxInstance.h"
#include "Interpreter.h"
#include "Stmt.h"
using namespace std;
LoxFunction::LoxFunction(shared_ptr<Function> declaration,
                         shared_ptr<Environment> closure,
                         bool isInitializer)
  : isInitializer{isInitializer}, closure{move(closure)},
    declaration{move(declaration)}
{}

shared_ptr<LoxFunction> LoxFunction::bind(
    shared_ptr<LoxInstance> instance) {
  auto environment = make_shared<Environment>(closure);
  environment->define("this", instance);
  return make_shared<LoxFunction>(declaration, environment,
                                       isInitializer);
}

string LoxFunction::toString() {
  return "<fn " + declaration->name.lexeme + ">";
}

int LoxFunction::arity() {
  return declaration->params.size();
}

any LoxFunction::call(Interpreter& interpreter,
                           vector<any> arguments) {
  auto environment = make_shared<Environment>(closure);
  for (int i = 0; i < declaration->params.size(); ++i) {
    environment->define(declaration->params[i].lexeme,
        arguments[i]);
  }

  try {
    interpreter.executeBlock(declaration->body, environment);
  } catch (LoxReturn returnValue) {
    if (isInitializer) return closure->getAt(0, "this");

    return returnValue.value;
  }

  if (isInitializer) return closure->getAt(0, "this");

  return nullptr;
}
