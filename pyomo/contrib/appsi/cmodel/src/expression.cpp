#include "expression.hpp"


std::shared_ptr<ExpressionBase> binary_helper(std::shared_ptr<ExpressionBase> n1, std::shared_ptr<ExpressionBase> n2, std::shared_ptr<BinaryOperator> oper)
{
  std::shared_ptr<Expression> expr;
  if (n1->is_leaf() && n2->is_leaf())
    {
      oper->operand1 = n1;
      oper->operand2 = n2;
      expr = std::make_shared<Expression>();
    }
  else if (n1->is_leaf())
    {
      oper->operand1 = n1;
      std::shared_ptr<Expression> n2_expr = std::dynamic_pointer_cast<Expression>(n2);
      expr = n2_expr->copy_expr();
      oper->operand2 = n2_expr->last_operator();
    }
  else if (n2->is_leaf())
    {
      std::shared_ptr<Expression> n1_expr = std::dynamic_pointer_cast<Expression>(n1);
      oper->operand1 = n1_expr->last_operator();
      oper->operand2 = n2;
      expr = n1_expr->copy_expr();
    }
  else
    {
      std::shared_ptr<Expression> n1_expr = std::dynamic_pointer_cast<Expression>(n1);
      std::shared_ptr<Expression> n2_expr = std::dynamic_pointer_cast<Expression>(n2);
      oper->operand1 = n1_expr->last_operator();
      oper->operand2 = n2_expr->last_operator();
      expr = n1_expr->copy_expr();
      expr->extend_operators(n2_expr);
    }
  expr->add_operator(oper);
  return expr;
}


std::shared_ptr<ExpressionBase> external_helper(std::string function_name, std::vector<std::shared_ptr<ExpressionBase> > operands)
{
  std::shared_ptr<ExternalOperator> oper = std::make_shared<ExternalOperator>();
  oper->function_name = function_name;
  std::shared_ptr<Expression> expr = std::make_shared<Expression>();
  for (std::shared_ptr<Node> n : operands)
    {
      if (n->is_leaf())
	{
	  oper->operands->push_back(n);
	}
      else
	{
	  std::shared_ptr<Expression> n_expr = std::dynamic_pointer_cast<Expression>(n);
	  expr->extend_operators(n_expr);
	  oper->operands->push_back(n_expr->last_operator());
	}
    }
  expr->add_operator(oper);
  return expr;
}


std::shared_ptr<ExpressionBase> unary_helper(std::shared_ptr<ExpressionBase> n1, std::shared_ptr<UnaryOperator> oper)
{
  std::shared_ptr<Expression> expr;
  if (n1->is_leaf())
    {
      oper->operand = n1;
      expr = std::make_shared<Expression>();
    }
  else
    {
      std::shared_ptr<Expression> n1_expr = std::dynamic_pointer_cast<Expression>(n1);
      oper->operand = n1_expr->last_operator();
      expr = n1_expr->copy_expr();
    }
  expr->add_operator(oper);
  return expr;
}


std::shared_ptr<ExpressionBase> ExpressionBase::operator+(ExpressionBase& other)
{
  if (other.is_constant_type() && other.evaluate() == 0.0)
    {
      return shared_from_this();
    }
  else if (is_constant_type() && evaluate() == 0.0)
    {
      return other.shared_from_this();
    }
  else if (is_constant_type() && other.is_constant_type())
    {
      return std::make_shared<Constant>(evaluate() + other.evaluate());
    }
  else
    {
      std::shared_ptr<AddOperator> oper = std::make_shared<AddOperator>();
      return binary_helper(shared_from_this(), other.shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> ExpressionBase::operator-(ExpressionBase& other)
{
  if (other.is_constant_type() && other.evaluate() == 0.0)
    {
      return shared_from_this();
    }
  else if (is_constant_type() && evaluate() == 0.0)
    {
      return -other;
    }
  else if (is_constant_type() && other.is_constant_type())
    {
      return std::make_shared<Constant>(evaluate() - other.evaluate());
    }
  else
    {
      std::shared_ptr<SubtractOperator> oper = std::make_shared<SubtractOperator>();
      return binary_helper(shared_from_this(), other.shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> ExpressionBase::operator*(ExpressionBase& other)
{
  if (other.is_constant_type() && other.evaluate() == 1.0)
    {
      return shared_from_this();
    }
  else if (other.is_constant_type() && other.evaluate() == 0.0)
    {
      return other.shared_from_this();
    }
  else if (is_constant_type() && evaluate() == 1.0)
    {
      return other.shared_from_this();
    }
  else if (is_constant_type() && evaluate() == 0.0)
    {
      return shared_from_this();
    }
  else if (is_constant_type() && other.is_constant_type())
    {
      return std::make_shared<Constant>(evaluate() * other.evaluate());
    }
  else
    {
      std::shared_ptr<MultiplyOperator> oper = std::make_shared<MultiplyOperator>();
      return binary_helper(shared_from_this(), other.shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> ExpressionBase::operator/(ExpressionBase& other)
{
  if (other.is_constant_type() && other.evaluate() == 1.0)
    {
      return shared_from_this();
    }
  else if (other.is_constant_type() && other.evaluate() == 0.0)
    {
      assert (false);
    }
  else if (is_constant_type() && evaluate() == 0.0)
    {
      return shared_from_this();
    }
  else if (is_constant_type() && other.is_constant_type())
    {
      return std::make_shared<Constant>(evaluate() / other.evaluate());
    }
  std::shared_ptr<DivideOperator> oper = std::make_shared<DivideOperator>();
  return binary_helper(shared_from_this(), other.shared_from_this(), oper);
}


std::shared_ptr<ExpressionBase> ExpressionBase::__pow__(ExpressionBase& other)
{
  if (other.is_constant_type() && other.evaluate() == 1.0)
    {
      return shared_from_this();
    }
  else if (other.is_constant_type() && other.evaluate() == 0.0)
    {
      return std::make_shared<Constant>(1.0);
    }
  else if (is_constant_type() && evaluate() == 1.0)
    {
      return shared_from_this();
    }
  else if (is_constant_type() && evaluate() == 0.0)
    {
      return shared_from_this();
    }
  else if (is_constant_type() && other.is_constant_type())
    {
      return std::make_shared<Constant>(std::pow(evaluate(), other.evaluate()));
    }
  else
    {
      std::shared_ptr<PowerOperator> oper = std::make_shared<PowerOperator>();
      return binary_helper(shared_from_this(), other.shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> ExpressionBase::operator-()
{
  if (is_constant_type())
    {
      return std::make_shared<Constant>(-evaluate());
    }
  else
    {
      std::shared_ptr<NegationOperator> oper = std::make_shared<NegationOperator>();
      return unary_helper(shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> appsi_exp(std::shared_ptr<ExpressionBase> n)
{
  if (n->is_constant_type())
    {
      return std::make_shared<Constant>(std::exp(n->evaluate()));
    }
  else
    {
      std::shared_ptr<ExpOperator> oper = std::make_shared<ExpOperator>();
      return unary_helper(n->shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> appsi_log(std::shared_ptr<ExpressionBase> n)
{
  if (n->is_constant_type())
    {
      return std::make_shared<Constant>(std::log(n->evaluate()));
    }
  else
    {
      std::shared_ptr<LogOperator> oper = std::make_shared<LogOperator>();
      return unary_helper(n->shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> appsi_log10(std::shared_ptr<ExpressionBase> n)
{
  if (n->is_constant_type())
    {
      return std::make_shared<Constant>(std::log10(n->evaluate()));
    }
  else
    {
      std::shared_ptr<Log10Operator> oper = std::make_shared<Log10Operator>();
      return unary_helper(n->shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> appsi_sin(std::shared_ptr<ExpressionBase> n)
{
  if (n->is_constant_type())
    {
      return std::make_shared<Constant>(std::sin(n->evaluate()));
    }
  else
    {
      std::shared_ptr<SinOperator> oper = std::make_shared<SinOperator>();
      return unary_helper(n->shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> appsi_cos(std::shared_ptr<ExpressionBase> n)
{
  if (n->is_constant_type())
    {
      return std::make_shared<Constant>(std::cos(n->evaluate()));
    }
  else
    {
      std::shared_ptr<CosOperator> oper = std::make_shared<CosOperator>();
      return unary_helper(n->shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> appsi_tan(std::shared_ptr<ExpressionBase> n)
{
  if (n->is_constant_type())
    {
      return std::make_shared<Constant>(std::tan(n->evaluate()));
    }
  else
    {
      std::shared_ptr<TanOperator> oper = std::make_shared<TanOperator>();
      return unary_helper(n->shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> appsi_asin(std::shared_ptr<ExpressionBase> n)
{
  if (n->is_constant_type())
    {
      return std::make_shared<Constant>(std::asin(n->evaluate()));
    }
  else
    {
      std::shared_ptr<AsinOperator> oper = std::make_shared<AsinOperator>();
      return unary_helper(n->shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> appsi_acos(std::shared_ptr<ExpressionBase> n)
{
  if (n->is_constant_type())
    {
      return std::make_shared<Constant>(std::acos(n->evaluate()));
    }
  else
    {
      std::shared_ptr<AcosOperator> oper = std::make_shared<AcosOperator>();
      return unary_helper(n->shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> appsi_atan(std::shared_ptr<ExpressionBase> n)
{
  if (n->is_constant_type())
    {
      return std::make_shared<Constant>(std::atan(n->evaluate()));
    }
  else
    {
      std::shared_ptr<AtanOperator> oper = std::make_shared<AtanOperator>();
      return unary_helper(n->shared_from_this(), oper);
    }
}


std::shared_ptr<ExpressionBase> appsi_sum(std::vector<std::shared_ptr<ExpressionBase> > &exprs_to_sum)
{
  if (exprs_to_sum.size() == 0)
    {
      return std::make_shared<Constant>(0.0);
    }
  std::shared_ptr<Expression> res = std::make_shared<Expression>();
  std::shared_ptr<SumOperator> sum_op = std::make_shared<SumOperator>();
  for (std::shared_ptr<ExpressionBase> &e : exprs_to_sum)
    {
      if (e->is_leaf())
	{
	  sum_op->operands->push_back(e);
	}
      else
	{
	  std::shared_ptr<Expression> other = std::dynamic_pointer_cast<Expression>(e);
	  res->extend_operators(other);
	  sum_op->operands->push_back(other->last_operator());
	}
    }
  res->add_operator(sum_op);
  return res;
}


std::shared_ptr<ExpressionBase> ExpressionBase::operator+(double other)
{
  std::shared_ptr<Constant> other_const = std::make_shared<Constant>(other);
  return (*this) + (*other_const);
}


std::shared_ptr<ExpressionBase> ExpressionBase::operator*(double other)
{
  std::shared_ptr<Constant> other_const = std::make_shared<Constant>(other);
  return (*this) * (*other_const);
}


std::shared_ptr<ExpressionBase> ExpressionBase::operator-(double other)
{
  std::shared_ptr<Constant> other_const = std::make_shared<Constant>(other);
  return (*this) - (*other_const);
}


std::shared_ptr<ExpressionBase> ExpressionBase::operator/(double other)
{
  std::shared_ptr<Constant> other_const = std::make_shared<Constant>(other);
  return (*this) / (*other_const);
}


std::shared_ptr<ExpressionBase> ExpressionBase::__pow__(double other)
{
  std::shared_ptr<Constant> other_const = std::make_shared<Constant>(other);
  return this->__pow__(*other_const);
}


std::shared_ptr<ExpressionBase> ExpressionBase::__radd__(double other)
{
  std::shared_ptr<Constant> other_const = std::make_shared<Constant>(other);
  return (*other_const) + (*this);
}


std::shared_ptr<ExpressionBase> ExpressionBase::__rmul__(double other)
{
  std::shared_ptr<Constant> other_const = std::make_shared<Constant>(other);
  return (*other_const) * (*this);
}


std::shared_ptr<ExpressionBase> ExpressionBase::__rsub__(double other)
{
  std::shared_ptr<Constant> other_const = std::make_shared<Constant>(other);
  return (*other_const) - (*this);
}


std::shared_ptr<ExpressionBase> ExpressionBase::__rdiv__(double other)
{
  std::shared_ptr<Constant> other_const = std::make_shared<Constant>(other);
  return (*other_const) / (*this);
}


std::shared_ptr<ExpressionBase> ExpressionBase::__rtruediv__(double other)
{
  std::shared_ptr<Constant> other_const = std::make_shared<Constant>(other);
  return (*other_const) / (*this);
}


std::shared_ptr<ExpressionBase> ExpressionBase::__rpow__(double other)
{
  std::shared_ptr<Constant> other_const = std::make_shared<Constant>(other);
  return other_const->__pow__(*this);
}


bool Leaf::is_leaf()
{
  return true;
}


bool Var::is_variable_type()
{
  return true;
}


bool Param::is_param_type()
{
  return true;
}


bool Constant::is_constant_type()
{
  return true;
}


bool Expression::is_expression_type()
{
  return true;
}


double Leaf::evaluate()
{
  return value;
}


bool Operator::is_operator_type()
{
  return true;
}


std::shared_ptr<Expression> Expression::copy_expr()
{
  std::shared_ptr<Expression> new_expr = std::make_shared<Expression>();
  if (operators->size() == n_operators())
    {
      new_expr->operators = operators;
      new_expr->_n_operators = n_operators();
    }
  else
    {
      for (unsigned int i=0; i<n_operators(); ++i)
	{
	  new_expr->add_operator(operators->at(i));
	}
    }
  return new_expr;
}


std::shared_ptr<Operator> Expression::last_operator()
{
  return operators->at(n_operators() - 1);
}


void Expression::add_operator(std::shared_ptr<Operator> op)
{
  if (n_operators() == operators->size())
    {
      operators->push_back(op);
      _n_operators += 1;
    }
  else
    {
      throw std::string("Cannot modify this expression");
    }
}


void Expression::extend_operators(std::shared_ptr<Expression> other)
{
  for (unsigned int i=0; i<other->n_operators(); ++i)
    {
      add_operator(other->operators->at(i));
    }
}


std::vector<std::shared_ptr<Operator> > Expression::get_operators()
{
  std::vector<std::shared_ptr<Operator> > res;
  for (unsigned int i=0; i<n_operators(); ++i)
    {
      res.push_back(operators->at(i));
    }
  return res;
}

double Leaf::get_value_from_array(double* val_array)
{
  return value;
}


double Expression::get_value_from_array(double* val_array)
{
  return val_array[n_operators()-1];
}


double Operator::get_value_from_array(double* val_array)
{
  return val_array[index];
}


void MultiplyOperator::evaluate(double* values)
{
  values[index] = operand1->get_value_from_array(values) * operand2->get_value_from_array(values);
}


void ExternalOperator::evaluate(double* values)
{
  // It would be nice to implement this, but it will take some more work.
  // This would require dynamic linking to the external function.
  assert (false);
}


void AddOperator::evaluate(double* values)
{
  values[index] = operand1->get_value_from_array(values) + operand2->get_value_from_array(values);
}


void LinearOperator::evaluate(double* values)
{
  values[index] = constant->evaluate();
  for (unsigned int i=0; i<variables->size(); ++i)
    {
      values[index] += coefficients->at(i)->evaluate() * variables->at(i)->evaluate();
    }
}


void SumOperator::evaluate(double* values)
{
  values[index] = 0.0;
  for (std::shared_ptr<Node> &n : *operands)
    {
      values[index] += n->get_value_from_array(values);
    }
}


void SubtractOperator::evaluate(double* values)
{
  values[index] = operand1->get_value_from_array(values) - operand2->get_value_from_array(values);
}


void DivideOperator::evaluate(double* values)
{
  values[index] = operand1->get_value_from_array(values) / operand2->get_value_from_array(values);
}


void PowerOperator::evaluate(double* values)
{
  values[index] = std::pow(operand1->get_value_from_array(values), operand2->get_value_from_array(values));
}


void NegationOperator::evaluate(double* values)
{
  values[index] = -operand->get_value_from_array(values);
}


void ExpOperator::evaluate(double* values)
{
  values[index] = std::exp(operand->get_value_from_array(values));
}


void LogOperator::evaluate(double* values)
{
  values[index] = std::log(operand->get_value_from_array(values));
}


void Log10Operator::evaluate(double* values)
{
  values[index] = std::log10(operand->get_value_from_array(values));
}


void SinOperator::evaluate(double* values)
{
  values[index] = std::sin(operand->get_value_from_array(values));
}


void CosOperator::evaluate(double* values)
{
  values[index] = std::cos(operand->get_value_from_array(values));
}


void TanOperator::evaluate(double* values)
{
  values[index] = std::tan(operand->get_value_from_array(values));
}


void AsinOperator::evaluate(double* values)
{
  values[index] = std::asin(operand->get_value_from_array(values));
}


void AcosOperator::evaluate(double* values)
{
  values[index] = std::acos(operand->get_value_from_array(values));
}


void AtanOperator::evaluate(double* values)
{
  values[index] = std::atan(operand->get_value_from_array(values));
}


double Expression::evaluate()
{
  double* values = new double[n_operators()];
  for (unsigned int i=0; i<n_operators(); ++i)
    {
      operators->at(i)->index = i;
      operators->at(i)->evaluate(values);
    }
  double res = get_value_from_array(values);
  delete[] values;
  return res;
}


void UnaryOperator::identify_variables(std::set<std::shared_ptr<Node> > &var_set)
{
  if (operand->is_variable_type())
    {
      var_set.insert(operand);
    }
}


void BinaryOperator::identify_variables(std::set<std::shared_ptr<Node> > &var_set)
{
  if (operand1->is_variable_type())
    {
      var_set.insert(operand1);
    }
  if (operand2->is_variable_type())
    {
      var_set.insert(operand2);
    }
}


void ExternalOperator::identify_variables(std::set<std::shared_ptr<Node> > &var_set)
{
  for (unsigned int i=0; i<operands->size(); ++i)
    {
      if (operands->at(i)->is_variable_type())
	{
	  var_set.insert(operands->at(i));
	}
    }
}


void LinearOperator::identify_variables(std::set<std::shared_ptr<Node> > &var_set)
{
  for (std::shared_ptr<Var> v : *variables)
    {
      var_set.insert(v);
    }
}


void SumOperator::identify_variables(std::set<std::shared_ptr<Node> > &var_set)
{
  for (std::shared_ptr<Node> &n : *operands)
    {
      if (n->is_variable_type())
	{
	  var_set.insert(n);
	}
    }
}


std::shared_ptr<std::vector<std::shared_ptr<Var> > > Expression::identify_variables()
{
  std::set<std::shared_ptr<Node> > var_set;
  for (unsigned int i=0; i<n_operators(); ++i)
    {
      operators->at(i)->identify_variables(var_set);
    }
  std::shared_ptr<std::vector<std::shared_ptr<Var> > > res = std::make_shared<std::vector<std::shared_ptr<Var> > >();
  for (std::shared_ptr<Node> v : var_set)
    {
      res->push_back(std::dynamic_pointer_cast<Var>(v));
    }
  return res;
}


std::shared_ptr<std::vector<std::shared_ptr<Var> > > Var::identify_variables()
{
  std::shared_ptr<std::vector<std::shared_ptr<Var> > > res = std::make_shared<std::vector<std::shared_ptr<Var> > >();
  res->push_back(shared_from_this());
  return res;
}


std::shared_ptr<std::vector<std::shared_ptr<Var> > > Constant::identify_variables()
{
  std::shared_ptr<std::vector<std::shared_ptr<Var> > > res = std::make_shared<std::vector<std::shared_ptr<Var> > >();
  return res;
}


std::shared_ptr<std::vector<std::shared_ptr<Var> > > Param::identify_variables()
{
  std::shared_ptr<std::vector<std::shared_ptr<Var> > > res = std::make_shared<std::vector<std::shared_ptr<Var> > >();
  return res;
}


std::shared_ptr<std::vector<std::shared_ptr<ExternalOperator> > > Expression::identify_external_operators()
{
  std::set<std::shared_ptr<Node> > external_set;
  for (unsigned int i=0; i<n_operators(); ++i)
    {
      if (operators->at(i)->is_external_operator())
	{
	  external_set.insert(operators->at(i));
	}
    }
  std::shared_ptr<std::vector<std::shared_ptr<ExternalOperator> > > res = std::make_shared<std::vector<std::shared_ptr<ExternalOperator> > >();
  for (std::shared_ptr<Node> n : external_set)
    {
      res->push_back(std::dynamic_pointer_cast<ExternalOperator>(n));
    }
  return res;
}


std::shared_ptr<std::vector<std::shared_ptr<ExternalOperator> > > Var::identify_external_operators()
{
  std::shared_ptr<std::vector<std::shared_ptr<ExternalOperator> > > res = std::make_shared<std::vector<std::shared_ptr<ExternalOperator> > >();
  return res;
}


std::shared_ptr<std::vector<std::shared_ptr<ExternalOperator> > > Constant::identify_external_operators()
{
  std::shared_ptr<std::vector<std::shared_ptr<ExternalOperator> > > res = std::make_shared<std::vector<std::shared_ptr<ExternalOperator> > >();
  return res;
}


std::shared_ptr<std::vector<std::shared_ptr<ExternalOperator> > > Param::identify_external_operators()
{
  std::shared_ptr<std::vector<std::shared_ptr<ExternalOperator> > > res = std::make_shared<std::vector<std::shared_ptr<ExternalOperator> > >();
  return res;
}


bool Leaf::get_unique_degree_from_array(bool* unique_degrees)
{
  return true;
}


bool Expression::get_unique_degree_from_array(bool* unique_degrees)
{
  return unique_degrees[n_operators()-1];
}


bool Operator::get_unique_degree_from_array(bool* unique_degrees)
{
  return unique_degrees[index];
}


std::shared_ptr<Repn> ExpressionBase::get_repn_from_vector(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  return generate_repn();
}


std::shared_ptr<Repn> Expression::get_repn_from_vector(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  if (unique_degrees[n_operators()-1])
    {
      std::shared_ptr<Repn> repn = std::make_shared<Repn>();
      repn->reset_with_constants();
      int deg = degrees[n_operators()-1];
      if (deg == 0)
	{
	  repn->constant = shared_from_this();
	}
      else if (deg == 1)
	{
	  repn->linear = shared_from_this();
	}
      else if (deg == 2)
	{
	  repn->quadratic = shared_from_this();
	}
      else
	{
	  repn->nonlinear = shared_from_this();
	}
      return repn;
    }
  else
    {
      return repns.at(n_operators()-1);
    }
}


std::shared_ptr<Repn> Operator::get_repn_from_vector(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  if (unique_degrees[index])
    {
      std::shared_ptr<ExpressionBase> expr = expression_from_operator();
      std::shared_ptr<Repn> repn = repns[index];
      repn->reset_with_constants();
      int deg = degrees[index];
      if (deg == 0)
	{
	  repn->constant = expr;
	}
      else if (deg == 1)
	{
	  repn->linear = expr;
	}
      else if (deg == 2)
	{
	  repn->quadratic = expr;
	}
      else
	{
	  repn->nonlinear = expr;
	}
      return repn;
    }
  else
    {
      return repns[index];
    }
}


std::shared_ptr<ExpressionBase> Operator::expression_from_operator()
{
  std::shared_ptr<std::vector<std::shared_ptr<Node> > > prefix_notation = std::make_shared<std::vector<std::shared_ptr<Node> > >();
  std::shared_ptr<std::vector<std::shared_ptr<Node> > > stack = std::make_shared<std::vector<std::shared_ptr<Node> > >();
  std::shared_ptr<Node> node;
  stack->push_back(shared_from_this());
  while (stack->size() > 0)
    {
      node = stack->back();
      stack->pop_back();
      prefix_notation->push_back(node);
      node->fill_prefix_notation_stack(stack);
    }
  
  std::shared_ptr<Expression> res = std::make_shared<Expression>();
  int ndx = prefix_notation->size() - 1;
  while (ndx >= 0)
    {
      if (prefix_notation->at(ndx)->is_operator_type())
	{
	  res->add_operator(std::dynamic_pointer_cast<Operator>(prefix_notation->at(ndx)));
	}
      ndx -= 1;
    }
  return res;
}


int Var::get_degree_from_array(int* degree_array)
{
  return 1;
}


int Param::get_degree_from_array(int* degree_array)
{
  return 0;
}


int Constant::get_degree_from_array(int* degree_array)
{
  return 0;
}


int Expression::get_degree_from_array(int* degree_array)
{
  return degree_array[n_operators()-1];
}


int Operator::get_degree_from_array(int* degree_array)
{
  return degree_array[index];
}


void LinearOperator::propagate_degree_forward(int* degrees, double* values)
{
  degrees[index] = 1;
}


void SumOperator::propagate_degree_forward(int* degrees, double* values)
{
  int deg = 0;
  int _deg;
  for (std::shared_ptr<Node> &n : *operands)
    {
      _deg = n->get_degree_from_array(degrees);
      if (_deg > deg)
	{
	  deg = _deg;
	}
    }
  degrees[index] = deg;
}


void MultiplyOperator::propagate_degree_forward(int* degrees, double* values)
{
  degrees[index] = operand1->get_degree_from_array(degrees) + operand2->get_degree_from_array(degrees);
}


void ExternalOperator::propagate_degree_forward(int* degrees, double* values)
{
  // External functions are always considered nonlinear
  // Anything larger than 2 is nonlinear
  degrees[index] = 3;
}


void AddOperator::propagate_degree_forward(int* degrees, double* values)
{
  degrees[index] = std::max(operand1->get_degree_from_array(degrees), operand2->get_degree_from_array(degrees));
}


void SubtractOperator::propagate_degree_forward(int* degrees, double* values)
{
  degrees[index] = std::max(operand1->get_degree_from_array(degrees), operand2->get_degree_from_array(degrees));
}


void DivideOperator::propagate_degree_forward(int* degrees, double* values)
{
  // anything larger than 2 is nonlinear
  degrees[index] = std::max(operand1->get_degree_from_array(degrees), 3*(operand2->get_degree_from_array(degrees)));
}


void PowerOperator::propagate_degree_forward(int* degrees, double* values)
{
  if (operand2->get_degree_from_array(degrees) != 0)
    {
      degrees[index] = 3;
    }
  else
    {
      double val2 = operand2->get_value_from_array(values);
      double intpart;
      if (std::modf(val2, &intpart) == 0.0)
	{
	  degrees[index] = operand1->get_degree_from_array(degrees) * (int)val2;
	}
      else
	{
	  degrees[index] = 3;
	}
    }
}


void NegationOperator::propagate_degree_forward(int* degrees, double* values)
{
  degrees[index] = operand->get_degree_from_array(degrees);
}


void UnaryOperator::propagate_degree_forward(int* degrees, double* values)
{
  if (operand->get_degree_from_array(degrees) == 0)
    {
      degrees[index] = 0;
    }
  else
    {
      degrees[index] = 3;
    }
}


std::shared_ptr<ExpressionBase> NegationOperator::call_unary_function(std::shared_ptr<ExpressionBase> e)
{
  return -(*e);
}


std::shared_ptr<ExpressionBase> ExpOperator::call_unary_function(std::shared_ptr<ExpressionBase> e)
{
  return appsi_exp(e);
}


std::shared_ptr<ExpressionBase> LogOperator::call_unary_function(std::shared_ptr<ExpressionBase> e)
{
  return appsi_log(e);
}


std::shared_ptr<ExpressionBase> Log10Operator::call_unary_function(std::shared_ptr<ExpressionBase> e)
{
  return appsi_log10(e);
}


std::shared_ptr<ExpressionBase> SinOperator::call_unary_function(std::shared_ptr<ExpressionBase> e)
{
  return appsi_sin(e);
}


std::shared_ptr<ExpressionBase> CosOperator::call_unary_function(std::shared_ptr<ExpressionBase> e)
{
  return appsi_cos(e);
}


std::shared_ptr<ExpressionBase> TanOperator::call_unary_function(std::shared_ptr<ExpressionBase> e)
{
  return appsi_tan(e);
}


std::shared_ptr<ExpressionBase> AsinOperator::call_unary_function(std::shared_ptr<ExpressionBase> e)
{
  return appsi_asin(e);
}


std::shared_ptr<ExpressionBase> AcosOperator::call_unary_function(std::shared_ptr<ExpressionBase> e)
{
  return appsi_acos(e);
}


std::shared_ptr<ExpressionBase> AtanOperator::call_unary_function(std::shared_ptr<ExpressionBase> e)
{
  return appsi_atan(e);
}


void Repn::reset_with_constants()
{
  constant = std::make_shared<Constant>();
  linear = std::make_shared<Constant>();
  quadratic = std::make_shared<Constant>();
  nonlinear = std::make_shared<Constant>();
}


void AddOperator::generate_repn(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  repns.push_back(res);
  if (!(get_unique_degree_from_array(unique_degrees)))
    {
      std::shared_ptr<Repn> repn1, repn2;
      repn1 = operand1->get_repn_from_vector(repns, degrees, unique_degrees);
      repn2 = operand2->get_repn_from_vector(repns, degrees, unique_degrees);
      res->constant = *(repn1->constant) + *(repn2->constant); 
      res->linear = *(repn1->linear) + *(repn2->linear); 
      res->quadratic = *(repn1->quadratic) + *(repn2->quadratic); 
      res->nonlinear = *(repn1->nonlinear) + *(repn2->nonlinear);
    }
}


void SubtractOperator::generate_repn(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  repns.push_back(res);
  if (!(get_unique_degree_from_array(unique_degrees)))
    {
      std::shared_ptr<Repn> repn1, repn2;
      repn1 = operand1->get_repn_from_vector(repns, degrees, unique_degrees);
      repn2 = operand2->get_repn_from_vector(repns, degrees, unique_degrees);
      res->constant = *(repn1->constant) - *(repn2->constant); 
      res->linear = *(repn1->linear) - *(repn2->linear); 
      res->quadratic = *(repn1->quadratic) - *(repn2->quadratic); 
      res->nonlinear = *(repn1->nonlinear) - *(repn2->nonlinear);
    }
}


void SumOperator::generate_repn(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  repns.push_back(res);
  if (!(get_unique_degree_from_array(unique_degrees)))
    {
      std::shared_ptr<Repn> other;
      std::vector<std::shared_ptr<ExpressionBase> > constants;
      std::vector<std::shared_ptr<ExpressionBase> > linears;
      std::vector<std::shared_ptr<ExpressionBase> > quadratics;
      std::vector<std::shared_ptr<ExpressionBase> > nonlinears;
      for (std::shared_ptr<Node> &op : (*operands))
        {
	  other = op->get_repn_from_vector(repns, degrees, unique_degrees);
      	  constants.push_back(other->constant);
      	  linears.push_back(other->linear);
      	  quadratics.push_back(other->quadratic);
      	  nonlinears.push_back(other->nonlinear);
        }
      res->constant = appsi_sum(constants);
      res->linear = appsi_sum(linears);
      res->quadratic = appsi_sum(quadratics);
      res->nonlinear = appsi_sum(nonlinears);
    }
}


void MultiplyOperator::generate_repn(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  repns.push_back(res);
  if (!(get_unique_degree_from_array(unique_degrees)))
    {
      std::shared_ptr<Repn> repn1, repn2;
      repn1 = operand1->get_repn_from_vector(repns, degrees, unique_degrees);
      repn2 = operand2->get_repn_from_vector(repns, degrees, unique_degrees);
      res->constant = *(repn1->constant) * *(repn2->constant);
      res->linear = *(*(repn1->constant) * *(repn2->linear)) + *(*(repn2->constant) * *(repn1->linear));
      res->quadratic = *(*(*(repn1->linear) * *(repn2->linear)) + *(*(repn1->constant) * *(repn2->quadratic))) + *(*(repn1->quadratic) * *(repn2->constant));
      res->nonlinear = (*(*(*(*(*(*(*(*(*(*(repn1->nonlinear) * *(repn2->constant))
					+ *(*(repn1->nonlinear) * *(repn2->linear)))
				      + *(*(repn1->nonlinear) * *(repn2->quadratic)))
				    + *(*(repn1->nonlinear) * *(repn2->nonlinear)))
				  + *(*(repn1->quadratic) * *(repn2->linear)))
				+ *(*(repn1->quadratic) * *(repn2->quadratic)))
			      + *(*(repn1->quadratic) * *(repn2->nonlinear)))
			    + *(*(repn1->linear) * *(repn2->quadratic)))
			  + *(*(repn1->linear) * *(repn2->nonlinear)))
			+ *(*(repn1->constant) * *(repn2->nonlinear)));
    }
}


void DivideOperator::generate_repn(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  repns.push_back(res);
  if (!(get_unique_degree_from_array(unique_degrees)))
    {
      std::shared_ptr<Repn> repn1, repn2;
      repn1 = operand1->get_repn_from_vector(repns, degrees, unique_degrees);
      repn2 = operand2->get_repn_from_vector(repns, degrees, unique_degrees);
      int deg2;
      deg2 = operand2->get_degree_from_array(degrees);
      if (deg2 != 0)
	{
	  res->reset_with_constants();
	  res->nonlinear = *(*(*(*(repn1->constant) + *(repn1->linear)) + *(repn1->quadratic)) + *(repn1->nonlinear)) / *(*(*(*(repn2->constant) + *(repn2->linear)) + *(repn2->quadratic)) + *(repn2->nonlinear));
	}
      else
	{
	  std::shared_ptr<ExpressionBase> denom = (*(*(*(repn2->constant) + *(repn2->linear)) + *(repn2->quadratic)) + *(repn2->nonlinear));
	  res->constant = *(repn1->constant) / *denom;
	  res->linear = *(repn1->linear) / *denom;
	  res->quadratic = *(repn1->quadratic) / *denom;
	  res->nonlinear = *(repn1->nonlinear) / *denom;
	}
    }
}


void PowerOperator::generate_repn(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  repns.push_back(res);
  if (!(get_unique_degree_from_array(unique_degrees)))
    {
      std::shared_ptr<Repn> repn1, repn2;
      repn1 = operand1->get_repn_from_vector(repns, degrees, unique_degrees);
      repn2 = operand2->get_repn_from_vector(repns, degrees, unique_degrees);
      int deg2;
      deg2 = operand2->get_degree_from_array(degrees);
      if (operand2->is_constant_type())
	{
	  std::shared_ptr<Constant> op2 = std::dynamic_pointer_cast<Constant>(operand2);
	  if (op2->value == 0)
	    {
	      res->reset_with_constants();
	      res->constant = std::make_shared<Constant>(1.0);
	    }
	  else if (op2->value == 1)
	    {
	      res->constant = repn1->constant;
	      res->linear = repn1->linear;
	      res->quadratic = repn1->quadratic;
	      res->nonlinear = repn1->nonlinear;
	    }
	  else if (op2->value == 2)
	    {
	      res->constant = repn1->constant->__pow__(2);
	      res->linear = Constant(2) * *(*(repn1->constant) * *(repn1->linear));
	      res->quadratic = *(Constant(2) * *(*(repn1->constant) * *(repn1->quadratic))) + *(repn1->linear->__pow__(2));
	      res->nonlinear = *(*(*(*(*(Constant(2) * *(*(repn1->constant) * *(repn1->nonlinear)))
				       + *(Constant(2) * *(*(repn1->linear) * *(repn1->quadratic))))
				     + *(Constant(2) * *(*(repn1->linear) * *(repn1->nonlinear))))
				   + *(repn1->quadratic->__pow__(2)))
				 + *(Constant(2) * *(*(repn1->quadratic) * *(repn1->nonlinear)))) + *(*(repn1->nonlinear) * *(repn1->nonlinear));
	    }
	  else
	    {
	      std::shared_ptr<ExpressionBase> base_expr = (*(*(*(repn1->constant) + *(repn1->linear)) + *(repn1->quadratic)) + *(repn1->nonlinear));
	      res->reset_with_constants();
	      res->nonlinear = base_expr->__pow__(*op2);
	    }
	}
      else
	{
	  std::shared_ptr<ExpressionBase> base_expr = (*(*(*(repn1->constant) + *(repn1->linear)) + *(repn1->quadratic)) + *(repn1->nonlinear));
	  std::shared_ptr<ExpressionBase> pow_expr = (*(*(*(repn2->constant) + *(repn2->linear)) + *(repn2->quadratic)) + *(repn2->nonlinear));
	  res->reset_with_constants();
	  res->nonlinear = base_expr->__pow__(*pow_expr);
	}
    }
}


void NegationOperator::generate_repn(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  repns.push_back(res);
  if (!(get_unique_degree_from_array(unique_degrees)))
    {
      std::shared_ptr<Repn> repn1;
      repn1 = operand->get_repn_from_vector(repns, degrees, unique_degrees);
      res->constant = - *(repn1->constant); 
      res->linear = - *(repn1->linear); 
      res->quadratic = - *(repn1->quadratic); 
      res->nonlinear = - *(repn1->nonlinear);
    }
}


void UnaryOperator::generate_repn(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  repns.push_back(res);
  if (!(get_unique_degree_from_array(unique_degrees)))
    {
      std::shared_ptr<Repn> repn1;
      repn1 = operand->get_repn_from_vector(repns, degrees, unique_degrees);
      int deg1;
      deg1 = operand->get_degree_from_array(degrees);
      if (deg1 == 0)
	{
	  res->reset_with_constants();
	  res->constant = call_unary_function(repn1->constant);
	}
      else
	{
	  res->reset_with_constants();
	  std::shared_ptr<ExpressionBase> exp_expr = (*(*(*(repn1->constant) + *(repn1->linear)) + *(repn1->quadratic)) + *(repn1->nonlinear));
	  res->nonlinear = call_unary_function(exp_expr);
	}
    }
}


void ExternalOperator::generate_repn(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  repns.push_back(res);
  if (!(get_unique_degree_from_array(unique_degrees)))
    {
      res->reset_with_constants();
      res->nonlinear = expression_from_operator();
    }
}


void LinearOperator::generate_repn(std::vector<std::shared_ptr<Repn> >& repns, int* degrees, bool* unique_degrees)
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  repns.push_back(res);
  if (!(get_unique_degree_from_array(unique_degrees)))
    {
      res->reset_with_constants();
      res->constant = constant;
      std::shared_ptr<Expression> linear_expr = std::make_shared<Expression>();
      std::shared_ptr<LinearOperator> op = std::make_shared<LinearOperator>();
      op->variables = variables;
      op->coefficients = coefficients;
      op->constant = std::make_shared<Constant>(0);
      linear_expr->add_operator(op);
      res->linear = linear_expr;
    }
}


std::shared_ptr<Repn> Expression::generate_repn()
{
  std::vector<std::shared_ptr<Repn> > repns;
  int degrees[n_operators()];
  double values[n_operators()];
  bool unique_degrees[n_operators()];
  std::shared_ptr<Operator> oper;
  for (unsigned int i=0; i<n_operators(); ++i)
    {
      oper = operators->at(i);
      oper->index = i;
      oper->evaluate(values);
      oper->propagate_degree_forward(degrees, values);
      oper->propagate_unique_degree(degrees, unique_degrees);
      oper->generate_repn(repns, degrees, unique_degrees);
    }

  return get_repn_from_vector(repns, degrees, unique_degrees);
}


std::shared_ptr<Repn> Var::generate_repn()
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  res->reset_with_constants();
  res->linear = shared_from_this();
  return res;
}


std::shared_ptr<Repn> Constant::generate_repn()
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  res->reset_with_constants();
  res->constant = shared_from_this();
  return res;
}


std::shared_ptr<Repn> Param::generate_repn()
{
  std::shared_ptr<Repn> res = std::make_shared<Repn>();
  res->reset_with_constants();
  res->constant = shared_from_this();
  return res;
}


void AddOperator::propagate_unique_degree(int* degrees, bool* unique_degrees)
{
  int deg1, deg2;
  deg1 = operand1->get_degree_from_array(degrees);
  deg2 = operand2->get_degree_from_array(degrees);
  bool unique1, unique2;
  unique1 = operand1->get_unique_degree_from_array(unique_degrees);
  unique2 = operand2->get_unique_degree_from_array(unique_degrees);
  if (unique1 && unique2 && (deg1 == deg2))
    {
      unique_degrees[index] = true;
    }
  else
    {
      unique_degrees[index] = false;
    }
}


void SumOperator::propagate_unique_degree(int* degrees, bool* unique_degrees)
{
  int deg;
  deg = operands->at(0)->get_degree_from_array(degrees);
  unique_degrees[index] = true;
  for (std::shared_ptr<Node> &op : *(operands))
    {
      if (deg != op->get_degree_from_array(degrees) || !(op->get_unique_degree_from_array(unique_degrees)))
	{
	  unique_degrees[index] = false;
	  break;
	}
    }
}


void SubtractOperator::propagate_unique_degree(int* degrees, bool* unique_degrees)
{
  int deg1, deg2;
  deg1 = operand1->get_degree_from_array(degrees);
  deg2 = operand2->get_degree_from_array(degrees);
  bool unique1, unique2;
  unique1 = operand1->get_unique_degree_from_array(unique_degrees);
  unique2 = operand2->get_unique_degree_from_array(unique_degrees);
  if (unique1 && unique2 && (deg1 == deg2))
    {
      unique_degrees[index] = true;
    }
  else
    {
      unique_degrees[index] = false;
    }
}


void BinaryOperator::propagate_unique_degree(int* degrees, bool* unique_degrees)
{
  bool unique1, unique2;
  unique1 = operand1->get_unique_degree_from_array(unique_degrees);
  unique2 = operand2->get_unique_degree_from_array(unique_degrees);
  
  if (unique1 && unique2)
    {
      unique_degrees[index] = true;
    }
  else
    {
      unique_degrees[index] = false;
    }
}


void ExternalOperator::propagate_unique_degree(int* degrees, bool* unique_degrees)
{
  unique_degrees[index] = true;
  for (std::shared_ptr<Node> &op : *(operands))
    {
      if (!(op->get_unique_degree_from_array(unique_degrees)))
	{
	  unique_degrees[index] = false;
	  break;
	}
    }
}


void UnaryOperator::propagate_unique_degree(int* degrees, bool* unique_degrees)
{
  unique_degrees[index] = operand->get_unique_degree_from_array(unique_degrees);
}


void LinearOperator::propagate_unique_degree(int* degrees, bool* unique_degrees)
{
  if (constant->is_constant_type() && constant->evaluate() == 0)
    {
      unique_degrees[index] = true;
    }
  else
    {
      unique_degrees[index] = false;
    }
}


std::string Var::__str__()
{
  return name;
}


std::string Param::__str__()
{
  return name;
}


std::string Constant::__str__()
{
  return std::to_string(value);
}


std::string Expression::__str__()
{
  std::string* string_array = new std::string[n_operators()];
  std::shared_ptr<Operator> oper;
  for (unsigned int i=0; i<n_operators(); ++i)
    {
      oper = operators->at(i);
      oper->index = i;
      oper->print(string_array);
    }
  std::string res = string_array[n_operators()-1];
  delete[] string_array;
  return res;
}


std::string Leaf::get_string_from_array(std::string* string_array)
{
  return __str__();
}


std::string Expression::get_string_from_array(std::string* string_array)
{
  return string_array[n_operators()-1];
}


std::string Operator::get_string_from_array(std::string* string_array)
{
  return string_array[index];
}


void AddOperator::print(std::string* string_array)
{
  string_array[index] = ("(" +
			 operand1->get_string_from_array(string_array) +
			 " + " +
			 operand2->get_string_from_array(string_array) +
			 ")");
}


void SubtractOperator::print(std::string* string_array)
{
  string_array[index] = ("(" +
			 operand1->get_string_from_array(string_array) +
			 " - " +
			 operand2->get_string_from_array(string_array) +
			 ")");
}


void MultiplyOperator::print(std::string* string_array)
{
  string_array[index] = ("(" +
			 operand1->get_string_from_array(string_array) +
			 "*" +
			 operand2->get_string_from_array(string_array) +
			 ")");
}


void ExternalOperator::print(std::string* string_array)
{
  std::string res = function_name + "(";
  for (unsigned int i=0; i<(operands->size() - 1); ++i)
    {
      res += operands->at(i)->get_string_from_array(string_array);
      res += ", ";
    }
  res += operands->back()->get_string_from_array(string_array);
  res += ")";
  string_array[index] = res;
}


void DivideOperator::print(std::string* string_array)
{
  string_array[index] = ("(" +
			 operand1->get_string_from_array(string_array) +
			 "/" +
			 operand2->get_string_from_array(string_array) +
			 ")");
}


void PowerOperator::print(std::string* string_array)
{
  string_array[index] = ("(" +
			 operand1->get_string_from_array(string_array) +
			 "**" +
			 operand2->get_string_from_array(string_array) +
			 ")");
}


void NegationOperator::print(std::string* string_array)
{
  string_array[index] = ("(-" +
			 operand->get_string_from_array(string_array) +
			 ")");
}


void ExpOperator::print(std::string* string_array)
{
  string_array[index] = ("exp(" +
			 operand->get_string_from_array(string_array) +
			 ")");
}


void LogOperator::print(std::string* string_array)
{
  string_array[index] = ("log(" +
			 operand->get_string_from_array(string_array) +
			 ")");
}


void Log10Operator::print(std::string* string_array)
{
  string_array[index] = ("log10(" +
			 operand->get_string_from_array(string_array) +
			 ")");
}


void SinOperator::print(std::string* string_array)
{
  string_array[index] = ("sin(" +
			 operand->get_string_from_array(string_array) +
			 ")");
}


void CosOperator::print(std::string* string_array)
{
  string_array[index] = ("cos(" +
			 operand->get_string_from_array(string_array) +
			 ")");
}


void TanOperator::print(std::string* string_array)
{
  string_array[index] = ("tan(" +
			 operand->get_string_from_array(string_array) +
			 ")");
}


void AsinOperator::print(std::string* string_array)
{
  string_array[index] = ("asin(" +
			 operand->get_string_from_array(string_array) +
			 ")");
}


void AcosOperator::print(std::string* string_array)
{
  string_array[index] = ("acos(" +
			 operand->get_string_from_array(string_array) +
			 ")");
}


void AtanOperator::print(std::string* string_array)
{
  string_array[index] = ("atan(" +
			 operand->get_string_from_array(string_array) +
			 ")");
}


void LinearOperator::print(std::string* string_array)
{
  std::string res = "(" + constant->__str__();
  for (unsigned int i=0; i<variables->size(); ++i)
    {
      res += " + " + coefficients->at(i)->__str__() + "*" + variables->at(i)->__str__();
    }
  res += ")";
  string_array[index] = res;
}


void SumOperator::print(std::string* string_array)
{
  std::string res = "(" + operands->at(0)->get_string_from_array(string_array);
  for (unsigned int i=1; i<operands->size(); ++ i)
    {
      res += " + " + operands->at(i)->get_string_from_array(string_array);
    }
  res += ")";
  string_array[index] = res;
}


std::shared_ptr<std::vector<std::shared_ptr<Node> > > Leaf::get_prefix_notation()
{
  std::shared_ptr<std::vector<std::shared_ptr<Node> > > res = std::make_shared<std::vector<std::shared_ptr<Node> > >();
  res->push_back(shared_from_this());
  return res;
}


std::shared_ptr<std::vector<std::shared_ptr<Node> > > Expression::get_prefix_notation()
{
  std::shared_ptr<std::vector<std::shared_ptr<Node> > > res = std::make_shared<std::vector<std::shared_ptr<Node> > >();
  std::shared_ptr<std::vector<std::shared_ptr<Node> > > stack = std::make_shared<std::vector<std::shared_ptr<Node> > >();
  std::shared_ptr<Node> node;
  stack->push_back(last_operator());
  while (stack->size() > 0)
    {
      node = stack->back();
      stack->pop_back();
      res->push_back(node);
      node->fill_prefix_notation_stack(stack);
    }
  
  return res;
}


void BinaryOperator::fill_prefix_notation_stack(std::shared_ptr<std::vector<std::shared_ptr<Node> > > stack)
{
  stack->push_back(operand2);
  stack->push_back(operand1);
}


void UnaryOperator::fill_prefix_notation_stack(std::shared_ptr<std::vector<std::shared_ptr<Node> > > stack)
{
  stack->push_back(operand);
}


void SumOperator::fill_prefix_notation_stack(std::shared_ptr<std::vector<std::shared_ptr<Node> > > stack)
{
  int ndx = operands->size() - 1;
  while (ndx >= 0)
    {
      stack->push_back(operands->at(ndx));
      ndx -= 1;
    }
}


void LinearOperator::fill_prefix_notation_stack(std::shared_ptr<std::vector<std::shared_ptr<Node> > > stack)
{
  ; // This is treated as a leaf in this context; write_nl_string will take care of it
}


void ExternalOperator::fill_prefix_notation_stack(std::shared_ptr<std::vector<std::shared_ptr<Node> > > stack)
{
  int i = operands->size() - 1;
  while (i >= 0)
    {
      stack->push_back(operands->at(i));
      i -= 1;
    }
}


void Var::write_nl_string(std::ofstream& f)
{
  f << "v" << index << "\n";
}


void Param::write_nl_string(std::ofstream& f)
{
  f << "n" << value << "\n";
}


void Constant::write_nl_string(std::ofstream& f)
{
  f << "n" << value << "\n";
}


void Expression::write_nl_string(std::ofstream& f)
{
  std::shared_ptr<std::vector<std::shared_ptr<Node> > > prefix_notation = get_prefix_notation();
  for (std::shared_ptr<Node> &node : *(prefix_notation))
    {
      node->write_nl_string(f);
    }
}


void MultiplyOperator::write_nl_string(std::ofstream& f)
{
  f << "o2\n";
}


void ExternalOperator::write_nl_string(std::ofstream& f)
{
  f << "f" << external_function_index << " " << operands->size() << "\n";
}


void SumOperator::write_nl_string(std::ofstream& f)
{
  if (operands->size() == 2)
    {
      f << "o0\n";
    }
  else
    {
      f << "o54\n";
      f << operands->size() << "\n";
    }
}


void LinearOperator::write_nl_string(std::ofstream& f)
{
  bool has_const = (!constant->is_constant_type()) || constant->evaluate() != 0;
  if (has_const)
    {
      if (variables->size() == 1)
	{
	  f << "o0\n";
	}
      else
	{
	  f << "o54\n";
	  f << variables->size() + 1 << "\n";
	}
      f << "n" << constant->evaluate() << "\n";
    }
  else
    {
      if (variables->size() == 2)
	{
	  f << "o0\n";
	}
      else
	{
	  f << "o54\n";
	  f << variables->size() << "\n";
	}
    }
  for (unsigned int ndx=0; ndx<variables->size(); ++ndx)
    {
      f << "o2\n";
      f << "n" <<  coefficients->at(ndx)->evaluate() << "\n";
      variables->at(ndx)->write_nl_string(f);
    }
}


void AddOperator::write_nl_string(std::ofstream& f)
{
  f << "o0\n";
}


void SubtractOperator::write_nl_string(std::ofstream& f)
{
  f << "o1\n";
}


void DivideOperator::write_nl_string(std::ofstream& f)
{
  f << "o3\n";
}


void PowerOperator::write_nl_string(std::ofstream& f)
{
  f << "o5\n";
}


void NegationOperator::write_nl_string(std::ofstream& f)
{
  f << "o16\n";
}


void ExpOperator::write_nl_string(std::ofstream& f)
{
  f << "o44\n";
}


void LogOperator::write_nl_string(std::ofstream& f)
{
  f << "o43\n";
}


void Log10Operator::write_nl_string(std::ofstream& f)
{
  f << "o42\n";
}


void SinOperator::write_nl_string(std::ofstream& f)
{
  f << "o41\n";
}


void CosOperator::write_nl_string(std::ofstream& f)
{
  f << "o46\n";
}


void TanOperator::write_nl_string(std::ofstream& f)
{
  f << "o38\n";
}


void AsinOperator::write_nl_string(std::ofstream& f)
{
  f << "o51\n";
}


void AcosOperator::write_nl_string(std::ofstream& f)
{
  f << "o53\n";
}


void AtanOperator::write_nl_string(std::ofstream& f)
{
  f << "o49\n";
}


bool BinaryOperator::is_binary_operator()
{
  return true;
}


bool UnaryOperator::is_unary_operator()
{
  return true;
}


bool LinearOperator::is_linear_operator()
{
  return true;
}


bool SumOperator::is_sum_operator()
{
  return true;
}


bool MultiplyOperator::is_multiply_operator()
{
  return true;
}


bool AddOperator::is_add_operator()
{
  return true;
}


bool SubtractOperator::is_subtract_operator()
{
  return true;
}


bool DivideOperator::is_divide_operator()
{
  return true;
}


bool PowerOperator::is_power_operator()
{
  return true;
}


bool NegationOperator::is_negation_operator()
{
  return true;
}


bool ExpOperator::is_exp_operator()
{
  return true;
}


bool LogOperator::is_log_operator()
{
  return true;
}


bool ExternalOperator::is_external_operator()
{
  return true;
}


std::string Repn::__str__()
{
  std::string res = "constant: ";
  res += constant->__str__();
  res += "\n";
  res += "linear: ";
  res += linear->__str__();
  res += "\n";
  res += "quadratic: ";
  res += quadratic->__str__();
  res += "\n";
  res += "nonlinear: ";
  res += nonlinear->__str__();
  return res;
}


std::vector<std::shared_ptr<Repn> > generate_repns(std::vector<std::shared_ptr<ExpressionBase> > exprs)
{
  std::vector<std::shared_ptr<Repn> > res;
  for (std::shared_ptr<ExpressionBase> &e : exprs)
    {
      res.push_back(e->generate_repn());
    }
  return res;
}


std::vector<std::shared_ptr<Var> > create_vars(int n_vars)
{
  std::vector<std::shared_ptr<Var> > res;
  for (int i=0; i<n_vars; ++i)
    {
      res.push_back(std::make_shared<Var>());
    }
  return res;
}


std::vector<std::shared_ptr<Param> > create_params(int n_params)
{
  std::vector<std::shared_ptr<Param> > res;
  for (int i=0; i<n_params; ++i)
    {
      res.push_back(std::make_shared<Param>());
    }
  return res;
}


std::vector<std::shared_ptr<Constant> > create_constants(int n_constants)
{
  std::vector<std::shared_ptr<Constant> > res;
  for (int i=0; i<n_constants; ++i)
    {
      res.push_back(std::make_shared<Constant>());
    }
  return res;
}


std::shared_ptr<Node> appsi_operator_from_pyomo_expr(py::handle expr, py::dict var_map, py::dict param_map, PyomoExprTypes& expr_types)
{
  py::type tmp_type = py::type::of(expr);
  std::shared_ptr<Node> res;

  if (tmp_type.is(expr_types.int_) || tmp_type.is(expr_types.float_))
    {
      res =  std::make_shared<Constant>(expr.cast<double>());
    }
  else if (tmp_type.is(expr_types.ScalarParam) || tmp_type.is(expr_types._ParamData))
    {
      res =  param_map[expr_types.id(expr)].cast<std::shared_ptr<Node> >();
    }
  else if (tmp_type.is(expr_types.ScalarVar) || tmp_type.is(expr_types._GeneralVarData))
    {
      res =  var_map[expr_types.id(expr)].cast<std::shared_ptr<Node> >();
    }
  else if (tmp_type.is(expr_types.MonomialTermExpression))
    {
      res =  std::make_shared<MultiplyOperator>();
    }
  else if (tmp_type.is(expr_types.ProductExpression) || tmp_type.is(expr_types.NPV_ProductExpression))
    {
      res =  std::make_shared<MultiplyOperator>();
    }
  else if (tmp_type.is(expr_types.SumExpression) || tmp_type.is(expr_types.NPV_SumExpression))
    {
      res =  std::make_shared<SumOperator>();
    }
  else if (tmp_type.is(expr_types.NegationExpression) || tmp_type.is(expr_types.NPV_NegationExpression))
    {
      res =  std::make_shared<NegationOperator>();
    }
  else if (tmp_type.is(expr_types.DivisionExpression) || tmp_type.is(expr_types.NPV_DivisionExpression))
    {
      res =  std::make_shared<DivideOperator>();
    }
  else if (tmp_type.is(expr_types.LinearExpression))
    {
      res =  std::make_shared<LinearOperator>();
    }
  else if (tmp_type.is(expr_types.PowExpression) || tmp_type.is(expr_types.NPV_PowExpression))
    {
      res =  std::make_shared<PowerOperator>();
    }
  else if (tmp_type.is(expr_types.UnaryFunctionExpression) || tmp_type.is(expr_types.NPV_UnaryFunctionExpression))
    {
      std::string function_name = expr.attr("getname")().cast<std::string>();
      if (function_name == "exp")
	{
	  res =  std::make_shared<ExpOperator>();
	}
      else if (function_name == "log")
	{
	  res =  std::make_shared<LogOperator>();
	}
      else if (function_name == "log10")
	{
	  res =  std::make_shared<Log10Operator>();
	}
      else if (function_name == "sin")
	{
	  res =  std::make_shared<SinOperator>();
	}
      else if (function_name == "cos")
	{
	  res =  std::make_shared<CosOperator>();
	}
      else if (function_name == "tan")
	{
	  res =  std::make_shared<TanOperator>();
	}
      else if (function_name == "asin")
	{
	  res =  std::make_shared<AsinOperator>();
	}
      else if (function_name == "acos")
	{
	  res =  std::make_shared<AcosOperator>();
	}
      else if (function_name == "atan")
	{
	  res =  std::make_shared<AtanOperator>();
	}
      else
	{
	  throw py::value_error("Unrecognized expression type");
	}
    }
  else if (tmp_type.is(expr_types.ExternalFunctionExpression) || tmp_type.is(expr_types.NPV_ExternalFunctionExpression))
    {
      res =  std::make_shared<ExternalOperator>();
    }
  else
    {
      throw py::value_error("Unrecognized expression type");
    }
  return res;
}


std::shared_ptr<ExpressionBase> appsi_expr_from_pyomo_expr(py::handle expr, py::dict var_map, py::dict param_map, PyomoExprTypes& expr_types)
{
  std::shared_ptr<Node> node;
  node = appsi_operator_from_pyomo_expr(expr, var_map, param_map, expr_types);
  if (node->is_operator_type())
    {
      std::vector<std::shared_ptr<Node> > oper_stack;
      std::vector<py::handle> expr_stack;
      expr_stack.push_back(expr);
      oper_stack.push_back(node);
  
      std::shared_ptr<Node> root = node;
  
      while (expr_stack.size() > 0)
  	{
  	  py::handle _expr = expr_stack.back();
  	  node = oper_stack.back();
  	  expr_stack.pop_back();
  	  oper_stack.pop_back();
  
  	  if (node->is_binary_operator())
  	    {
  	      std::shared_ptr<BinaryOperator> oper = std::dynamic_pointer_cast<BinaryOperator>(node);
  	      py::tuple expr_args = _expr.attr("args");
  	      oper->operand1 = appsi_operator_from_pyomo_expr(expr_args[0], var_map, param_map, expr_types);
  	      oper->operand2 = appsi_operator_from_pyomo_expr(expr_args[1], var_map, param_map, expr_types);
  	      if (oper->operand1->is_operator_type())
  		{
  		  expr_stack.push_back(expr_args[0]);
  		  oper_stack.push_back(oper->operand1);
  		}
  	      if (oper->operand2->is_operator_type())
  		{
  		  expr_stack.push_back(expr_args[1]);
  		  oper_stack.push_back(oper->operand2);
  		}
  	    }
  	  else if (node->is_unary_operator())
  	    {
  	      std::shared_ptr<UnaryOperator> oper = std::dynamic_pointer_cast<UnaryOperator>(node);
  	      py::tuple expr_args = _expr.attr("args");
  	      oper->operand = appsi_operator_from_pyomo_expr(expr_args[0], var_map, param_map, expr_types);
  	      if (oper->operand->is_operator_type())
  		{
  		  expr_stack.push_back(expr_args[0]);
  		  oper_stack.push_back(oper->operand);
  		}
  	    }
  	  else if (node->is_sum_operator())
  	    {
  	      std::shared_ptr<SumOperator> oper = std::dynamic_pointer_cast<SumOperator>(node);
  	      py::tuple expr_args = _expr.attr("args");
  	      std::shared_ptr<Node> _operand;
  	      for (py::handle arg : expr_args)
  		{
  		  _operand = appsi_operator_from_pyomo_expr(arg, var_map, param_map, expr_types);
  		  oper->operands->push_back(_operand);
  		  if (_operand->is_operator_type())
  		    {
  		      expr_stack.push_back(arg);
  		      oper_stack.push_back(_operand);
  		    }
  		}
  	    }
  	  else if (node->is_linear_operator())
  	    {
  	      std::shared_ptr<LinearOperator> oper = std::dynamic_pointer_cast<LinearOperator>(node);
  	      oper->constant = appsi_expr_from_pyomo_expr(_expr.attr("constant"), var_map, param_map, expr_types);
  	      py::list pyomo_vars = _expr.attr("linear_vars");
  	      py::list pyomo_coefs = _expr.attr("linear_coefs");
  	      for (py::handle v : pyomo_vars)
  	  	{
  	  	  oper->variables->push_back(var_map[expr_types.id(v)].cast<std::shared_ptr<Var> >());
  	  	}
  	      for (py::handle c : pyomo_coefs)
  	  	{
  	  	  oper->coefficients->push_back(appsi_expr_from_pyomo_expr(c, var_map, param_map, expr_types));
  	  	}
  	    }
  	  else if (node->is_external_operator())
  	    {
  	      std::shared_ptr<ExternalOperator> oper = std::dynamic_pointer_cast<ExternalOperator>(node);
  	      oper->function_name = _expr.attr("_fcn").attr("_function").cast<std::string>();
  	      py::tuple expr_args = _expr.attr("args");
  	      std::shared_ptr<Node> _operand;
  	      for (py::handle arg : expr_args)
  		{
  		  _operand = appsi_operator_from_pyomo_expr(arg, var_map, param_map, expr_types);
  		  oper->operands->push_back(_operand);
  		  if (_operand->is_operator_type())
  		    {
  		      expr_stack.push_back(arg);
  		      oper_stack.push_back(_operand);
  		    }
  		}	      
  	    }
  	  else
  	    {
  	      throw py::value_error("Unrecognized expression type");
  	    }
  	}
      return std::dynamic_pointer_cast<Operator>(root)->expression_from_operator();
    }
  else
    {
      return std::dynamic_pointer_cast<ExpressionBase>(node);
    }
}


std::vector<std::shared_ptr<ExpressionBase> > appsi_exprs_from_pyomo_exprs(py::list expr_list, py::dict var_map, py::dict param_map)
{
  std::vector<std::shared_ptr<ExpressionBase> > res;
  PyomoExprTypes expr_types = PyomoExprTypes();

  for (py::handle expr : expr_list)
    {
      res.push_back(appsi_expr_from_pyomo_expr(expr, var_map, param_map, expr_types));
    }
  return res;
}


/*
int main()
{
  std::shared_ptr<Param> a = std::make_shared<Param>();
  a->value = 2.0;
  std::shared_ptr<Var> x = std::make_shared<Var>();
  x->value = 3.0;
  std::shared_ptr<ExpressionBase> expr;
  double val;
  std::shared_ptr<Repn> repn;
  clock_t t0 = clock();
  expr = (*a)*(*x);
  for (int i=0; i<999; ++i)
    {
      expr = *expr + *((*a)*(*x));
    }
  clock_t t1 = clock();
  for (int i=0; i<1000; ++i)
    {
      val = expr->evaluate();
    }
  clock_t t2 = clock();
  std::shared_ptr<Expression> expr2 = std::dynamic_pointer_cast<Expression>(expr);
  for (int i=0; i<1000; ++i)
    {
      repn = expr2->generate_repn();
    }
  clock_t t3 = clock();
  std::cout << ((float)(t1-t0))/CLOCKS_PER_SEC << std::endl;
  std::cout << ((float)(t2-t1))/CLOCKS_PER_SEC << std::endl;
  std::cout << ((float)(t3-t2))/CLOCKS_PER_SEC << std::endl;
  return 0;
  }
*/
