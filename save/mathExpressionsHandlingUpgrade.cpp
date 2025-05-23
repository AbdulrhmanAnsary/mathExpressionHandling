#include "mathExpressionsHandling.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <map>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

// Tokenizer
std::vector<std::string> tokenize(const std::string &expr) {
  std::vector<std::string> tokens;
  for (size_t i = 0; i < expr.length();) {
    if (std::isspace(expr[i])) {
      ++i;
      continue;
    }
    if (std::isdigit(expr[i]) ||
        (expr[i] == '-' && (i == 0 || !std::isdigit(expr[i - 1])))) {
      size_t j = i + (expr[i] == '-' ? 1 : 0);
      bool hasDot = false;
      while (j < expr.length() &&
             (std::isdigit(expr[j]) || (expr[j] == '.' && !hasDot))) {
        if (expr[j] == '.')
          hasDot = true;
        ++j;
      }
      tokens.emplace_back(expr.substr(i, j - i));
      i = j;
    } else {
      if (expr[i] == '*' && i + 1 < expr.length() && expr[i + 1] == '*') {
        tokens.emplace_back("**");
        i += 2;
      } else {
        tokens.emplace_back(std::string(1, expr[i]));
        ++i;
      }
    }
  }
  return tokens;
}

std::string join(const std::vector<std::string> &tokens) {
  std::string result;
  for (const auto &token : tokens) {
    if (!result.empty())
      result += ' ';
    result += token;
  }
  return result;
}

inline bool isNumeric(const std::string &token) {
  if (token.empty())
    return false;
  size_t i = (token[0] == '-') ? 1 : 0;
  bool dotFound = false;
  for (; i < token.length(); ++i) {
    if (token[i] == '.') {
      if (dotFound)
        return false;
      dotFound = true;
    } else if (!std::isdigit(token[i])) {
      return false;
    }
  }
  return true;
}

inline bool isRightAssociative(const std::string &op) {
  return (op == "^" || op == "**");
}

std::vector<std::string> binaryOp(const std::vector<std::string> &a,
                                  const std::vector<std::string> &b,
                                  const std::string &op, bool parentheses) {
  std::vector<std::string> result;
  if (parentheses)
    result.emplace_back("(");
  result.insert(result.end(), a.begin(), a.end());
  result.push_back(op);
  result.insert(result.end(), b.begin(), b.end());
  if (parentheses)
    result.emplace_back(")");
  return result;
}

} // namespace

std::map<std::string, int> OperatorsHandling::operatorsPriority = {
    {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {"^", 3}, {"**", 3}};

bool OperatorsHandling::isOperator(const std::string &expr) const {
  return operatorsPriority.count(expr) != 0;
}

int OperatorsHandling::getOperatorPriority(const std::string &expr) const {
  auto it = operatorsPriority.find(expr);
  if (it == operatorsPriority.end())
    throw std::runtime_error("Unknown operator: " + expr);
  return it->second;
}

std::string ExpressionConverter::infixToPostfix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::vector<std::string> output;
  std::stack<std::string> ops;
  OperatorsHandling opHandler;

  for (const auto &tok : tokens) {
    if (isNumeric(tok)) {
      output.push_back(tok);
    } else if (tok == "(") {
      ops.push(tok);
    } else if (tok == ")") {
      while (!ops.empty() && ops.top() != "(") {
        output.push_back(ops.top());
        ops.pop();
      }
      if (ops.empty())
        throw std::runtime_error("Mismatched parentheses");
      ops.pop(); // remove '('
    } else if (opHandler.isOperator(tok)) {
      while (!ops.empty() && opHandler.isOperator(ops.top())) {
        int currPri = opHandler.getOperatorPriority(tok);
        int topPri = opHandler.getOperatorPriority(ops.top());
        if (currPri < topPri ||
            (currPri == topPri && !isRightAssociative(tok))) {
          output.push_back(ops.top());
          ops.pop();
        } else
          break;
      }
      ops.push(tok);
    } else {
      throw std::runtime_error("Invalid token: " + tok);
    }
  }

  while (!ops.empty()) {
    if (ops.top() == "(")
      throw std::runtime_error("Mismatched parentheses");
    output.push_back(ops.top());
    ops.pop();
  }

  return join(output);
}

std::string ExpressionConverter::infixToPrefix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::reverse(tokens.begin(), tokens.end());
  for (auto &tok : tokens) {
    if (tok == "(")
      tok = ")";
    else if (tok == ")")
      tok = "(";
  }
  auto postfix = infixToPostfix(join(tokens));
  auto postTokens = tokenize(postfix);
  std::reverse(postTokens.begin(), postTokens.end());
  return join(postTokens);
}

std::string
ExpressionConverter::postfixToPrefix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::stack<std::vector<std::string>> st;
  for (const auto &tok : tokens) {
    if (isNumeric(tok)) {
      st.push({tok});
    } else {
      if (st.size() < 2)
        throw std::runtime_error("Invalid postfix expression");
      auto b = st.top();
      st.pop();
      auto a = st.top();
      st.pop();
      std::vector<std::string> combined = {tok};
      combined.insert(combined.end(), a.begin(), a.end());
      combined.insert(combined.end(), b.begin(), b.end());
      st.push(combined);
    }
  }
  if (st.size() != 1)
    throw std::runtime_error("Invalid postfix expression");
  return join(st.top());
}

std::string
ExpressionConverter::prefixToPostfix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::stack<std::vector<std::string>> st;
  for (int i = static_cast<int>(tokens.size()) - 1; i >= 0; --i) {
    const auto &tok = tokens[i];
    if (isNumeric(tok)) {
      st.push({tok});
    } else {
      if (st.size() < 2)
        throw std::runtime_error("Invalid prefix expression");
      auto a = st.top();
      st.pop();
      auto b = st.top();
      st.pop();
      std::vector<std::string> combined = a;
      combined.insert(combined.end(), b.begin(), b.end());
      combined.push_back(tok);
      st.push(combined);
    }
  }
  if (st.size() != 1)
    throw std::runtime_error("Invalid prefix expression");
  return join(st.top());
}

std::string ExpressionConverter::postfixToInfix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::stack<std::vector<std::string>> st;
  for (const auto &tok : tokens) {
    if (isNumeric(tok)) {
      st.push({tok});
    } else {
      if (st.size() < 2)
        throw std::runtime_error("Invalid postfix expression");
      auto b = st.top();
      st.pop();
      auto a = st.top();
      st.pop();
      st.push(binaryOp(a, b, tok, true));
    }
  }
  if (st.size() != 1)
    throw std::runtime_error("Invalid postfix expression");
  return join(st.top());
}

std::string ExpressionConverter::prefixToInfix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::stack<std::vector<std::string>> st;
  for (int i = static_cast<int>(tokens.size()) - 1; i >= 0; --i) {
    const auto &tok = tokens[i];
    if (isNumeric(tok)) {
      st.push({tok});
    } else {
      if (st.size() < 2)
        throw std::runtime_error("Invalid prefix expression");
      auto a = st.top();
      st.pop();
      auto b = st.top();
      st.pop();
      st.push(binaryOp(a, b, tok, true));
    }
  }
  if (st.size() != 1)
    throw std::runtime_error("Invalid prefix expression");
  return join(st.top());
}

double ExpressionEvaluator::calcPostfix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::stack<double> st;
  for (const auto &tok : tokens) {
    if (isNumeric(tok)) {
      st.push(std::stod(tok));
    } else {
      if (st.size() < 2)
        throw std::runtime_error("Invalid postfix expression");
      double b = st.top();
      st.pop();
      double a = st.top();
      st.pop();
      if (tok == "+")
        st.push(a + b);
      else if (tok == "-")
        st.push(a - b);
      else if (tok == "*")
        st.push(a * b);
      else if (tok == "/")
        st.push(a / b);
      else if (tok == "^" || tok == "**")
        st.push(std::pow(a, b));
      else
        throw std::runtime_error("Unknown operator: " + tok);
    }
  }
  if (st.size() != 1)
    throw std::runtime_error("Invalid postfix expression");
  return st.top();
}

double ExpressionEvaluator::calcPrefix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::stack<double> st;
  for (int i = static_cast<int>(tokens.size()) - 1; i >= 0; --i) {
    const auto &tok = tokens[i];
    if (isNumeric(tok)) {
      st.push(std::stod(tok));
    } else {
      if (st.size() < 2)
        throw std::runtime_error("Invalid prefix expression");
      double a = st.top();
      st.pop();
      double b = st.top();
      st.pop();
      if (tok == "+")
        st.push(a + b);
      else if (tok == "-")
        st.push(a - b);
      else if (tok == "*")
        st.push(a * b);
      else if (tok == "/")
        st.push(a / b);
      else if (tok == "^" || tok == "**")
        st.push(std::pow(a, b));
      else
        throw std::runtime_error("Unknown operator: " + tok);
    }
  }
  if (st.size() != 1)
    throw std::runtime_error("Invalid prefix expression");
  return st.top();
}

double ExpressionEvaluator::calcInfix(const std::string &expr) const {
  ExpressionConverter converter;
  auto postfix = converter.infixToPostfix(expr);
  return calcPostfix(postfix);
}
