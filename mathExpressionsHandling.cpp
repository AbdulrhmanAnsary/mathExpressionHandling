#include "mathExpressionsHandling.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

static std::vector<std::string> tokenize(const std::string &expr) {
  std::vector<std::string> tokens;
  for (size_t i = 0; i < expr.size();) {
    if (std::isspace((unsigned char)expr[i])) {
      ++i;
      continue;
    }
    // Check for numbers (integer or floating point)
    // A number can start with a digit.
    // A number can start with a '.' IF:
    //   1. It's followed by a digit.
    //   2. It's at the beginning of the expression OR the preceding character is not a digit and not a '.'.
    bool can_start_with_dot = (expr[i] == '.' && 
                               i + 1 < expr.size() && 
                               std::isdigit((unsigned char)expr[i + 1]) &&
                               (i == 0 || (!std::isdigit((unsigned char)expr[i-1]) && expr[i-1] != '.')));

    if (std::isdigit((unsigned char)expr[i]) || can_start_with_dot) {
      size_t j = i;
      bool hasDecimal = false;
      
      while (j < expr.size()) {
        if (std::isdigit((unsigned char)expr[j])) {
          j++;
        } else if (expr[j] == '.' && !hasDecimal) {
          hasDecimal = true;
          j++;
        } else {
          // Not a digit, or a second decimal point, or not a '.'
          break;
        }
      }
      tokens.emplace_back(expr.substr(i, j - i));
      i = j;
    } else {
      if (expr[i] == '*' && i + 1 < expr.size() && expr[i + 1] == '*') {
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

static std::string join(const std::vector<std::string> &tokens) {
  std::string out;
  for (size_t i = 0; i < tokens.size(); ++i) {
    out += tokens[i];
    if (i + 1 < tokens.size())
      out += ' ';
  }
  return out;
}

template <typename T> bool isNum(const T &expression) {
  if (expression.empty()) {
    return false;
  }
  if (expression.length() == 1 && expression[0] == '.') { // Rule out "."
    return false;
  }

  bool hasDecimal = false;
  bool hasDigit = false;
  for (char c : expression) {
    if (std::isdigit(static_cast<unsigned char>(c))) {
      hasDigit = true;
    } else if (c == '.') {
      if (hasDecimal) {
        return false; // Second decimal point
      }
      hasDecimal = true;
    } else {
      return false; // Invalid character
    }
  }
  return hasDigit; // Must have at least one digit
}
template bool isNum<std::string>(const std::string &);

bool OperatorsHandling::isOperator(const std::string &expr) const {
  return operatorsPriority.find(expr) != operatorsPriority.end();
}

int OperatorsHandling::getOperatorPriority(const std::string &expr) const {
  auto it = operatorsPriority.find(expr);
  if (it == operatorsPriority.end())
    return -1;
  return it->second;
}

std::map<std::string, int> OperatorsHandling::operatorsPriority = {
    {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {"^", 3}, {"**", 3}};

std::string ExpressionConverter::infixToPostfix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::vector<std::string> output;
  std::stack<std::string> ops;
  for (const auto &tok : tokens) { // Use const auto&
    if (isNum(tok)) {
      output.push_back(tok);
    } else if (opHandling.isOperator(tok)) {
      // "^" and "**" are right-associative. Others are mostly left-associative.
      // Standard shunting-yard:
      // while (op_on_stack is operator AND
      //        (op_on_stack has greater precedence than tok OR
      //         (op_on_stack has equal precedence as tok AND tok is left-associative (not right))))
      // For simplicity, we'll use getOperatorPriority for comparisons.
      // ^ and ** are right associative, all others left.
      bool currentIsRightAssociative = (tok == "^" || tok == "**");
      while (!ops.empty() && ops.top() != "(" && opHandling.isOperator(ops.top())) {
        int prec_current = opHandling.getOperatorPriority(tok);
        int prec_stack = opHandling.getOperatorPriority(ops.top());
        bool stackIsRightAssociative = (ops.top() == "^" || ops.top() == "**"); // Not strictly needed here but good for clarity

        if ((!currentIsRightAssociative && prec_stack >= prec_current) || (currentIsRightAssociative && prec_stack > prec_current)) {
          output.push_back(ops.top());
          ops.pop();
        } else {
          break; // Lower precedence or right-associative handling
        }
      }
      ops.push(tok);
    } else if (tok == "(") {
      ops.push(tok);
    } else if (tok == ")") {
      while (!ops.empty() && ops.top() != "(") {
        output.push_back(ops.top());
        ops.pop();
      }
      if (ops.empty()) {
        throw std::runtime_error("Invalid infix expression: Mismatched parentheses - no matching '('.");
      }
      ops.pop(); // Pop the "("
    } else {
      throw std::runtime_error("Invalid infix expression: Unknown token '" + tok + "'.");
    }
  }

  while (!ops.empty()) {
    if (ops.top() == "(") {
      throw std::runtime_error("Invalid infix expression: Mismatched parentheses - unclosed '('.");
    }
    output.push_back(ops.top());
    ops.pop();
  }
  return join(output);
}

std::string ExpressionConverter::infixToPrefix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::reverse(tokens.begin(), tokens.end());

  // Swap parentheses
  for (auto &tok : tokens) {
    if (tok == "(") {
      tok = ")";
    } else if (tok == ")") {
      tok = "(";
    }
  }

  std::vector<std::string> output;
  std::stack<std::string> ops;

  // This is essentially infix-to-postfix shunting yard on the reversed/parenthesis-swapped string
  for (const auto &tok : tokens) {
    if (isNum(tok)) {
      output.push_back(tok);
    } else if (opHandling.isOperator(tok)) {
      // For prefix (reversed infix processed as postfix), operator associativity rules are flipped for comparison.
      // Left-associative in infix (like *, /) become right-associative when reversed.
      // Right-associative in infix (like ^, **) become left-associative when reversed.
      // Standard postfix rule: pop if stack_op_prec >= current_op_prec (for left-assoc)
      // or stack_op_prec > current_op_prec (for right-assoc)

      // Let's consider the original associativity of the operator `tok`
      bool tok_is_originally_right_associative = (tok == "^" || tok == "**");

      while (!ops.empty() && ops.top() != "(" && opHandling.isOperator(ops.top())) {
        int prec_tok = opHandling.getOperatorPriority(tok);
        int prec_stack = opHandling.getOperatorPriority(ops.top());

        // If tok was originally right-associative (e.g. ^), when reversed, it behaves like a left-associative rule for popping.
        // Pop if stack operator has greater or equal precedence.
        // If tok was originally left-associative (e.g. *), when reversed, it behaves like a right-associative rule for popping.
        // Pop if stack operator has strictly greater precedence.
        if ((tok_is_originally_right_associative && prec_stack >= prec_tok) ||
            (!tok_is_originally_right_associative && prec_stack > prec_tok)) {
          output.push_back(ops.top());
          ops.pop();
        } else {
          break;
        }
      }
      ops.push(tok);
    } else if (tok == "(") { // This is a ')' from original expression
      ops.push(tok);
    } else if (tok == ")") { // This is a '(' from original expression
      while (!ops.empty() && ops.top() != "(") {
        output.push_back(ops.top());
        ops.pop();
      }
      if (ops.empty()) {
        throw std::runtime_error("Invalid infix expression (for prefix conversion): Mismatched parentheses - no matching '('. Original ')' was missing.");
      }
      ops.pop(); // Pop the corresponding "("
    } else {
      throw std::runtime_error("Invalid infix expression (for prefix conversion): Unknown token '" + tok + "'.");
    }
  }

  while (!ops.empty()) {
    if (ops.top() == "(") { // This means an original ')' was left unclosed
      throw std::runtime_error("Invalid infix expression (for prefix conversion): Mismatched parentheses - unclosed '('. Original ')' was unclosed.");
    }
    output.push_back(ops.top());
    ops.pop();
  }

  std::reverse(output.begin(), output.end()); // Reverse the postfix to get prefix
  return join(output);
}

std::string
ExpressionConverter::postfixToPrefix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::stack<std::string> st; // Changed to stack<string>
  for (const auto &tok : tokens) { // Iterate left to right
    if (isNum(tok)) {
      st.push(tok);
    } else if (opHandling.isOperator(tok)) {
      if (st.size() < 2) {
        throw std::runtime_error("Invalid postfix expression: insufficient operands for operator " + tok);
      }
      std::string operand2 = st.top(); // op2 is top for postfix
      st.pop();
      std::string operand1 = st.top(); // op1 is second for postfix
      st.pop();
      // For postfix to prefix: operator operand1 operand2
      st.push(tok + " " + operand1 + " " + operand2);
    } else {
      throw std::runtime_error("Invalid token in postfix expression: " + tok);
    }
  }
  if (st.size() != 1) {
    throw std::runtime_error("Invalid postfix expression: stack should have one item at the end.");
  }
  return st.top();
}

std::string
ExpressionConverter::prefixToPostfix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::stack<std::string> st; // Changed to stack<string>
  for (int i = static_cast<int>(tokens.size()) - 1; i >= 0; --i) { // Iterate right to left
    const auto &tok = tokens[i];
    if (isNum(tok)) {
      st.push(tok);
    } else if (opHandling.isOperator(tok)) {
      if (st.size() < 2) {
        throw std::runtime_error("Invalid prefix expression: insufficient operands for operator " + tok);
      }
      // For prefix to postfix, when reading tokens from right to left:
      // Top of stack is operand1 (left operand in infix/postfix)
      // Second top is operand2 (right operand in infix/postfix)
      std::string operand1 = st.top();
      st.pop();
      std::string operand2 = st.top();
      st.pop();
      // For prefix to postfix: operand1 operand2 operator
      st.push(operand1 + " " + operand2 + " " + tok);
    } else {
      throw std::runtime_error("Invalid token in prefix expression: " + tok);
    }
  }
  if (st.size() != 1) {
    throw std::runtime_error("Invalid prefix expression: stack should have one item at the end.");
  }
  return st.top();
}

std::string ExpressionConverter::postfixToInfix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::stack<std::string> st; // Stack now holds strings directly
  for (const auto &tok : tokens) {
    if (isNum(tok)) {
      st.push(tok);
    } else if (opHandling.isOperator(tok)) { // Use opHandling to check for operator
      if (st.size() < 2) {
        throw std::runtime_error("Invalid postfix expression: insufficient operands for operator " + tok);
      }
      std::string operand2 = st.top();
      st.pop();
      std::string operand1 = st.top();
      st.pop();
      // Form the infix sub-expression: "( operand1 op operand2 )" with spaces
      st.push("( " + operand1 + " " + tok + " " + operand2 + " )");
    } else {
      // This case should ideally not be reached if the postfix expression is valid
      // and only contains numbers and recognized operators.
      throw std::runtime_error("Invalid token in postfix expression: " + tok);
    }
  }
  if (st.size() != 1) {
    throw std::runtime_error("Invalid postfix expression: The final stack should contain exactly one item.");
  }
  return st.top();
}

std::string ExpressionConverter::prefixToInfix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::stack<std::string> st; // Stack now holds strings directly
  // Iterate from right to left for prefix to infix conversion
  for (int i = static_cast<int>(tokens.size()) - 1; i >= 0; --i) {
    const auto &tok = tokens[i];
    if (isNum(tok)) {
      st.push(tok);
    } else if (opHandling.isOperator(tok)) { // Use opHandling to check for operator
      if (st.size() < 2) {
        throw std::runtime_error("Invalid prefix expression: insufficient operands for operator " + tok);
      }
      // When processing prefix from right to left:
      // The first operand popped (top of stack) is the one that was to the left of the operator.
      // The second operand popped is the one that was to the right of the operator.
      std::string operand1 = st.top();
      st.pop();
      std::string operand2 = st.top();
      st.pop();
      // Form the infix sub-expression: "( operand1 op operand2 )" with spaces
      st.push("( " + operand1 + " " + tok + " " + operand2 + " )");
    } else {
      // This case should ideally not be reached if the prefix expression is valid.
      throw std::runtime_error("Invalid token in prefix expression: " + tok);
    }
  }
  if (st.size() != 1) {
    throw std::runtime_error("Invalid prefix expression: The final stack should contain exactly one item.");
  }
  return st.top();
}

double ExpressionEvaluator::calcPostfix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::stack<double> st; // Changed stack type to double
  for (const auto &tok : tokens) { // Use const auto&
    if (isNum(tok)) {
      try {
        st.push(std::stod(tok)); // Convert to double
      } catch (const std::out_of_range& oor) {
        throw std::runtime_error("Number out of range for double: " + tok);
      }
    } else if (opHandling.isOperator(tok)) { // Use opHandling to check operator
      if (st.size() < 2) {
        throw std::runtime_error("Invalid postfix expression: insufficient operands for operator " + tok);
      }
      double b = st.top(); // Operands are double
      st.pop();
      double a = st.top();
      st.pop();
      if (tok == "+") {
        st.push(a + b);
      } else if (tok == "-") {
        st.push(a - b);
      } else if (tok == "*") {
        st.push(a * b);
      } else if (tok == "/") {
        if (b == 0.0) {
          throw std::runtime_error("Division by zero");
        }
        st.push(a / b); // Floating point division
      } else if (tok == "^" || tok == "**") {
        st.push(std::pow(a, b)); // Operates on and returns double
      } else {
        // This part should ideally not be reached if opHandling.isOperator is comprehensive
        throw std::runtime_error("Unknown operator in postfix expression: " + tok);
      }
    } else {
      throw std::runtime_error("Invalid token in postfix expression: " + tok);
    }
  }
  if (st.size() != 1) {
    throw std::runtime_error("Invalid postfix expression: The final stack should contain exactly one item.");
  }
  return st.top();
}

double ExpressionEvaluator::calcPrefix(const std::string &expr) const {
  auto tokens = tokenize(expr);
  std::stack<double> st; // Changed stack type to double
  for (int i = static_cast<int>(tokens.size()) - 1; i >= 0; --i) {
    const auto &tok = tokens[i];
    if (isNum(tok)) {
      try {
        st.push(std::stod(tok)); // Convert to double
      } catch (const std::out_of_range& oor) {
        throw std::runtime_error("Number out of range for double: " + tok);
      }
    } else if (opHandling.isOperator(tok)) { // Use opHandling to check operator
      if (st.size() < 2) {
        throw std::runtime_error("Invalid prefix expression: insufficient operands for operator " + tok);
      }
      // Note: For prefix evaluation (right-to-left token processing),
      // 'a' is the first operand popped, 'b' is the second.
      double a_op = st.top(); // Operands are double
      st.pop();
      double b_op = st.top();
      st.pop();
      if (tok == "+") {
        st.push(a_op + b_op);
      } else if (tok == "-") {
        st.push(a_op - b_op); // a_op is the left operand in infix: a - b
      } else if (tok == "*") {
        st.push(a_op * b_op);
      } else if (tok == "/") {
        if (b_op == 0.0) {
          throw std::runtime_error("Division by zero");
        }
        st.push(a_op / b_op); // a_op is the dividend: a / b
      } else if (tok == "^" || tok == "**") {
        st.push(std::pow(a_op, b_op)); // a_op is the base: a ^ b
      } else {
        throw std::runtime_error("Unknown operator in prefix expression: " + tok);
      }
    } else {
      throw std::runtime_error("Invalid token in prefix expression: " + tok);
    }
  }
  if (st.size() != 1) {
    throw std::runtime_error("Invalid prefix expression: The final stack should contain exactly one item.");
  }
  return st.top();
}

double ExpressionEvaluator::calcInfix(const std::string &expr) const {
  ExpressionConverter conv;
  std::string postfix = conv.infixToPostfix(expr);
  return calcPostfix(postfix);
}
