#include "source.hpp"

namespace sb2cpp {

void Source::register_variable(std::string &name, RegisterType type) {
    auto name_lower = strtolower(name);
    VariableSymbol &var = this->variables[name_lower];
    // First name used for a variable is canonical
    if (var.cname == "") {
        var.cname = name;
    }
    else {
        name = var.cname;
    }
    var.used = var.used || (type == Use);
    var.defined = var.defined || (type == Define);
}

void Source::register_subroutine(std::string &name, AST::Subroutine *subroutine) {
    this->register_node(name, subroutine, this->subroutines, "Subroutine");
}
void Source::register_goto_label(std::string &name, AST::GotoLabel *label) {
    this->register_node(name, label, this->goto_labels, "Goto label");
}

void Source::visit_array_assign(AST::ArrayAssign *assign) {
    Source::register_variable(assign->variable, Define);
    AST::Visitor::visit_array_assign(assign);
}

void Source::visit_array_value(AST::ArrayValue *value) {
    Source::register_variable(value->variable, Use);
    AST::Visitor::visit_array_value(value);
}

void Source::visit_variable_value(AST::VariableValue *variable) {
    Source::register_variable(variable->variable, Use);
    AST::Visitor::visit_variable_value(variable);
}

void Source::visit_assign(AST::Assign *assign) {
    Source::register_variable(assign->variable, Define);
    AST::Visitor::visit_assign(assign);
}

void Source::visit_subroutine_call(AST::SubroutineCall *call) {
    Source::register_subroutine(call->subroutine_name, nullptr);
    AST::Visitor::visit_subroutine_call(call);
}

void Source::visit_subroutine(AST::Subroutine *subroutine) {
    Source::register_subroutine(subroutine->subroutine_name, subroutine);
    AST::Visitor::visit_subroutine(subroutine);
}

// FIXME: This needs to be updated after the standard library is implemented.
// It should be able to determine the expected argument type by querying the
// standard library.
void Source::visit_stdlib_assign(AST::StdlibAssign *assign) {
    auto rvalue_var = dynamic_cast<AST::VariableValue *>(assign->value);
    if (rvalue_var != nullptr) {
        // This may be the name of either a subroutine or a variable
        auto &unknown = this->unknown_identifiers;
        auto name_lower = strtolower(rvalue_var->variable);
        if (std::find(unknown.begin(), unknown.end(), name_lower) == unknown.end()) {
            unknown.push_back(name_lower);
        }
        return;
    }
    AST::Visitor::visit_stdlib_assign(assign);
}

void Source::visit_goto_label(AST::GotoLabel *label) {
    this->register_goto_label(label->name, label);
    AST::Visitor::visit_goto_label(label);
}

void Source::visit_goto_statement(AST::GotoStatement *goto_statement) {
    this->register_goto_label(goto_statement->label, nullptr);
    AST::Visitor::visit_goto_statement(goto_statement);
}

}