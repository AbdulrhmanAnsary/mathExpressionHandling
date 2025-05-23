#pragma once

#include <iterator>
#include <map>
#include <string>

template <typename T> bool isNum(const T &expression);

class IOperatorsHandling {
public:
  virtual ~IOperatorsHandling() = default;
  virtual bool isOperator(const std::string &expr) const = 0;
  virtual int getOperatorPriority(const std::string &expr) const = 0;
};

class OperatorsHandling : public IOperatorsHandling {
private:
  static std::map<std::string, int> operatorsPriority;

public:
  bool isOperator(const std::string &expr) const override;
  int getOperatorPriority(const std::string &expr) const override;
};

class IExpressionHandling {
public:
  virtual ~IExpressionHandling() = default;
};

class ExpressionParser : public IExpressionHandling {
public:
  OperatorsHandling opHandling;
};

class IExpressionConverter : public ExpressionParser {
public:
  virtual std::string infixToPrefix(const std::string &expr) const = 0;
  virtual std::string postfixToPrefix(const std::string &expr) const = 0;
  virtual std::string infixToPostfix(const std::string &expr) const = 0;
  virtual std::string prefixToPostfix(const std::string &expr) const = 0;
  virtual std::string prefixToInfix(const std::string &expr) const = 0;
  virtual std::string postfixToInfix(const std::string &expr) const = 0;
};

class ExpressionConverter : public IExpressionConverter {
public:
  std::string infixToPrefix(const std::string &expr) const override;
  std::string postfixToPrefix(const std::string &expr) const override;
  std::string infixToPostfix(const std::string &expr) const override;
  std::string prefixToPostfix(const std::string &expr) const override;
  std::string prefixToInfix(const std::string &expr) const override;
  std::string postfixToInfix(const std::string &expr) const override;
};

class IExpressionEvaluator : public ExpressionParser {
public:
  virtual double calcPrefix(const std::string &expr) const = 0;
  virtual double calcPostfix(const std::string &expr) const = 0;
  virtual double calcInfix(const std::string &expr) const = 0;
};

class ExpressionEvaluator : public IExpressionEvaluator {
public:
  double calcPrefix(const std::string &expr) const override;
  double calcPostfix(const std::string &expr) const override;
  double calcInfix(const std::string &expr) const override;
};
