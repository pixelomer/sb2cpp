#include "transpiler.hpp"

#define VAR_LSTR "$"
#define VAR(name) (VAR_LSTR + name)
#define SUB_LSTR "sub_"
#define SUB(name) (SUB_LSTR + name)
#define LABEL(name) ("lbl_" + name)

namespace sb2cpp {

std::string Transpiler::transpile() {
    this->out.clear();

    this->out << "#include \"stdlib/SmallBasic.hpp\"" << CodeWriter::endl;

    this->out << "using namespace SmallBasic;" << CodeWriter::endl;
    this->out << CodeWriter::endl;

    // For iterator
    this->out << "inline bool ForIter(Obj const& acc, Obj const& end, " <<
        "Obj const& step) {" << CodeWriter::endl;
    this->out.indent++;
    this->out << "return (step >= Obj(0) && acc <= end) || " <<
        "(step <= Obj(0) && acc >= end);" << CodeWriter::endl;
    this->out.indent--;
    this->out << "}" << CodeWriter::endl << CodeWriter::endl;

    // Declarations
    #define DECLARE(map, type, lstr, rstr) do { \
        if (map.size() > 0) { \
            this->out << type " "; \
            int i=0; \
            for (auto &pair : map) { \
                this->out << lstr + pair.first + rstr; \
                if (++i != map.size()) { \
                    this->out << ", "; \
                } \
            } \
            this->out << ";" << CodeWriter::endl; \
        } \
    } while (0)
    DECLARE(this->source.variables, "Obj", VAR_LSTR, "");
    DECLARE(this->source.subroutines, "void", SUB_LSTR, "()");
    #undef DECLARE

    // Subroutines
    for (auto &sub_elem : this->source.subroutines) {
        auto &sub = sub_elem.second.node;
        sub->accept(this);
    }

    // Entry point
    this->out << "void SmallBasic_EntryPoint()";
    this->write_block(this->source.entry_point);

    // Program start
    this->out << "int main() {" << CodeWriter::endl;
    this->out.indent++;
    this->out << "SmallBasic_EntryPoint();" << CodeWriter::endl;
    this->out.indent--;
    this->out << "}";

    std::string result = this->out.str();
    this->out.clear();

    return result;
}
void Transpiler::write_block(AST::Statement *statement) {
    this->out << " {" << CodeWriter::endl;
    this->out.indent++;
    statement->accept(this);
    this->out.indent--;
    this->out << "}" << CodeWriter::endl;
}
void Transpiler::write_condition(AST::Condition *condition) {
    this->out << " (";
    condition->accept(this);
    this->out << ")";
}
void Transpiler::visit_string(AST::StringValue *str_value) {
    this->out << "Obj(\"" + str_value->value + "\")";
}
void Transpiler::visit_number(AST::NumberValue *num_value) {
    this->out << "Obj(" << num_value->number << ")";
}
void Transpiler::visit_value_op(AST::BinaryValueOp *op) {
    this->node_parents.push(op);
    op->lvalue->accept(this);
    switch (op->op) {
        case AST::ValueOp::Multiply: this->out << " * "; break;
        case AST::ValueOp::Divide:   this->out << " / "; break;
        default: throw TranspilerError("Illegal value operator");
    }
    op->rvalue->accept(this);
    this->node_parents.pop();
}
void Transpiler::visit_logic_op(AST::BinaryLogicOp *op) {
    auto parent = this->get_parent<AST::BinaryLogicOp>();
    bool use_paran = (parent != nullptr) &&
        (parent->op == AST::LogicOp::And) &&
        (op->op == AST::LogicOp::Or);
    if (use_paran) this->out << "(";
    this->node_parents.push(op);
    op->lvalue->accept(this);
    switch (op->op) {
        case AST::LogicOp::And: this->out << " && "; break;
        case AST::LogicOp::Or:  this->out << " || "; break;
        default: throw TranspilerError("Illegal logic operator");
    }
    op->rvalue->accept(this);
    this->node_parents.pop();
    if (use_paran) this->out << ")";
}
void Transpiler::visit_compare_op(AST::BinaryCompareOp *op) {
    op->lvalue->accept(this);
    switch (op->op) {
        case AST::ComparisonOp::Equal:              this->out << " == "; break;
        case AST::ComparisonOp::GreaterThan:        this->out << " > "; break;
        case AST::ComparisonOp::GreaterThanOrEqual: this->out << " >= "; break;
        case AST::ComparisonOp::LessThan:           this->out << " < "; break;
        case AST::ComparisonOp::LessThanOrEqual:    this->out << " <= "; break;
        case AST::ComparisonOp::NotEqual:           this->out << " != "; break;
        default: throw TranspilerError("Illegal comparator");
    }
    op->rvalue->accept(this);
}
void Transpiler::visit_add_group(AST::AddGroup *group) {
    auto mult_div_op = this->get_parent<AST::BinaryValueOp>();
    if (mult_div_op != nullptr) this->out << "(";
    this->node_parents.push(group);
    for (auto &elem : group->elements) {
        if (elem.sign != AST::NoSignOp) {
            switch (elem.sign) {
                case AST::Positive: this->out << " + "; break;
                case AST::Negative: this->out << " - "; break;
                default: throw TranspilerError("Illegal sign");
            }
        }
        elem.value->accept(this);
    }
    this->node_parents.pop();
    if (mult_div_op != nullptr) this->out << ")";
}
void Transpiler::visit_array_assign(AST::ArrayAssign *assign) {
    this->out << VAR(assign->variable);
    for (auto key : assign->keys) {
        this->out << "[";
        key->accept(this);
        this->out << "]";
    }
    this->out << " = ";
    assign->value->accept(this);
    this->out << ";" << CodeWriter::endl;
}
void Transpiler::visit_array_value(AST::ArrayValue *value) {
    this->out << VAR(value->variable);
    for (auto key : value->keys) {
        this->out << "[";
        key->accept(this);
        this->out << "]";
    }
}
void Transpiler::visit_variable_value(AST::VariableValue *var) {
    if (var->is_subroutine) {
        this->out << SUB(var->variable);
    }
    else {
        this->out << VAR(var->variable);
    }
}
void Transpiler::visit_statement_group(AST::StatementGroup *group) {
    for (auto &statement : group->statements) {
        statement->accept(this);
    }
}
void Transpiler::visit_stdlib_call(AST::StdlibCall *call) {
    this->out << call->class_name << "::" << call->method_name
        << "(";
    for (int i=0; i<call->arguments.size(); i++) {
        call->arguments[i]->accept(this);
        if (i != call->arguments.size()-1) {
            this->out << ", ";
        }
    }
    this->out << ")";
    if (!call->returns_value) {
        this->out << ";" << CodeWriter::endl;
    }
}
void Transpiler::visit_stdlib_value(AST::StdlibValue *value) {
    this->out << value->class_name << "::" << "_Get" << value->property_name
        << "()";
}
void Transpiler::visit_stdlib_assign(AST::StdlibAssign *assign) {
    this->out << assign->class_name << "::" << "_Set" << assign->property_name
        << "(";
    assign->value->accept(this);
    this->out << ");" << CodeWriter::endl;
}
void Transpiler::visit_assign(AST::Assign *assign) {
    this->out << VAR(assign->variable) << " = ";
    assign->value->accept(this);
    this->out << ";" << CodeWriter::endl;
}
void Transpiler::visit_subroutine_call(AST::SubroutineCall *call) {
    this->out << SUB(call->subroutine_name) << "();" << CodeWriter::endl;
}
void Transpiler::visit_while_loop(AST::WhileLoop *loop) {
    this->out << "while";
    this->write_condition(loop->condition);
    this->write_block(loop->statement);
}
void Transpiler::visit_goto_label(AST::GotoLabel *label) {
    this->out << LABEL(label->name) << ":" << CodeWriter::endl;
}
void Transpiler::visit_goto_statement(AST::GotoStatement *statement) {
    this->out << "goto " << LABEL(statement->label) << ";" << CodeWriter::endl;
}
void Transpiler::visit_if_statement(AST::IfStatement *if_statement) {
    bool first = true;
    for (auto &part : if_statement->parts) {
        if (!first) {
            this->out << "else";
        }
        if (part.condition != nullptr) {
            this->out << "if";
            this->write_condition(part.condition);
        }
        this->write_block(part.statement);
        first = false;
    }
}
void Transpiler::visit_subroutine(AST::Subroutine *sub) {
    this->out << "void " << SUB(sub->subroutine_name) << "()";
    this->write_block(sub->contents);
}
void Transpiler::visit_for_loop(AST::ForLoop *loop) {
    auto acc = loop->initializer->variable;
    auto step = loop->step_value;
    AST::NumberValue default_step(1);
    if (step == nullptr) {
        step = &default_step;
    }
    this->out << "for (" << VAR(acc) << " = ";
    loop->initializer->value->accept(this);
    this->out << "; ForIter(" << VAR(acc) << ", ";
    loop->last_value->accept(this);
    this->out << ", ";
    step->accept(this);
    this->out << "); " << VAR(acc) << " += ";
    step->accept(this);
    this->out << ")";
    this->write_block(loop->statement);
}

}