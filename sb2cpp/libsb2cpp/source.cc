#include "source.hpp"
#include "../stdlib/Runtime.hpp"
#include "../stdlib/Runtime_generated.hpp"

namespace sb2cpp {

bool Source::did_register_builtin = false;

void Source::register_builtin() {
    if (!did_register_builtin) {
        did_register_builtin = true;
        SmallBasic::register_builtin();
    }
}

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

void Source::visit_stdlib_call(AST::StdlibCall *call) {
    auto &cls = SmallBasic::Runtime::get_class(call->class_name);
    auto &method = cls.get_method(call->method_name);
    if (call->returns_value && !method.returns_value) {
        throw SourceError("Does not return value: '" + cls.cname + "." +
            method.cname + "()");
    }
    if (call->arguments.size() != method.argc) {
        throw SourceError("Argument count mismatch (" + std::to_string(method.argc) +
            " != " + std::to_string(call->arguments.size()) + "): " + cls.cname +
            "." + method.cname + "()");
    }
    call->class_name = cls.cname;
    call->method_name = method.cname;
    call->cls = &cls;
    call->method = &method;
    AST::Visitor::visit_stdlib_call(call);
}

void Source::visit_stdlib_assign(AST::StdlibAssign *assign) {
    auto &cls = SmallBasic::Runtime::get_class(assign->class_name);
    auto &property = cls.get_property(assign->property_name);

    assign->class_name = cls.cname;
    assign->property_name = property.cname;
    assign->cls = &cls;
    assign->property = &property;

    auto rvalue_var = dynamic_cast<AST::VariableValue *>(assign->value);
    if (rvalue_var != nullptr) {
        if (property.value_setter != nullptr) {
            this->register_variable(rvalue_var->variable, Use);
        }
        else if (property.callback_setter != nullptr) {
            this->register_subroutine(rvalue_var->variable, nullptr);
            rvalue_var->is_subroutine = true;
        }
        else {
            throw SourceError("Cannot assign variable to read-only property " +
                cls.cname + "." + property.cname);
        }
    }
    else {
        if (property.value_setter == nullptr) {
            throw SourceError("Cannot assign value to read-only property " +
                cls.cname + "." + property.cname);
        }
        AST::Visitor::visit_stdlib_assign(assign);
    }
}

void Source::visit_stdlib_value(AST::StdlibValue *value) {
    auto &cls = SmallBasic::Runtime::get_class(value->class_name);
    auto &property = cls.get_property(value->property_name);

    value->class_name = cls.cname;
    value->property_name = property.cname;
    value->cls = &cls;
    value->property = &property;

    if (property.value_getter == nullptr) {
        throw SourceError("Property has no getter: " + cls.cname +
            "." + property.cname);
    }
    AST::Visitor::visit_stdlib_value(value);
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