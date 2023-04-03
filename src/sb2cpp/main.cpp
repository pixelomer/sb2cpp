#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <map>
#include <set>

#ifdef __INTELLISENSE__
// function "Node::~Node()" (declared implicitly) cannot be referenced --
// it is a deleted function
#pragma diag_suppress 1776
#endif

enum node_type {
	ASSIGNMENT,     // VARIABLE | ARRAY_VALUE = VALUE_LIST
	FOR_LOOP,       // For VALUE_LIST To VALUE_LIST Step VALUE_LIST ... EndFor
	VALUE_LIST,     // VARIABLE + ARRAY_VALUE / (FUNCTION_CALL - STDLIB_CALL) ...
	WHILE_LOOP,     // While CONDITION_LIST ... EndWhile
	VARIABLE,       // a
	ARRAY_VALUE,    // a[VALUE_LIST]
	SUB,            // Sub MyFunction ... EndSub
	FUNCTION_CALL,  // MyFunction()
	STDLIB_CALL,    // Text.Append(VALUE_LIST, VALUE_LIST)
	IF_ELSE_LIST,   // Contains all of IF_ELSE
	IF_ELSE,        // If ... ElseIf ... Else
	NUMBER_LITERAL, // 10.0
	STRING_LITERAL, // "Hello"
	OPERATOR,       // +
	GOTO_LABEL,     // label:
	GOTO            // Goto label
};

class Node;

//FIXME: wastes a lot of memory
class Node {
public:
	Node(){}
	enum node_type type;
	struct {
		struct {
			// ASSIGNMENT
			std::unique_ptr<Node> assignment_to;
			std::unique_ptr<Node> assignment_value;
		};
		struct {
			// FOR_LOOP
			std::unique_ptr<Node> for_start;
			std::unique_ptr<Node> for_end;
			std::unique_ptr<Node> for_step;
			std::vector<std::unique_ptr<Node>> for_statements;
		};
		struct {
			// VALUE_LIST
			std::vector<std::unique_ptr<Node>> value_list;
		};
		struct {
			// WHILE_LOOP
			std::unique_ptr<Node> while_condition;
			std::vector<std::unique_ptr<Node>> while_statements;
		};
		struct {
			// COMPARE
			std::unique_ptr<Node> compare_left;
			std::wstring compare_operator;
			std::unique_ptr<Node> compare_right;
		};
		struct {
			// VARIABLE
			std::wstring variable_name;
		};
		struct {
			// ARRAY_VALUE
			std::wstring array_name;
			std::vector<std::unique_ptr<Node>> array_indexes;
		};
		struct {
			// FUNCTION_CALL
			std::wstring function_call;
		};
		struct {
			// STDLIB_CALL
			std::wstring stdlib_class;
			std::wstring stdlib_function;
			std::vector<std::unique_ptr<Node>> stdlib_arguments;
		};
		struct {
			// IF_ELSE
			std::unique_ptr<Node> if_condition;
			std::vector<std::unique_ptr<Node>> if_statements;
		};
		struct {
			// IF_ELSE_LIST
			std::vector<std::unique_ptr<Node>> if_list;
		};
		struct {
			// NUMBER_LITERAL
			long double number_literal;
		};
		struct {
			// STRING_LITERAL
			std::wstring string_literal;
		};
		struct {
			// OPERATOR
			std::wstring operator_str;
		};
		struct {
			// SUB
			std::wstring sub_name;
			std::vector<std::unique_ptr<Node>> sub_statements;
		};
		struct {
			// GOTO_LABEL
			std::wstring label_name;
		};
		struct {
			// GOTO
			std::wstring goto_name;
		};
	};
};

inline std::wstring &get_token(std::vector<std::wstring> &tokens, size_t index
#ifdef DEBUG
	, const char *func, const char *file, int line
#endif
)
{
	static std::wstring empty = L"";
	std::wstring &token = (index < tokens.size()) ? tokens[index] : empty;
#ifdef DEBUG
	std::wcerr << file << ":" << line << " (" << func << ") token read -> " << token << std::endl;
#endif
	return token;
}

#ifdef DEBUG
#define get_token(tokens, index) get_token(tokens, index, __FUNCTION__, __FILE__, __LINE__)
#endif

const std::set<const std::wstring> keywords{ { L"For", L"EndFor", L"To", L"Step",
	L"If", L"Then", L"Else", L"ElseIf", L"EndIf", L"Goto", L"Sub", L"EndSub",
	L"While", L"EndWhile", L"And", L"Or" } };

#define REGULAR_OPS { L"-", L"-" }, { L"+", L"+" }, { L"/", L"/" }, \
	{ L"*", L"*" }
#define COMPARE_OPS { L"<", L"<" }, { L">", L">" }, { L"<>", L"!=" }, \
	{ L"Or", L"||" }, { L"And", L"&&" }, { L">=", L">=" }, { L"<=", L"<=" }, \
	{ L"=", L"==" }

std::map<const std::wstring, const std::wstring> nologic_ops{ REGULAR_OPS };
std::map<const std::wstring, const std::wstring> all_ops{ REGULAR_OPS,
	COMPARE_OPS };

std::unique_ptr<Node> parse_variable(std::vector<std::wstring> &tokens, size_t *index);
std::unique_ptr<Node> parse_value(std::vector<std::wstring> &tokens, size_t *index,
	std::map<const std::wstring, const std::wstring> &ops = nologic_ops);

std::unique_ptr<Node> parse_call(std::vector<std::wstring> &tokens, size_t *index,
	bool no_set = false)
{
	Node node;
	std::wstring &token1 = get_token(tokens, (*index)++);
	std::wstring &token2 = get_token(tokens, (*index)++);
	if (token2 == L".") {
		token2 = get_token(tokens, (*index)++);
		node.type = STDLIB_CALL;
		node.stdlib_class = token1;
		node.stdlib_function = token2;
		token2 = get_token(tokens, (*index)++);
		if (token2 != L"(") {
			if (!no_set && token2 == L"=") {
				// Class.Value = ...
				auto value = parse_value(tokens, index);
				node.stdlib_function = L"_Set" + node.stdlib_function;
				node.stdlib_arguments.push_back(std::move(value));
			}
			else {
				// Class.Value
				(*index)--;
				node.stdlib_function = L"_Get" + node.stdlib_function;
			}
		}
		else {
			// Class.Function(...)
			while (true) {
				if (get_token(tokens, (*index)) == L")") {
					break;
				}
				auto value = parse_value(tokens, index);
				node.stdlib_arguments.push_back(std::move(value));
				if (get_token(tokens, (*index)) == L",") {
					(*index)++;
					if (get_token(tokens, (*index)) == L")") {
						throw std::runtime_error("expected value, got ')' instead");
					}
				}
			}
			(*index)++;
		}
	}
	else if (token2 == L"(") {
		node.type = FUNCTION_CALL;
		node.function_call = L"_" + token1;
		token2 = get_token(tokens, (*index)++);
		if (token2 != L")") {
			throw std::runtime_error("expected ')'");
		}
	}
	else {
		throw std::runtime_error("expected function call");
	}
	return std::make_unique<Node>(std::move(node));
}

std::unique_ptr<Node> parse_assignment(std::vector<std::wstring> &tokens, size_t *index) {
	Node node;
	auto variable = parse_variable(tokens, index);
	if (get_token(tokens, *index) != L"=") {
		throw std::runtime_error("expected '='");
	}
	(*index)++;
	auto value = parse_value(tokens, index);
	node.type = ASSIGNMENT;
	node.assignment_to = std::move(variable);
	node.assignment_value = std::move(value);
	return std::make_unique<Node>(std::move(node));
}

std::unique_ptr<Node> parse_value(std::vector<std::wstring> &tokens, size_t *index,
	std::map<const std::wstring, const std::wstring> &ops)
{
	Node node;
	node.type = VALUE_LIST;
	bool was_operator = true;
	while (true) {
		std::wstring &token1 = get_token(tokens, (*index));
		std::wstring &token2 = get_token(tokens, (*index)+1);
		std::unique_ptr<Node> subnode;
		if (was_operator && isnumber((int)token1[0])) {
			(*index)++;
			was_operator = false;
			subnode = std::make_unique<Node>(Node());
			subnode->type = NUMBER_LITERAL;
			subnode->number_literal = std::stold(token1);
		}
		else if (was_operator && token1[0] == L'"') {
			(*index)++;
			was_operator = false;
			subnode = std::make_unique<Node>(Node());
			subnode->type = STRING_LITERAL;
			subnode->string_literal = token1;
		}
		else if (was_operator && token1 == L"(") {
			// ... + (a + b + c)
			(*index)++;
			was_operator = false;
			subnode = parse_value(tokens, index, ops);
			if (get_token(tokens, *index) != L")") {
				throw std::runtime_error("expected ')'");
			}
			(*index)++;
		}
		else if (was_operator && token2 == L"." /* || token2 == L"(" */) {
			// ... + Math.Random()
			was_operator = false;
			subnode = parse_call(tokens, index, true);
		}
		else if (ops.count(token1) != 0) {
			// ... +
			if (was_operator) {
				// Unary + and -
				if (token1 != L"-" && token1 != L"+") {
					throw std::runtime_error("expected '+' or '-'");
				}
			}
			was_operator = true;
			subnode = std::make_unique<Node>(Node());
			subnode->type = OPERATOR;
			subnode->operator_str = ops[token1];
			(*index)++;
		}
		else {
			// Variable?
			if (!was_operator) {
				// Reached the end
				return std::make_unique<Node>(std::move(node));
			}
			else {
				subnode = parse_variable(tokens, index);
				was_operator = false;
			}
		}
		node.value_list.push_back(std::move(subnode));
	}
}

std::unique_ptr<Node> parse_variable(std::vector<std::wstring> &tokens, size_t *index) {
	Node node;
	std::wstring &name = get_token(tokens, *index);
	if (keywords.find(name) != keywords.end()) {
		throw std::runtime_error("keyword used as variable name");
	}
	std::wstring &token = get_token(tokens, ++(*index));
	if (token == L"[") {
		node.type = ARRAY_VALUE;
		node.array_name = L"_" + name;
		while (token == L"[") {
			(*index)++;
			std::unique_ptr<Node> key = parse_value(tokens, index);
			token = get_token(tokens, *index);
			if (token != L"]") {
				throw std::runtime_error("expected ']'");
			}
			node.array_indexes.push_back(std::move(key));
			token = get_token(tokens, ++(*index));
		}
	}
	else {
		node.type = VARIABLE;
		node.variable_name = L"_" + name;
	}
	return std::make_unique<Node>(std::move(node));
}

std::vector<std::unique_ptr<Node>> parse(std::vector<std::wstring> &tokens,
	size_t *pIndex = nullptr, const std::set<std::wstring> &end = {})
{
	std::vector<std::unique_ptr<Node>> nodes;
	size_t index = 0;
	if (pIndex != NULL) index = *pIndex;

	while (index < tokens.size()) {
		std::wstring &token1 = get_token(tokens, index);
		std::unique_ptr<Node> node;
		if (end.count(token1) != 0) {
			break;
		}
		if (token1 == L"For") {
			// For (from) To (to) [Step (step)]
			// For var = (from) To (to) [Step (step)]
			// EndFor
			node = std::make_unique<Node>(Node());
			node->type = FOR_LOOP;
			index++;
			std::unique_ptr<Node> for_assignment;
			try {
				size_t _index = index;
				for_assignment = std::move(parse_assignment(tokens, &_index));
				index = _index;
			}
			catch (...) {
				auto i_variable = std::make_unique<Node>();
				i_variable->type = VARIABLE;
				i_variable->variable_name = L"i";
				auto value = parse_value(tokens, &index);
				for_assignment = std::make_unique<Node>();
				for_assignment->type = ASSIGNMENT;
				for_assignment->assignment_to = std::move(i_variable);
				for_assignment->assignment_value = std::move(value);
			}
			if (get_token(tokens, index++) != L"To") {
				throw std::runtime_error("expected 'To'");
			}
			auto to_value = parse_value(tokens, &index);
			node->for_start = std::move(for_assignment);
			node->for_end = std::move(to_value);
			if (get_token(tokens, index) == L"Step") {
				index++;
				auto step_value = parse_value(tokens, &index);
				node->for_step = std::move(step_value);
			}
			auto statements = parse(tokens, &index, { L"EndFor" });
			if (get_token(tokens, index++) != L"EndFor") {
				throw std::runtime_error("expected 'EndFor'");
			}
			node->for_statements = std::move(statements);
		}
		else if (token1 == L"Sub") {
			index++;
			auto &sub_name = get_token(tokens, index++);
			auto statements = parse(tokens, &index, { L"EndSub" });
			if (get_token(tokens, index++) != L"EndSub") {
				throw std::runtime_error("expected 'EndSub'");
			}
			node = std::make_unique<Node>(Node());
			node->sub_name = L"_" + sub_name;
			node->type = SUB;
			node->sub_statements = std::move(statements);
		}
		else if (token1 == L"If") {
			index++;
			auto end_tokens = std::set<std::wstring>{ L"EndIf", L"Else", L"ElseIf" };
			node = std::make_unique<Node>(Node());
			node->type = IF_ELSE_LIST;
			do {
				std::unique_ptr<Node> new_node;
				new_node = std::make_unique<Node>(Node());
				new_node->type = IF_ELSE;
				if (token1 == L"Else") {
					end_tokens = {{ L"EndIf" }};
				}
				else {
					auto condition = parse_value(tokens, &index, all_ops);
					if (get_token(tokens, index++) != L"Then") {
						throw std::runtime_error("expected 'Then'");
					}
					new_node->if_condition = std::move(condition);
				}
				new_node->if_statements = parse(tokens, &index, end_tokens);
				token1 = get_token(tokens, index++);
				node->if_list.push_back(std::move(new_node));
			}
			while (token1 != L"EndIf" && end_tokens.count(token1) != 0);
			if (token1 != L"EndIf") {
				throw std::runtime_error("Expected 'EndIf'");
			}
		}
		else if (token1 == L"Goto") {
			node = std::make_unique<Node>();
			node->type = GOTO;
			node->goto_name = get_token(tokens, ++index);
			index++;
		}
		else if (token1 == L"While") {
			node = std::make_unique<Node>();
			index++;
			auto condition = parse_value(tokens, &index, all_ops);
			auto statements = parse(tokens, &index, { L"EndWhile" });
			token1 = get_token(tokens, index);
			if (token1 != L"EndWhile") {
				throw std::runtime_error("Expected 'EndWhile'");
			}
			index++;
			node->type = WHILE_LOOP;
			node->while_condition = std::move(condition);
			node->while_statements = std::move(statements);
		}
		else {
			// Assignment, function call or label
			std::wstring &token2 = get_token(tokens, index+1);
			if (token2 == L":") {
				// Label
				node = std::make_unique<Node>();
				node->type = GOTO_LABEL;
				node->label_name = token1;
				index += 2;
			}
			else if (token2 == L"(" || token2 == L".") {
				// Function call
				node = parse_call(tokens, &index);
			}
			else if (token2 == L"=" || token2 == L"[") {
				// Assignment
				node = parse_assignment(tokens, &index);
			}
			else {
				throw std::runtime_error("expected assignment");
			}
		}
		if (node != nullptr) {
			nodes.push_back(std::move(node));
		}
	}

	if (pIndex != NULL) *pIndex = index;
	return nodes;
}

std::vector<std::wstring> tokenize(std::wstring const& source) {
	std::vector<std::wstring> tokens;
	std::wstring token;
	const std::wstring special = L"()[]<>+-/*=.',:";
	size_t len = source.length();
	for (int i=0; i<len; i++) {
		wchar_t c = source[i];
		if (iswspace(c)) {
			// whitespace
			continue;
		}
		else if (c == '\'') {
			// comments
			while (i+1 < len && source[++i] != '\n');
			continue;
		}
		else if (c == '>' || c == '<') {
			// >, <, >=, <=, <>
			token += c;
			if (i+1 < len) {
				wchar_t nextc = source[i+1];
				if ((c == '<' && nextc == '>') || nextc == '=') {
					token += nextc;
					i++;
				}
			}
		}
		else if (isnumber((int)c)) {
			// number
			bool parsed_dot = false;
			do { token += c; c = source[++i]; }
			while (isnumber((int)c) || (!parsed_dot && (parsed_dot = (c == L'.'))));
			i--;
		}
		else if (special.find(c) != std::wstring::npos) {
			// operators, etc.
			token += c;
		}
		else if (c == '"') {
			// strings
			bool backslash = false;
			do {
				token += source[i];
				backslash = source[i] == '\\';
			}
			while (i+1 < len && (source[++i] != '"' || backslash));
			token += source[i];
			if (source[i] != L'"') {
				throw std::runtime_error("expected '\"'");
			}
		}
		else {
			// variable names, keywords, etc.
			do {
				token += source[i++];
			}
			while (i < len && (special.find(source[i]) == std::wstring::npos)
				&& !iswspace(source[i]));
			i--;
		}
		tokens.push_back(token);
		token.clear();
	}
	return tokens;
}

void sb2cpp_multi(std::vector<std::unique_ptr<Node>> const& parsed, std::wstring
	const& delim, int indent = 0, bool root = false);

std::wstring sb2cpp_indent(int indent) {
	std::wstring str;
	for (int i=0; i<indent; i++) {
		str += L"  ";
	}
	return str;
}

void sb2cpp_single(std::unique_ptr<Node> const& node, int indent = 0, bool root
	= false)
{
	if (root) {
		std::wcout << sb2cpp_indent(indent);
	}
	switch (node->type) {
		case VALUE_LIST:
			if (node->value_list.size() > 1) std::wcout << "(";
			sb2cpp_multi(node->value_list, L" ", indent);
			if (node->value_list.size() > 1) std::wcout << ")";
			break;
		case VARIABLE:
			std::wcout << node->variable_name;
			break;
		case ASSIGNMENT:
			sb2cpp_single(node->assignment_to, indent);
			std::wcout << " = ";
			sb2cpp_single(node->assignment_value, indent);
			if (root) std::wcout << ";";
			break;
		case STRING_LITERAL:
			std::wcout << "Mixed(L" << node->string_literal << ")";
			break;
		case NUMBER_LITERAL:
			std::wcout << "Mixed((Number)" << node->number_literal << ")";
			break;
		case OPERATOR:
			std::wcout << node->operator_str;
			break;
		case IF_ELSE:
			if (node->if_condition != nullptr) {
				std::wcout << L"if (";
				sb2cpp_single(node->if_condition, indent);
				std::wcout << L") ";
			}
			std::wcout << L"{\n";
			sb2cpp_multi(node->if_statements, L"\n", indent+1, true);
			std::wcout << "\n" << sb2cpp_indent(indent) << "}";
			break;
		case IF_ELSE_LIST:
			sb2cpp_multi(node->if_list, L"\n" + sb2cpp_indent(indent) + L"else ", indent);
			break;
		case ARRAY_VALUE:
			std::wcout << node->array_name << "[";
			sb2cpp_multi(node->array_indexes, L"][", indent);
			std::wcout << "]";
			break;
		case STDLIB_CALL:
			std::wcout << node->stdlib_class << "::" << node->stdlib_function << "(";
			sb2cpp_multi(node->stdlib_arguments, L", ", indent);
			std::wcout << ")";
			if (root) std::wcout << ";";
			break;
		case WHILE_LOOP:
			std::wcout << "while (";
			sb2cpp_single(node->while_condition, indent);
			std::wcout << ") {\n";
			sb2cpp_multi(node->while_statements, L"\n", indent+1, true);
			std::wcout << "\n" << sb2cpp_indent(indent) << "}";
			break;
		case FUNCTION_CALL:
			std::wcout << node->function_call << "()";
			if (root) std::wcout << ";";
			break;
		case FOR_LOOP:
			std::wcout << "for (Mixed ";
			sb2cpp_single(node->for_start, indent);
			std::wcout << ", end = ";
			sb2cpp_single(node->for_end, indent);
			std::wcout << "; ";
			sb2cpp_single(node->for_start->assignment_to, indent);
			std::wcout << " <= end; ";
			sb2cpp_single(node->for_start->assignment_to, indent);
			std::wcout << " = ";
			sb2cpp_single(node->for_start->assignment_to, indent);
			std::wcout << " + ";
			if (node->for_step == nullptr) {
				std::wcout << "Mixed(1.L)";
			}
			else {
				sb2cpp_single(node->for_step, indent);
			}
			std::wcout << ") {\n";
			sb2cpp_multi(node->for_statements, L"\n", indent+1, true);
			std::wcout << "\n" << sb2cpp_indent(indent) << "}";
			break;
		case GOTO_LABEL:
			std::wcout << node->label_name << ":;";
			break;
		case GOTO:
			std::wcout << "goto " << node->goto_name << ";";
			break;
		case SUB:
			throw std::runtime_error("not implemented");
	}
}

void sb2cpp_multi(std::vector<std::unique_ptr<Node>> const& parsed, std::wstring
	const& delim, int indent, bool root)
{
	for (int i=0; i<parsed.size(); i++) {
		auto const& node = parsed[i];
		sb2cpp_single(node, indent, root);
		if ((i + 1) != parsed.size()) {
			std::wcout << delim;
		}
	}
}

inline void sb2cpp_decl_write(std::wstring const &str,
	std::set<std::wstring> &defined)
{
	if (defined.count(str) == 0) {
		defined.insert(str);
		std::wcout << str << L";" << std::endl;
	}
}

void sb2cpp_impl(std::wstring const &name,
	std::vector<std::unique_ptr<Node>> const &statements)
{
	std::wcout << L"void " << name << "() {" << std::endl;
	sb2cpp_multi(statements, L"\n", 1, true);
	std::wcout << L"\n}" << std::endl;
}

void sb2cpp_decl_multi(std::vector<std::unique_ptr<Node>> const &nodes,
	std::set<std::wstring> &defined);

void sb2cpp_decl_single(std::unique_ptr<Node> const &node,
	std::set<std::wstring> &defined)
{
	if (node == nullptr) return;
	switch (node->type) {
		case ARRAY_VALUE:
			sb2cpp_decl_write(L"Mixed " + node->array_name, defined);
			sb2cpp_decl_multi(node->array_indexes, defined);
			break;
		case STDLIB_CALL:
			sb2cpp_decl_multi(node->stdlib_arguments, defined);
			break;
		case ASSIGNMENT:
			sb2cpp_decl_single(node->assignment_value, defined);
			sb2cpp_decl_single(node->assignment_to, defined);
			break;
		case IF_ELSE_LIST:
			sb2cpp_decl_multi(node->if_list, defined);
			break;
		case IF_ELSE:
			sb2cpp_decl_multi(node->if_statements, defined);
			sb2cpp_decl_single(node->if_condition, defined);
			break;
		case SUB:
			sb2cpp_decl_multi(node->sub_statements, defined);
			break;
		case VARIABLE:
			sb2cpp_decl_write(L"Mixed " + node->variable_name, defined);
			break;
		case WHILE_LOOP:
			sb2cpp_decl_single(node->while_condition, defined);
			sb2cpp_decl_multi(node->while_statements, defined);
			break;
		case VALUE_LIST:
			sb2cpp_decl_multi(node->value_list, defined);
			break;
		case FUNCTION_CALL:
			sb2cpp_decl_write(L"void " + node->function_call + L"()", defined);
			break;
		case FOR_LOOP:
			sb2cpp_decl_single(node->for_start, defined);
			sb2cpp_decl_single(node->for_end, defined);
			sb2cpp_decl_single(node->for_step, defined);
			sb2cpp_decl_multi(node->for_statements, defined);
			break;
		case NUMBER_LITERAL:
		case STRING_LITERAL:
		case GOTO_LABEL:
		case GOTO:
		case OPERATOR:
			break;
	}
}

void sb2cpp_decl_multi(std::vector<std::unique_ptr<Node>> const &nodes,
	std::set<std::wstring> &defined)
{
	for (auto const& node : nodes) {
		sb2cpp_decl_single(node, defined);
	}
}

void sb2cpp_vars(std::vector<std::unique_ptr<Node>> const &nodes) {
	auto defined = std::set<std::wstring>{};
	sb2cpp_decl_multi(nodes, defined);
}

void sb2cpp(std::wstring const& source) {
	auto tokens = tokenize(source);
	auto parsed = parse(tokens);

	std::wcout << L"#include \"src/std/SmallBasic.hpp\"" << std::endl;
	std::wcout << L"#include <random>" << std::endl;
	std::wcout << L"using namespace SmallBasic;" << std::endl;
	std::wcout << L"" << std::endl;

	sb2cpp_vars(parsed);

	std::wcout << L"" << std::endl;

	for (auto iter = parsed.end() - 1; iter >= parsed.begin(); iter--) {
		auto const& node = *iter;
		if (node->type == SUB) {
			sb2cpp_impl(node->sub_name, node->sub_statements);
			parsed.erase(iter);
		}
	}

	sb2cpp_impl(L"SmallBasic_Main", parsed);

	std::wcout << L"" << std::endl;
	std::wcout << L"int main(int argc, char **argv) {" << std::endl;
	std::wcout << L"  srandom(time(NULL));" << std::endl;
	std::wcout << L"  SmallBasic_Main();" << std::endl;
	std::wcout << L"}" << std::endl;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " file.sb" << std::endl;
		return EXIT_FAILURE;
	}
	std::wstring source;
	{
		std::wifstream file;
		file.open(argv[1]);
		std::wstringstream source_stream;
		source_stream << file.rdbuf();
		source = source_stream.str();
		file.close();
	}
	sb2cpp(source);
}