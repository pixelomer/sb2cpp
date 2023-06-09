#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <tuple>
#include <map>
#include <clocale>
#include <set>
#include <memory>
#include <algorithm>
#include <iomanip>

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

	// ASSIGNMENT
	std::unique_ptr<Node> assignment_to;
	std::unique_ptr<Node> assignment_value;

	// FOR_LOOP
	std::unique_ptr<Node> for_start;
	std::unique_ptr<Node> for_end;
	std::unique_ptr<Node> for_step;
	std::vector<std::unique_ptr<Node>> for_statements;

	// VALUE_LIST
	std::vector<std::unique_ptr<Node>> value_list;

	// WHILE_LOOP
	std::unique_ptr<Node> while_condition;
	std::vector<std::unique_ptr<Node>> while_statements;

	// COMPARE
	std::unique_ptr<Node> compare_left;
	std::wstring compare_operator;
	std::unique_ptr<Node> compare_right;

	// VARIABLE
	std::wstring variable_name;

	// ARRAY_VALUE
	std::wstring array_name;
	std::vector<std::unique_ptr<Node>> array_indexes;

	// FUNCTION_CALL
	std::wstring function_call;

	// STDLIB_CALL
	std::wstring stdlib_class;
	std::wstring stdlib_function;
	std::vector<std::unique_ptr<Node>> stdlib_arguments;

	// IF_ELSE
	std::unique_ptr<Node> if_condition;
	std::vector<std::unique_ptr<Node>> if_statements;

	// IF_ELSE_LIST
	std::vector<std::unique_ptr<Node>> if_list;

	// NUMBER_LITERAL
	long double number_literal;

	// STRING_LITERAL
	std::wstring string_literal;

	// OPERATOR
	bool operator_unary;
	std::wstring operator_str;

	// SUB
	std::wstring sub_name;
	std::vector<std::unique_ptr<Node>> sub_statements;

	// GOTO_LABEL
	std::wstring label_name;

	// GOTO
	std::wstring goto_name;
};

std::wstring &get_token(std::vector<std::wstring> &tokens, size_t index
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

const std::set<std::wstring> keywords{ { L"For", L"EndFor", L"To", L"Step",
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
		if (token2[0] == L'_') {
			throw std::runtime_error("refusing to call private stdlib method");
		}
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
		node.function_call = L"$" + token1;
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
		if (was_operator && isdigit((int)token1[0])) {
			(*index)++;
			was_operator = false;
			subnode = std::make_unique<Node>(Node());
			subnode->type = NUMBER_LITERAL;
			subnode->number_literal = std::stold(token1);
		}
		else if (was_operator && token1[0] == L'"') {
			(*index)++;
			if (token1.find(L'\n') != std::wstring::npos) {
				throw std::runtime_error("expected '\"', got newline");
			}
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
			subnode = std::make_unique<Node>(Node());
			subnode->type = OPERATOR;
			// ... +
			if (was_operator) {
				// Unary + and -
				if (token1 != L"-" && token1 != L"+") {
					throw std::runtime_error("expected '+' or '-'");
				}
				subnode->operator_unary = true;
			}
			else {
				subnode->operator_unary = false;
			}
			subnode->operator_str = ops[token1];
			was_operator = true;
			(*index)++;
		}
		else {
			// Variable?
			if (!was_operator) {
				// Reached the end
				if (node.value_list.size() == 1) {
					std::unique_ptr<Node> only_value = std::move(node.value_list[0]);
					return only_value;
				}
				else {
					return std::make_unique<Node>(std::move(node));
				}
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
		node.array_name = L"$" + name;
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
		node.variable_name = L"$" + name;
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
			else {
				node->for_step = std::make_unique<Node>();
				node->for_step->type = NUMBER_LITERAL;
				node->for_step->number_literal = 1.0L;
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
			node->sub_name = L"$" + sub_name;
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
						index--;
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
		else if (isdigit((int)c)) {
			// number
			bool parsed_dot = false;
			do { token += c; c = source[++i]; }
			while (isdigit((int)c) || (!parsed_dot && (parsed_dot = (c == L'.'))));
			i--;
		}
		else if (special.find(c) != std::wstring::npos) {
			// operators, etc.
			token += c;
		}
		else if (c == '"') {
			// strings
			do token += source[i];
			while ((i+1 < len) && (source[++i] != '"') && (source[i] != '\n'));
			token += '"';
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
		// Check if the token is a keyword. If it is, use the correct casing.
		auto result = std::find_if(keywords.cbegin(), keywords.cend(),
			[token](std::wstring const& keyword) {
				return std::equal(keyword.begin(), keyword.end(), token.begin(),
				token.end(), [](wchar_t a, wchar_t b) { 
					return std::towlower(a) == std::towlower(b); 
				});
			});
		if (result != keywords.cend()) {
			tokens.push_back(*result);
		}
		else {
			tokens.push_back(token);
		}
		token.clear();
	}
	return tokens;
}

void sb2cpp_multi(enum node_type, std::vector<std::unique_ptr<Node>> const& parsed,
	std::wstring const& delim, std::map<std::wstring, std::wstring> const& replacements,
	int indent = 0, bool root = false);

std::wstring sb2cpp_indent(int indent) {
	std::wstring str;
	for (int i=0; i<indent; i++) {
		str += L"  ";
	}
	return str;
}

std::wstring sb2cpp_escape(std::wstring const& str) {
	std::wstring result;
	size_t i_start = 0, i_end = 0;
	while (i_end != std::wstring::npos) {
		i_end = str.find(L'\\', i_end);
		if (i_end != std::wstring::npos) {
			result += str.substr(i_start, i_end - i_start) + L"\\\\";
			i_start = ++i_end;
		}
	}
	result += str.substr(i_start);
	return result;
}

std::wstring const& sb2cpp_replaced_var(std::wstring const& name,
	std::map<std::wstring, std::wstring> const& replacements)
{
	std::wstring lowercase = name;
	std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(),
		towlower);
	return replacements.at(lowercase);
}

void sb2cpp_single(enum node_type parent_type, std::unique_ptr<Node> const& node,
	std::map<std::wstring, std::wstring> const& replacements, int indent = 0,
	bool root = false)
{
	if (root) {
		std::wcout << sb2cpp_indent(indent);
	}
	switch (node->type) {
		case VALUE_LIST: {
			bool group = (parent_type == VALUE_LIST && node->value_list.size() > 1);
			if (group) std::wcout << "(";
			sb2cpp_multi(node->type, node->value_list, L"", replacements, indent);
			if (group) std::wcout << ")";
			break;
		}
		case VARIABLE: {
			std::wcout << sb2cpp_replaced_var(node->variable_name, replacements);
			break;
		}
		case ASSIGNMENT:
			sb2cpp_single(node->type, node->assignment_to, replacements, indent);
			std::wcout << " = ";
			sb2cpp_single(node->type, node->assignment_value, replacements, indent);
			if (root) std::wcout << ";";
			break;
		case STRING_LITERAL:
			if (parent_type != STDLIB_CALL) std::wcout << "Mixed(";
			std::wcout << "L" << sb2cpp_escape(node->string_literal);
			if (parent_type != STDLIB_CALL) std::wcout << ")";
			break;
		case NUMBER_LITERAL: {
			long double int_part;
			long double fraction_part = modfl(node->number_literal, &int_part);
			if (parent_type != STDLIB_CALL) std::wcout << "Mixed(";
			std::wcout << node->number_literal;
			if (fraction_part == 0.L) {
				std::wcout << ".";
			}
			std::wcout << "L";
			if (parent_type != STDLIB_CALL) std::wcout << ")";
			break;
		}
		case OPERATOR:
			if (!node->operator_unary) std::wcout << " ";
			std::wcout << node->operator_str;
			if (!node->operator_unary) std::wcout << " ";
			break;
		case IF_ELSE:
			if (node->if_condition != nullptr) {
				std::wcout << L"if (";
				sb2cpp_single(node->type, node->if_condition, replacements, indent);
				std::wcout << L") ";
			}
			std::wcout << L"{\n";
			sb2cpp_multi(node->type, node->if_statements, L"\n", replacements, indent+1,
				true);
			std::wcout << "\n" << sb2cpp_indent(indent) << "}";
			break;
		case IF_ELSE_LIST:
			sb2cpp_multi(node->type, node->if_list, L"\n" + sb2cpp_indent(indent) + L"else ",
				replacements, indent);
			break;
		case ARRAY_VALUE: {
			std::wcout << sb2cpp_replaced_var(node->array_name, replacements) << "[";
			sb2cpp_multi(node->type, node->array_indexes, L"][", replacements, indent);
			std::wcout << "]";
			break;
		}
		case STDLIB_CALL:
			std::wcout << node->stdlib_class << "::" << node->stdlib_function << "(";
			sb2cpp_multi(node->type, node->stdlib_arguments, L", ", replacements, indent);
			std::wcout << ")";
			if (root) std::wcout << ";";
			break;
		case WHILE_LOOP:
			std::wcout << "while (";
			sb2cpp_single(node->type, node->while_condition, replacements, indent);
			std::wcout << ") {\n";
			sb2cpp_multi(node->type, node->while_statements, L"\n", replacements,
				indent+1, true);
			std::wcout << "\n" << sb2cpp_indent(indent) << "}";
			break;
		case FUNCTION_CALL:
			std::wcout << sb2cpp_replaced_var(node->function_call, replacements) << "()";
			if (root) std::wcout << ";";
			break;
		case FOR_LOOP:
			std::wcout << "for (auto _ : ForLoop(&(";
			sb2cpp_single(node->type, node->for_start, replacements);
			std::wcout << "), ";
			sb2cpp_single(node->type, node->for_step, replacements);
			std::wcout << ", ";
			sb2cpp_single(node->type, node->for_end, replacements);
			std::wcout << ")) {\n";
			sb2cpp_multi(node->type, node->for_statements, L"\n", replacements, indent+1,
				true);
			std::wcout << "\n" << sb2cpp_indent(indent) << "}\n";
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

void sb2cpp_multi(enum node_type parent_type, std::vector<std::unique_ptr<Node>>
	const& parsed, std::wstring const& delim, std::map<std::wstring, std::wstring>
	const& replacements, int indent, bool root)
{
	for (int i=0; i<parsed.size(); i++) {
		auto const& node = parsed[i];
		sb2cpp_single(parent_type, node, replacements, indent, root);
		if ((i + 1) != parsed.size()) {
			std::wcout << delim;
		}
	}
}

void sb2cpp_decl_commit_single(std::wstring const& type,
	std::wstring const& suffix, std::wstring const& name, int *col)
{
	if (*col == 0) {
		std::wcout << type << " ";
		*col += type.length() + 1;
	}
	else {
		std::wcout << ", ";
		*col += 2;
	}
	if (*col + name.length() + suffix.length() >= 80) {
		std::wcout << "\n  ";
		*col = 2;
	}
	std::wcout << name << suffix;
	*col += name.length() + suffix.length();
}

void sb2cpp_decl_commit(std::map<std::wstring, bool> &defined) {
	//FIXME: could be much cleaner
	int col = 0;
	for (auto pair : defined) {
		if (pair.second) {
			sb2cpp_decl_commit_single(L"void", L"()", pair.first, &col);
		}
	}
	if (col != 0) {
		std::wcout << ";\n";
	}
	col = 0;
	for (auto pair : defined) {
		if (!pair.second) {
			sb2cpp_decl_commit_single(L"Mixed", L"", pair.first, &col);
		}
	}
	if (col != 0) {
		std::wcout << ";\n";
	}
}

void sb2cpp_decl_write(std::wstring const &str, bool is_function,
	std::map<std::wstring, bool> &defined,
	std::map<std::wstring, std::wstring> &replacements)
{
	const std::wstring *replacedStr = &str;
	std::wstring lowercase = str;
	std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(),
		towlower);
	if (replacements.count(lowercase) > 0) {
		replacedStr = &replacements[lowercase];
	}
	else {
		replacements[lowercase] = *replacedStr;
	}
	defined[*replacedStr] = defined[*replacedStr] || is_function;
}

void sb2cpp_impl(std::wstring const &name,
	std::vector<std::unique_ptr<Node>> const &statements,
	std::map<std::wstring, std::wstring> const &replacements)
{
	std::wcout << L"void " << name << "() {" << std::endl;
	sb2cpp_multi(SUB, statements, L"\n", replacements, 1, true);
	std::wcout << L"\n}" << std::endl;
}

void sb2cpp_decl_multi(std::vector<std::unique_ptr<Node>> const &nodes,
	std::map<std::wstring, bool> &defined, std::map<std::wstring, std::wstring>
	&replacements);

void sb2cpp_decl_single(std::unique_ptr<Node> const &node,
	std::map<std::wstring, bool> &defined,
	std::map<std::wstring, std::wstring> &replacements)
{
	if (node == nullptr) return;
	switch (node->type) {
		case ARRAY_VALUE:
			sb2cpp_decl_write(node->array_name, false, defined, replacements);
			sb2cpp_decl_multi(node->array_indexes, defined, replacements);
			break;
		case STDLIB_CALL:
			sb2cpp_decl_multi(node->stdlib_arguments, defined, replacements);
			break;
		case ASSIGNMENT:
			sb2cpp_decl_single(node->assignment_value, defined, replacements);
			sb2cpp_decl_single(node->assignment_to, defined, replacements);
			break;
		case IF_ELSE_LIST:
			sb2cpp_decl_multi(node->if_list, defined, replacements);
			break;
		case IF_ELSE:
			sb2cpp_decl_multi(node->if_statements, defined, replacements);
			sb2cpp_decl_single(node->if_condition, defined, replacements);
			break;
		case SUB:
			sb2cpp_decl_write(node->sub_name, true, defined, replacements);
			sb2cpp_decl_multi(node->sub_statements, defined, replacements);
			break;
		case VARIABLE:
			sb2cpp_decl_write(node->variable_name, false, defined, replacements);
			break;
		case WHILE_LOOP:
			sb2cpp_decl_single(node->while_condition, defined, replacements);
			sb2cpp_decl_multi(node->while_statements, defined, replacements);
			break;
		case VALUE_LIST:
			sb2cpp_decl_multi(node->value_list, defined, replacements);
			break;
		case FUNCTION_CALL:
			sb2cpp_decl_write(node->function_call, true, defined, replacements);
			break;
		case FOR_LOOP:
			sb2cpp_decl_single(node->for_start, defined, replacements);
			sb2cpp_decl_single(node->for_end, defined, replacements);
			sb2cpp_decl_single(node->for_step, defined, replacements);
			sb2cpp_decl_multi(node->for_statements, defined, replacements);
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
	std::map<std::wstring, bool> &defined, std::map<std::wstring, std::wstring>
	&replacements)
{
	for (auto const& node : nodes) {
		sb2cpp_decl_single(node, defined, replacements);
	}
}

std::map<std::wstring, std::wstring> sb2cpp_vars(std::vector<std::unique_ptr<Node>>
	const &nodes)
{
	std::map<std::wstring, bool> defined;
	std::map<std::wstring, std::wstring> replacements;
	sb2cpp_decl_multi(nodes, defined, replacements);
	sb2cpp_decl_commit(defined);
	return replacements;
}

void sb2cpp(std::wstring const& source) {
	auto tokens = tokenize(source);
	auto parsed = parse(tokens);

	time_t epoch = time(NULL);
	struct tm *time = gmtime(&epoch);

	// Version header
	std::wcout << "// sb2cpp by pixelomer";
#ifdef GIT_COMMIT
	std::wcout << " (commit " GIT_COMMIT ")";
#endif
	std::wcout << std::endl;

	// Date
	std::wcout << "// Generated at " << std::put_time(time, L"%B %e %Y %H:%M:%S UTC")
		<< std::endl;
	std::wcout << std::endl;

	// Includes
	std::wcout << L"#include <std/SmallBasic.hpp>" << std::endl;
	std::wcout << L"using namespace SmallBasic::Std;" << std::endl;
	std::wcout << L"using SmallBasic::Mixed;" << std::endl;
	std::wcout << std::endl;

	// Variables and functions
	std::wcout << L"std::stack<Mixed> end;" << std::endl;
	std::map<std::wstring, std::wstring> replacements = sb2cpp_vars(parsed);
	std::wcout << std::endl;

	// User defined functions
	for (auto iter = parsed.end() - 1; iter >= parsed.begin(); iter--) {
		auto const& node = *iter;
		if (node->type == SUB) {
			sb2cpp_impl(sb2cpp_replaced_var(node->sub_name, replacements),
				node->sub_statements, replacements);
			parsed.erase(iter);
		}
	}

	// SmallBasic main
	sb2cpp_impl(L"SmallBasic_Main", parsed, replacements);
	std::wcout << std::endl;

	// Program main
	std::wcout << L"int main(int argc, char **argv) {" << std::endl;
	std::wcout << L"#ifdef SMALLBASIC_PACKED_ASSETS" << std::endl;
	std::wcout << sb2cpp_indent(1) << L"SmallBasic_RegisterPackedAssets();"
		<< std::endl;
	std::wcout << L"#endif" << std::endl;
	std::wcout << sb2cpp_indent(1) << L"Program::_Run(argc, argv, SmallBasic_Main);"
		<< std::endl;
	std::wcout << L"}";
}

int main(int argc, char **argv) {
	std::setlocale(LC_ALL, "C");
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