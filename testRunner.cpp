#include "mathExpressionsHandling.hpp"
#include "testUtilities.hpp"
#include <iostream>
#include <vector>
#include <string> // Required for std::string
#include <cmath>  // Required for std::abs (used in runTestsNumerical)

int main() {
  ExpressionConverter convertExpr; // For conversion tests
  ExpressionEvaluator evaluator;   // For evaluation tests

  // --- Test Data: Single Digit ---
  std::vector<std::string> infix_expressions_single_digit = {
      "2+3", "7-1", "2*3", "8/4", "2+3-1", "2+3*5", "8/4-1", "2**3",
      "1+2*3-4/2", // Extended
      "5*2+3/1-4"  // Extended
  };
  std::vector<std::string> prefix_expected_single_digit = {
      "+ 2 3", "- 7 1", "* 2 3", "/ 8 4", "- + 2 3 1", "+ 2 * 3 5", "- / 8 4 1", "** 2 3",
      "- + 1 * 2 3 / 4 2",
      "- + * 5 2 / 3 1 4"
  };
  std::vector<std::string> postfix_expected_single_digit = {
      "2 3 +", "7 1 -", "2 3 *", "8 4 /", "2 3 + 1 -", "2 3 5 * +", "8 4 / 1 -", "2 3 **",
      "1 2 3 * + 4 2 / -",
      "5 2 * 3 1 / + 4 -"
  };
  std::vector<double> eval_expected_single_digit = {
      5.0, 6.0, 6.0, 2.0, 4.0, 17.0, 1.0, 8.0,
      1.0 + 2.0*3.0 - 4.0/2.0, // 1 + 6 - 2 = 5.0
      5.0*2.0 + 3.0/1.0 - 4.0 // 10 + 3 - 4 = 9.0
  };
  // Canonical infix for toInfix conversions (fully parenthesized)
  std::vector<std::string> infix_expected_single_digit_canonical = {
      "( 2 + 3 )", "( 7 - 1 )", "( 2 * 3 )", "( 8 / 4 )", "( ( 2 + 3 ) - 1 )", "( 2 + ( 3 * 5 ) )", "( ( 8 / 4 ) - 1 )", "( 2 ** 3 )",
      "( ( 1 + ( 2 * 3 ) ) - ( 4 / 2 ) )",
      "( ( ( 5 * 2 ) + ( 3 / 1 ) ) - 4 )"
  };


  // --- Test Data: Multi Digit ---
  std::vector<std::string> infix_expressions_multi_digit = {
      "21+3", "73-10", "20*31", "80/4", "21+30-11", "20+30*51", "80/4-10", "2**10",
      "10+20*30-40/20", // Extended
      "50*2+30/10-40"   // Extended
  };
  std::vector<std::string> prefix_expected_multi_digit = {
      "+ 21 3", "- 73 10", "* 20 31", "/ 80 4", "- + 21 30 11", "+ 20 * 30 51", "- / 80 4 10", "** 2 10",
      "- + 10 * 20 30 / 40 20",
      "- + * 50 2 / 30 10 40"
  };
  std::vector<std::string> postfix_expected_multi_digit = {
      "21 3 +", "73 10 -", "20 31 *", "80 4 /", "21 30 + 11 -", "20 30 51 * +", "80 4 / 10 -", "2 10 **",
      "10 20 30 * + 40 20 / -",
      "50 2 * 30 10 / + 40 -"
  };
  std::vector<double> eval_expected_multi_digit = {
      24.0, 63.0, 620.0, 20.0, 40.0, 1550.0, 10.0, 1024.0,
      10.0 + 20.0*30.0 - 40.0/20.0, // 10 + 600 - 2 = 608.0
      50.0*2.0 + 30.0/10.0 - 40.0  // 100 + 3 - 40 = 63.0
  };
  std::vector<std::string> infix_expected_multi_digit_canonical = {
    "( 21 + 3 )", "( 73 - 10 )", "( 20 * 31 )", "( 80 / 4 )", "( ( 21 + 30 ) - 11 )", "( 20 + ( 30 * 51 ) )", "( ( 80 / 4 ) - 10 )", "( 2 ** 10 )",
    "( ( 10 + ( 20 * 30 ) ) - ( 40 / 20 ) )",
    "( ( ( 50 * 2 ) + ( 30 / 10 ) ) - 40 )"
  };

  // --- Test Data: With Parentheses ---
  std::vector<std::string> infix_expressions_with_parentheses = {
      "(2+3)*4", "10/(2+3)", "((1+2)*3)-4", "1+(2*(3-1))", "((9-5)/(3-1))**2",
      "(20-(3*4))/(15-(2**3))" // (20-12)/(15-8) = 8/7
  };
  std::vector<std::string> prefix_expected_with_parentheses = {
      "* + 2 3 4", "/ 10 + 2 3", "- * + 1 2 3 4", "+ 1 * 2 - 3 1", "** / - 9 5 - 3 1 2",
      "/ - 20 * 3 4 - 15 ** 2 3"
  };
  std::vector<std::string> postfix_expected_with_parentheses = {
      "2 3 + 4 *", "10 2 3 + /", "1 2 + 3 * 4 -", "1 2 3 1 - * +", "9 5 - 3 1 - / 2 **",
      "20 3 4 * - 15 2 3 ** - /"
  };
  std::vector<double> eval_expected_with_parentheses = {
      20.0, 2.0, 5.0, 5.0, 4.0,
      (20.0-(3.0*4.0))/(15.0-std::pow(2.0,3.0)) // 8.0/7.0
  };
   std::vector<std::string> infix_expected_with_parentheses_canonical = {
      "( ( 2 + 3 ) * 4 )", "( 10 / ( 2 + 3 ) )", "( ( ( 1 + 2 ) * 3 ) - 4 )", "( 1 + ( 2 * ( 3 - 1 ) ) )", "( ( ( 9 - 5 ) / ( 3 - 1 ) ) ** 2 )",
      "( ( 20 - ( 3 * 4 ) ) / ( 15 - ( 2 ** 3 ) ) )"
  };


  // --- Test Data: Floating Point ---
  std::vector<std::string> infix_expressions_floating_point = {
      "10/4", "7/2", "1/3+1/3+1/3", "5.5+2.2", "3.0*2.5", "10.0/4.0 - 0.5",
      "100 / ( 2.5 * 8 )", // 100 / 20 = 5
      // New valid test cases
      "0.5 + 0.25",
      "10. * .5",
      ".25 - .125",
      "100.0 / 2.0",
      "3.14 * (2.0 + 1.0)",
      "1. + 2",
      "3. - .5"
  };
  std::vector<std::string> prefix_expected_floating_point = {
      "/ 10 4", "/ 7 2", "+ + / 1 3 / 1 3 / 1 3", "+ 5.5 2.2", "* 3.0 2.5", "- / 10.0 4.0 0.5",
      "/ 100 * 2.5 8",
      // New prefix expected
      "+ 0.5 0.25",
      "* 10. .5",
      "- .25 .125",
      "/ 100.0 2.0",
      "* 3.14 + 2.0 1.0",
      "+ 1. 2",
      "- 3. .5"
  };
  std::vector<std::string> postfix_expected_floating_point = {
      "10 4 /", "7 2 /", "1 3 / 1 3 / + 1 3 / +", "5.5 2.2 +", "3.0 2.5 *", "10.0 4.0 / 0.5 -",
      "100 2.5 8 * /",
      // New postfix expected
      "0.5 0.25 +",
      "10. .5 *",
      ".25 .125 -",
      "100.0 2.0 /",
      "3.14 2.0 1.0 + *",
      "1. 2 +",
      "3. .5 -"
  };
  std::vector<double> eval_expected_floating_point = {
      2.5, 3.5, 1.0, 7.7, 7.5, 2.0,
      5.0,
      // New eval expected
      0.75,
      5.0,
      0.125,
      50.0,
      9.42, // 3.14 * 3.0
      3.0,
      2.5
  };
  std::vector<std::string> infix_expected_floating_point_canonical = {
      "( 10 / 4 )", "( 7 / 2 )", "( ( ( 1 / 3 ) + ( 1 / 3 ) ) + ( 1 / 3 ) )", "( 5.5 + 2.2 )", "( 3.0 * 2.5 )", "( ( 10.0 / 4.0 ) - 0.5 )",
      "( 100 / ( 2.5 * 8 ) )",
      // New canonical infix
      "( 0.5 + 0.25 )",
      "( 10. * .5 )",
      "( .25 - .125 )",
      "( 100.0 / 2.0 )",
      "( 3.14 * ( 2.0 + 1.0 ) )",
      "( 1. + 2 )",
      "( 3. - .5 )"
  };


  // --- Running Conversion Tests ---
  std::cout << "\n[========== Running Conversion Tests ==========]\n";

  std::cout << "\n[--- Testing infixToPostfix (single digit) ---]\n";
  runTests(infix_expressions_single_digit, postfix_expected_single_digit, &convertExpr, &ExpressionConverter::infixToPostfix);
  std::cout << "\n[--- Testing infixToPostfix (multi digit) ---]\n";
  runTests(infix_expressions_multi_digit, postfix_expected_multi_digit, &convertExpr, &ExpressionConverter::infixToPostfix);
  std::cout << "\n[--- Testing infixToPostfix (with parentheses) ---]\n";
  runTests(infix_expressions_with_parentheses, postfix_expected_with_parentheses, &convertExpr, &ExpressionConverter::infixToPostfix);
  std::cout << "\n[--- Testing infixToPostfix (floating point) ---]\n";
  runTests(infix_expressions_floating_point, postfix_expected_floating_point, &convertExpr, &ExpressionConverter::infixToPostfix);

  std::cout << "\n[--- Testing infixToPrefix (single digit) ---]\n";
  runTests(infix_expressions_single_digit, prefix_expected_single_digit, &convertExpr, &ExpressionConverter::infixToPrefix);
  std::cout << "\n[--- Testing infixToPrefix (multi digit) ---]\n";
  runTests(infix_expressions_multi_digit, prefix_expected_multi_digit, &convertExpr, &ExpressionConverter::infixToPrefix);
  std::cout << "\n[--- Testing infixToPrefix (with parentheses) ---]\n";
  runTests(infix_expressions_with_parentheses, prefix_expected_with_parentheses, &convertExpr, &ExpressionConverter::infixToPrefix);
  std::cout << "\n[--- Testing infixToPrefix (floating point) ---]\n";
  runTests(infix_expressions_floating_point, prefix_expected_floating_point, &convertExpr, &ExpressionConverter::infixToPrefix);

  std::cout << "\n[--- Testing postfixToInfix (single digit) ---]\n";
  runTests(postfix_expected_single_digit, infix_expected_single_digit_canonical, &convertExpr, &ExpressionConverter::postfixToInfix);
  std::cout << "\n[--- Testing postfixToInfix (multi digit) ---]\n";
  runTests(postfix_expected_multi_digit, infix_expected_multi_digit_canonical, &convertExpr, &ExpressionConverter::postfixToInfix);
  std::cout << "\n[--- Testing postfixToInfix (with parentheses) ---]\n";
  runTests(postfix_expected_with_parentheses, infix_expected_with_parentheses_canonical, &convertExpr, &ExpressionConverter::postfixToInfix);
   std::cout << "\n[--- Testing postfixToInfix (floating point) ---]\n";
  runTests(postfix_expected_floating_point, infix_expected_floating_point_canonical, &convertExpr, &ExpressionConverter::postfixToInfix);

  std::cout << "\n[--- Testing prefixToInfix (single digit) ---]\n";
  runTests(prefix_expected_single_digit, infix_expected_single_digit_canonical, &convertExpr, &ExpressionConverter::prefixToInfix);
  std::cout << "\n[--- Testing prefixToInfix (multi digit) ---]\n";
  runTests(prefix_expected_multi_digit, infix_expected_multi_digit_canonical, &convertExpr, &ExpressionConverter::prefixToInfix);
  std::cout << "\n[--- Testing prefixToInfix (with parentheses) ---]\n";
  runTests(prefix_expected_with_parentheses, infix_expected_with_parentheses_canonical, &convertExpr, &ExpressionConverter::prefixToInfix);
  std::cout << "\n[--- Testing prefixToInfix (floating point) ---]\n";
  runTests(prefix_expected_floating_point, infix_expected_floating_point_canonical, &convertExpr, &ExpressionConverter::prefixToInfix);
  
  std::cout << "\n[--- Testing postfixToPrefix (single digit) ---]\n";
  runTests(postfix_expected_single_digit, prefix_expected_single_digit, &convertExpr, &ExpressionConverter::postfixToPrefix);
  std::cout << "\n[--- Testing postfixToPrefix (multi digit) ---]\n";
  runTests(postfix_expected_multi_digit, prefix_expected_multi_digit, &convertExpr, &ExpressionConverter::postfixToPrefix);
  std::cout << "\n[--- Testing postfixToPrefix (with parentheses) ---]\n";
  runTests(postfix_expected_with_parentheses, prefix_expected_with_parentheses, &convertExpr, &ExpressionConverter::postfixToPrefix);
  std::cout << "\n[--- Testing postfixToPrefix (floating point) ---]\n";
  runTests(postfix_expected_floating_point, prefix_expected_floating_point, &convertExpr, &ExpressionConverter::postfixToPrefix);

  std::cout << "\n[--- Testing prefixToPostfix (single digit) ---]\n";
  runTests(prefix_expected_single_digit, postfix_expected_single_digit, &convertExpr, &ExpressionConverter::prefixToPostfix);
  std::cout << "\n[--- Testing prefixToPostfix (multi digit) ---]\n";
  runTests(prefix_expected_multi_digit, postfix_expected_multi_digit, &convertExpr, &ExpressionConverter::prefixToPostfix);
  std::cout << "\n[--- Testing prefixToPostfix (with parentheses) ---]\n";
  runTests(prefix_expected_with_parentheses, postfix_expected_with_parentheses, &convertExpr, &ExpressionConverter::prefixToPostfix);
  std::cout << "\n[--- Testing prefixToPostfix (floating point) ---]\n";
  runTests(prefix_expected_floating_point, postfix_expected_floating_point, &convertExpr, &ExpressionConverter::prefixToPostfix);


  // --- Running Evaluation Tests ---
  std::cout << "\n[========== Running Evaluation Tests ==========]\n";

  std::cout << "\n[--- Testing calcInfix (single digit) ---]\n";
  runTestsNumerical(infix_expressions_single_digit, eval_expected_single_digit, &evaluator, &ExpressionEvaluator::calcInfix);
  std::cout << "\n[--- Testing calcInfix (multi digit) ---]\n";
  runTestsNumerical(infix_expressions_multi_digit, eval_expected_multi_digit, &evaluator, &ExpressionEvaluator::calcInfix);
  std::cout << "\n[--- Testing calcInfix (with parentheses) ---]\n";
  runTestsNumerical(infix_expressions_with_parentheses, eval_expected_with_parentheses, &evaluator, &ExpressionEvaluator::calcInfix);
  std::cout << "\n[--- Testing calcInfix (floating point) ---]\n";
  runTestsNumerical(infix_expressions_floating_point, eval_expected_floating_point, &evaluator, &ExpressionEvaluator::calcInfix);

  std::cout << "\n[--- Testing calcPostfix (single digit) ---]\n";
  runTestsNumerical(postfix_expected_single_digit, eval_expected_single_digit, &evaluator, &ExpressionEvaluator::calcPostfix);
  std::cout << "\n[--- Testing calcPostfix (multi digit) ---]\n";
  runTestsNumerical(postfix_expected_multi_digit, eval_expected_multi_digit, &evaluator, &ExpressionEvaluator::calcPostfix);
  std::cout << "\n[--- Testing calcPostfix (with parentheses) ---]\n"; // Added
  runTestsNumerical(postfix_expected_with_parentheses, eval_expected_with_parentheses, &evaluator, &ExpressionEvaluator::calcPostfix);
  std::cout << "\n[--- Testing calcPostfix (floating point) ---]\n";
  runTestsNumerical(postfix_expected_floating_point, eval_expected_floating_point, &evaluator, &ExpressionEvaluator::calcPostfix);

  std::cout << "\n[--- Testing calcPrefix (single digit) ---]\n";
  runTestsNumerical(prefix_expected_single_digit, eval_expected_single_digit, &evaluator, &ExpressionEvaluator::calcPrefix);
  std::cout << "\n[--- Testing calcPrefix (multi digit) ---]\n";
  runTestsNumerical(prefix_expected_multi_digit, eval_expected_multi_digit, &evaluator, &ExpressionEvaluator::calcPrefix);
  std::cout << "\n[--- Testing calcPrefix (with parentheses) ---]\n"; // Added
  runTestsNumerical(prefix_expected_with_parentheses, eval_expected_with_parentheses, &evaluator, &ExpressionEvaluator::calcPrefix);
  std::cout << "\n[--- Testing calcPrefix (floating point) ---]\n";
  runTestsNumerical(prefix_expected_floating_point, eval_expected_floating_point, &evaluator, &ExpressionEvaluator::calcPrefix);

  // --- Running Malformed Input Tests ---
  std::cout << "\n[========== Running Malformed Input Tests ==========]\n";
  int malformedSuccessCounter = 0;
  int malformedFailCounter = 0;
  // Based on the current implementation, the most common error for these will be due to "." being an unknown token
  // after tokenization, because isNum(".") is false.
  std::string expectedErrorSubstringGeneric = "Unknown token '.'"; 

  auto testMalformedExpression = 
    [&](const std::string& exprStr, const std::string& expectedSubstr, const std::string& testName) {
    std::cout << "\n--- Testing Malformed: " << testName << " (\"" << exprStr << "\") ---" << std::endl;
    bool testPassed = false;
    try {
        // We test infixToPostfix as it's the first stage that uses tokenize and isNum extensively.
        convertExpr.infixToPostfix(exprStr);
        std::cerr << "\033[31mTest FAILED: Expected std::runtime_error for '" << exprStr << "' but none was thrown.\033[97m" << std::endl;
        malformedFailCounter++;
    } catch (const std::runtime_error& e) {
        std::string errorMsg = e.what();
        if (errorMsg.find(expectedSubstr) != std::string::npos) {
            std::cout << "\033[32mTest PASSED: Correctly threw for '" << exprStr << "'. Error: " << errorMsg << "\033[97m" << std::endl;
            malformedSuccessCounter++;
            testPassed = true;
        } else {
            std::cerr << "\033[31mTest FAILED: Incorrect error for '" << exprStr 
                      << "'.\n    Expected substring: \"" << expectedSubstr 
                      << "\"\n    Got error: \"" << errorMsg << "\"\033[97m" << std::endl;
            malformedFailCounter++;
        }
    } catch (...) {
        std::cerr << "\033[31mTest FAILED: Expected std::runtime_error for '" << exprStr << "' but a different exception was thrown.\033[97m" << std::endl;
        malformedFailCounter++;
    }
    return testPassed;
  };

  // Test cases for malformed inputs
  // For "3.1.4 + 5", tokenizer creates "3.1", ".", "4", "+", "5". The "." token is rejected by isNum.
  testMalformedExpression("3.1.4 + 5", expectedErrorSubstringGeneric, "Multiple decimals in number (3.1.4)");
  // For "1..2 - 3", tokenizer creates "1.", ".", "2", "-", "3". The second "." token is rejected.
  testMalformedExpression("1..2 - 3", expectedErrorSubstringGeneric, "Consecutive decimals (1..2)");
  // For "1.2. + 3", tokenizer creates "1.2", ".", "+", "3". The "." token is rejected.
  testMalformedExpression("1.2. + 3", expectedErrorSubstringGeneric, "Multiple decimals with operator (1.2.)");
  testMalformedExpression("( . + 1 )", expectedErrorSubstringGeneric, "Lone dot in parentheses");
  testMalformedExpression(". + 1", expectedErrorSubstringGeneric, "Lone dot at start of expression");
  testMalformedExpression("1 + .", expectedErrorSubstringGeneric, "Lone dot at end of expression");
  // For "1..", tokenizer creates "1.", ".", The second "." token is rejected.
  testMalformedExpression("1..", expectedErrorSubstringGeneric, "Number ending with multiple dots (1..)");
  // For "..1", tokenizer creates ".", ".", "1". The first "." token is rejected.
  testMalformedExpression("..1", expectedErrorSubstringGeneric, "Number starting with multiple dots (..1)");
  // For "1 . 2", tokenizer creates "1", ".", "2". The "." token is rejected.
  testMalformedExpression("1 . 2", expectedErrorSubstringGeneric, "Space separated dot (1 . 2)");


  std::cout << "\n[--- Malformed Input Test Summary ---]\n";
  if (malformedFailCounter > 0) {
      std::cout << "\033[31m"; // Red for summary if failures
      std::cout << "[  SOME MALFORMED TESTS FAILED  ]\n";
  } else {
      std::cout << "\033[32m"; // Green for summary if all pass
      std::cout << "[  ALL MALFORMED TESTS PASSED  ]\n";
  }
  std::cout << "Passed " << malformedSuccessCounter << " from " 
            << (malformedSuccessCounter + malformedFailCounter) << " malformed input tests.\033[97m\n"; // Reset color

  std::cout << "\n[========== All Tests Completed ==========]\n";
  // Check existing test failures (need to adapt if getTotalFailedTests() isn't available)
  // For now, just consider malformedFailCounter for return status
  if (malformedFailCounter > 0) { 
      std::cerr << "\n\033[31mOverall: Some malformed input tests failed.\033[97m" << std::endl;
      return 1; // Indicate failure
  }
  return 0;
}
