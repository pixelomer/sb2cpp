#include "source.hpp"
#include "util.hpp"
#include <iostream>

namespace sb2cpp {

void Source::register_variable(std::string const& name, RegisterType type) {
    auto name_lower = strtolower(name);
    Variable &var = this->variables[name_lower];
    if (var.name == "") {
        // First name used for a variable is canonical
        var.name = name;
    }
    var.used = var.used || (type == Use);
    var.defined = var.defined || (type == Define);
}

void Source::register_goto_label(AST::GotoLabel *label) {
    auto name_lower = strtolower(label->name);
    AST::GotoLabel *&label_in_array = this->goto_labels[name_lower];
    if (label_in_array != nullptr) {
        throw SourceError("Goto label '" + name_lower + "' declared "
            "multiple times");
    }
    label_in_array = label;
}

void Source::register_subroutine(AST::Subroutine *subroutine) {
    auto name_lower = strtolower(subroutine->subroutine_name);
    Subroutine &sub = this->subroutines[name_lower];
    if (sub.subroutine != nullptr) {
        throw SourceError("Subroutine '" + name_lower +
            "' declared multiple times");
    }
    sub.subroutine = subroutine;
    sub.defined = true;
}

void Source::register_call(std::string const& name) {
    auto name_lower = strtolower(name);
    Subroutine &sub = this->subroutines[name_lower];
    sub.used = true;
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
    Source::register_call(call->subroutine_name);
    AST::Visitor::visit_subroutine_call(call);
}

void Source::visit_subroutine(AST::Subroutine *subroutine) {
    Source::register_subroutine(subroutine);
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
    this->register_goto_label(label);
    AST::Visitor::visit_goto_label(label);
}

}