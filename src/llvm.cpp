#ifndef  LLVM_COMPILER_CPP
#define  LLVM_COMPILER_CPP
#include "../include/llvm.hpp"
#include "arena.cpp"
#include "pprint.cpp"
#include <cstdint>
#include <fstream>
#include <ios>
#include <llvm/Analysis/BasicAliasAnalysis.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <ostream>
#include <iostream>
#include "dev_debug.cpp"

using namespace Pietra;
using namespace llvm;
std::unique_ptr<LLVMContext>                        Ctx;
std::unique_ptr<Module>                             Mod;
std::unique_ptr<IRBuilder<>>                        Builder;    
std::map<std::string, AllocaInst*>                  NamedValues;
std::map<std::string, LLVMCompiler::C_var*>         globalVars;
std::map<std::string, LLVMCompiler::C_struct*>      NamedStructs;
LLVMCompiler::UCtx                                  uctx;
int count(){
    static int __count = 0;
    return __count++;
}
std::string lcount(){
    std::string LABEL_NAME  = std::string("L");
    std::string LABEL_COUNT = std::to_string(count());    
    return LABEL_NAME + LABEL_COUNT;
}
BasicBlock* LLVMCompiler::new_bb(Function* curr_F){
    return BasicBlock::Create(*Ctx, lcount(), curr_F);
}
Value* LLVMCompiler::compile_icmp_zero(Value* v){
    assert(v);
    ConstantInt* zero = ConstantInt::get(*Ctx, APInt(64, 0));
    return Builder->CreateICmpEQ(v, zero);
}
Value* LLVMCompiler::cond_jmp(Value* cond, BasicBlock* True, BasicBlock* False){
    Value* cmp      = compile_icmp_zero(cond);
    Value* cmp_not  = Builder->CreateNot(cmp);
    return Builder->CreateCondBr(cmp_not, True, False);
}
            
Value* LLVMCompiler::get_null(){
    return Constant::getNullValue(Builder->getCurrentFunctionReturnType());            
}
                
Value* LLVMCompiler::compile_modify(Lexer::tokenKind kind, Expr* var, Expr* rhs){
    if(kind == Lexer::TK_EQ){
        uctx.set(U_GET_ADDR);
        Value* var_val = compile_expr(var);
        uctx.reset();
        Value* rhs_val = compile_expr(rhs);                
        Builder->CreateStore(rhs_val, var_val);
    }
}
static inline bool is_modify(Lexer::tokenKind kind){
    return kind == Pietra::Lexer::TK_EQ;
}
Value* LLVMCompiler::compile_unary(Lexer::tokenKind kind, Expr* expr){
    switch(kind){
        case Lexer::TK_MULT: {
            Value* ptr = compile_expr(expr);
            assert(ptr->getType()->isPointerTy());
            return Builder->CreateLoad(ptr);
        }
        default: assert(0);
    }
}
Value* LLVMCompiler::compile_binary(Lexer::tokenKind kind, Expr* lhs, Expr* rhs){
    if(is_modify(kind)){
        return compile_modify(kind, lhs, rhs);
    }
    Value* left = compile_expr(lhs);
    Value* right = compile_expr(rhs);
    assert(left and right);
    switch(kind){
        case Lexer::TK_ADD:     return Builder->CreateAdd(left, right);
        case Lexer::TK_MULT:    return Builder->CreateMul(left, right);
        case Lexer::TK_SUB:     return Builder->CreateSub(left, right);            
        case Lexer::TK_LT:      {
            Value* val = Builder->CreateICmpSLT(left, right);
            return Builder->CreateIntCast(val, left->getType(), true);
        }            
        default: assert(0 && "undefined");
    }

    
    
}
Value* LLVMCompiler::compile_init_var(const char* name, TypeSpec* pts, Expr* rhs, bool isGlobal){    
    llvm::Type* ltype = typespec(pts);
    assert(ltype);    
    Value* opt_init = nullptr;
    if(rhs){
        opt_init = compile_expr(rhs);
    }


    if(isGlobal){
        std::string sname = std::string(name);
        assert(globalVars.find(sname) == globalVars.end());
        GlobalVariable* var = new GlobalVariable(
            ltype, 
            false, 
            GlobalVariable::WeakAnyLinkage,
            0,
            name);

        Mod->getOrInsertGlobal(name, ltype);
        globalVars[sname] = C_var::New(name, var, opt_init);
        return var;
    }
    else {
        AllocaInst* alloca_v = Builder->CreateAlloca(ltype, nullptr, name);            
        NamedValues[name] = alloca_v;
        if(opt_init){
            Builder->CreateStore(opt_init, alloca_v);
        }    
        return alloca_v;
    }
}
Value* LLVMCompiler::compile_call(Expr* base, SVec<Expr*> pargs){
    Value* callee = compile_expr(base);
    std::vector<Value*> largs;
    for(Expr* parg: pargs){
        Value* arg = compile_expr(parg);
        assert(arg);
        largs.push_back(arg);
    }
    Function* func = (llvm::Function*) callee;
    Value* ret =  Builder->CreateCall(func, largs);

    return ret;
}
Value* LLVMCompiler::compile_field_access(Expr* parent, Expr* child){
    Value* p = compile_expr(parent);
    switch(child->kind){
        case EXPR_NAME: {
            const char* fieldn = child->name;
            std::string pstname = std::string(p->getType()->getStructName());
            assert(p->getType()->isAggregateType());
            if(NamedStructs.find(pstname) != NamedStructs.end()){
                C_struct* st = NamedStructs[pstname];

                int offset = 0;
                for(AggregateItem* item: st->items){
                    for(auto name: item->field.names){
                        if(name == Core::cstr(fieldn)){
                            // Get this field with offset           
                            assert(0 && "TODO");                            
                        } else {
                            offset += 1;
                        }
                    }
                }
            }
            

            printf("str = %s\n", p->getType()->getStructName().data());
            exit(1);
        }

        default: assert(0);
    }
    return p;
    //return Builder->CreateExtractElement(c, p);
}
Value* LLVMCompiler::compile_expr(Expr* expr){
    switch(expr->kind){
        case EXPR_INT:  {
            Value* val = ConstantInt::get(*Ctx, APInt(64, expr->int_lit));
            return Builder->Insert(val);
        }
        case EXPR_FLOAT:    {
            Value* val = ConstantFP::get(*Ctx, APFloat(expr->float_lit));        
            return Builder->Insert(val);
        }
        case EXPR_NAME: {
            auto opt_load_var = [](AllocaInst* val) -> Value* {

                if(uctx.expect(U_GET_ADDR)){
                    return val;    
                }
                return Builder->CreateLoad(val->getAllocatedType(), val);
            };
            if(NamedValues.find(expr->name) != NamedValues.end()){                
                Value* val = NamedValues[expr->name];                                
                return opt_load_var((AllocaInst*)val);
                
            } else if(globalVars.find(expr->name) != globalVars.end()){
              Value* val = globalVars[expr->name]->var;
              return opt_load_var((AllocaInst*)val);
            } else if (Function* F = Mod->getFunction(expr->name)){
                return F;
            }
            
            printf("[ERROR]: the name %s, is not declared.\n", expr->name);
            exit(1);
            
        }       
        case EXPR_CALL:     return compile_call(expr->call.base, expr->call.args);    
        case EXPR_STRING:   return Builder->CreateGlobalStringPtr(expr->string_lit);;
        case EXPR_UNARY:    return compile_unary(expr->unary.unary_kind, expr->unary.expr);
        case EXPR_BINARY:   return compile_binary(expr->binary.binary_kind, expr->binary.left, expr->binary.right);
        case EXPR_INIT_VAR: return compile_init_var(expr->init_var.name, expr->init_var.type, expr->init_var.rhs, false);
        case EXPR_FIELD:    return compile_field_access(expr->field_acc.parent, expr->field_acc.children);
        default: assert(0);
    }
}
Value* LLVMCompiler::compile_if_clause(IfClause* clause, BasicBlock* block, llvm::BasicBlock* abs_end){
    assert(clause);    
    Value* if_v = compile_expr(clause->cond);
    assert(if_v);
    
    Value* zero_v = ConstantInt::get(*Ctx, APInt(64, 0, true));
    
    
    if_v = Builder->CreateICmpNE(if_v, zero_v); // if_v == 0     
    
    Builder->CreateCondBr(if_v, block, abs_end);
    Builder->SetInsertPoint(block);
    compile_stmt_block(clause->block);
    Builder->CreateBr(abs_end);
    Builder->SetInsertPoint(abs_end);    
    return nullptr;
}
Value* LLVMCompiler::compile_if(IfClause* ifc, SVec<IfClause*> elifs, SVec<Stmt*> else_block){            
    Function* F = Builder->GetInsertBlock()->getParent();
    assert(F);
    BasicBlock* ifbb = new_bb(F);
    SVec<BasicBlock*> elifs_bbs;
    BasicBlock* elsebb = nullptr;
    bool hasElse = false;
    for(IfClause* _: elifs){
        elifs_bbs.push(new_bb(F));
    }
    if(else_block.len() > 0){
        hasElse = true;
        elsebb = new_bb(F);
    }
    BasicBlock* end = new_bb(F);
    
    compile_if_clause(ifc, ifbb, end);
    int elif_id = 0;
    for(IfClause* elif: elifs){
        BasicBlock* elif_bb = elifs_bbs.at(elif_id++);
        Builder->SetInsertPoint(elif_bb);
        compile_if_clause(elif, elif_bb, end);
    }
    if(hasElse){
        assert(else_block.len() > 0);
        assert(elsebb);
        Builder->SetInsertPoint(elsebb);
        compile_stmt_block(else_block);
        Builder->CreateBr(end);
        Builder->SetInsertPoint(end);
    }
    return nullptr;
}
Value* LLVMCompiler::compile_for(Expr* init, Expr* cond, Expr* inc, SVec<Stmt*> block){    
    BasicBlock* curr    = Builder->GetInsertBlock();
    Function* F         = curr->getParent();
    if(init){ // Infinite loop        
        BasicBlock* check   = new_bb(F);
        BasicBlock* body    = new_bb(F);
        BasicBlock* end     = new_bb(F);
        
        {      
            // Initialize the variable      
            assert(init->kind == Ast::EXPR_INIT_VAR);
            const char* var_name        = init->init_var.name;
            {  
                
                AllocaInst* alloc_v         = (AllocaInst*) compile_expr(init);            
                NamedValues[var_name]       = alloc_v;
            }
            // Check the condition 
            { 
                Builder->CreateBr(check);
                Builder->SetInsertPoint(check);                                                       
                Value* cond_v   = compile_expr(cond);                            
                cond_jmp(cond_v, body, end);                
            }
            // Compile the body
            // and Increment the variable
            {
                Builder->SetInsertPoint(body);
                compile_stmt_block(block);
                compile_expr(inc);
                Builder->CreateBr(check);
            }
            NamedValues.erase(var_name);
            Builder->SetInsertPoint(end);
            
            return get_null();
        }
    } else {
        assert(0);
    }
}
Value* LLVMCompiler::compile_while(Expr* cond, SVec<Stmt*> block){
    assert(cond);
    BasicBlock* cbb     = Builder->GetInsertBlock();
    Function*   F       = cbb->getParent();

    BasicBlock* check   = new_bb(F);
    BasicBlock* loop    = new_bb(F);
    BasicBlock* end     = new_bb(F);

    Builder->CreateBr(check);
    {
        Builder->SetInsertPoint(check);
        Value* cond_v   = compile_expr(cond);
        Value* cmp      = compile_icmp_zero(cond_v);
        Builder->CreateCondBr(cmp, end, loop);

        Builder->SetInsertPoint(loop);
        compile_stmt_block(block);
        Builder->CreateBr(check);
    }
    Builder->SetInsertPoint(end); 
    return nullptr;
    
}
Value* LLVMCompiler::compile_stmt(Stmt* stmt){
    switch(stmt->kind){
        case STMT_EXPR:     return compile_expr(stmt->expr);            
        case STMT_IF:       return compile_if(stmt->stmt_if.if_clause, stmt->stmt_if.elif_clauses, stmt->stmt_if.else_block);    
        case STMT_FOR:      return compile_for(stmt->stmt_for.init, stmt->stmt_for.cond, stmt->stmt_for.inc, stmt->stmt_for.block);
        case STMT_WHILE:    return compile_while(stmt->stmt_while->cond, stmt->stmt_while->block);
        case STMT_RETURN:   {
            Value* ret = compile_expr(stmt->expr);
            Function* F = Builder->GetInsertBlock()->getParent();
            llvm::Type* ty = F->getReturnType();                        
            if(ret->getType() != ty){
                printf("[ERR]: trying to return wrong type, in procedure..\n");
                printf("[NOTE]: this should be warned in the resolving step.\n");
                exit(1);
            }
            return Builder->CreateRet(ret);

        }
        default: assert(0);
    }
}
Value* LLVMCompiler::compile_stmt_block(SVec<Stmt*> block){
    Value* ret = nullptr;
    for(Stmt* stmt: block) {
        ret = compile_stmt(stmt);            
    }    
    return ret;
}


llvm::Function* LLVMCompiler::compile_decl_proc(Decl* decl){        
    NamedValues.clear();
    assert(decl->kind == DECL_PROC);
    if(decl->name == Core::cstr("main")){
        decl->name = "pietra_main";
    }
    // Forward declaration
    if(!decl->proc.is_complete){ 
        assert(decl->proc.block.len() == 0);        
        return nullptr;        
    }
  
    Function* F = Mod->getFunction(decl->name);
    
    if(!F){
        F = proc_proto(decl->name, decl->proc.params, decl->proc.ret);
    }    
    assert(F);
    dvprint("declaring procedure: %s\n", decl->name);
    for(Note* note: decl->notes){
        if(note->name == Core::cstr("extern")){
            assert(note->args.len() == 0);
            return F;
        }
    }
    llvm::BasicBlock* bb 
        = llvm::BasicBlock::Create(*Ctx, "entry", F);    
    Builder->SetInsertPoint(bb);    

    compile_stmt_block(decl->proc.block);           

    Builder->CreateRet(Constant::getNullValue(F->getReturnType()));
    assert(F);
    verifyFunction(*F);    
    return F;
}        

llvm::Type* LLVMCompiler::type(Ast::Type* ptype){
    assert(ptype);
    switch(ptype->kind){

        case TYPE_I8:   return Builder->getInt8Ty();
        case TYPE_I16:  return Builder->getInt16Ty();
        case TYPE_I32:  return Builder->getInt32Ty();
        case TYPE_I64:  return Builder->getInt64Ty();
        case TYPE_F32:  return Builder->getFloatTy();
        case TYPE_F64:  return Builder->getDoubleTy();
        case TYPE_VOID: return Builder->getVoidTy();
        case TYPE_STRUCT:   {
            std::vector<llvm::Type*> items;
            for(TypeField* item: ptype->aggregate.items){
                items.push_back(type(item->type));
            }
            return llvm::StructType::create(*Ctx, items, "tmp");
        }
        case TYPE_PTR: return type(ptype->base)->getPointerTo();
        case TYPE_ARRAY:    {
            llvm::Type* base = type(ptype->array.base);
            return llvm::ArrayType::get(base, ptype->array.size);
        }
        case TYPE_UNION:
        case TYPE_NONE:
        case TYPE_UNRESOLVED:
        
        case TYPE_FIRST_ARITHMETRIC_TYPE:
        
        case TYPE_LAST_ARITHMETRIC_TYPE:        
        
        
        
        case TYPE_PROC:
        case TYPE_CONST:
        default: assert(0 && "forbiden PType -> llvm::Type");
    }
}
Function* LLVMCompiler::proc_proto(const char* &name, SVec<ProcParam*> &params, TypeSpec* &ret){
    bool is_vararg = false;
    std::vector<llvm::Type*> lparams;
    // declare params by populating lparams
    for(ProcParam* param: params){
        if(param->isVararg){
            //assert(param == params.back()); // Assert that this param is the last parameter
            is_vararg = true;
            break;
        }
        assert(param->type);
        TypeSpec* ptype  = param->type;
        llvm::Type* ltype = typespec(ptype);
        assert(ltype);

        lparams.push_back(ltype);    
    }
    assert(ret);
    llvm::Type* lret = typespec(ret);
    assert(lret);    

    FunctionType* FT =
        FunctionType::get(lret, lparams, is_vararg);

    Function* F = Function::Create(FT, Function::ExternalLinkage, name, Mod.get());    
    {
        // Set the arguments name
        int arg_loc = 0;
        for(llvm::Argument& arg: F->args()){
            ProcParam* param = params.at(arg_loc++);
            arg.setName(param->name);
        }
    }

    verifyFunction(*F);
    return F;
    
}
llvm::Type* LLVMCompiler::typespec(TypeSpec* ts){
    assert(ts);
    switch(ts->kind){
        case Ast::TYPESPEC_POINTER: return typespec(ts->base)->getPointerTo();
        case Ast::TYPESPEC_ARRAY:   {
            Expr* sz = ts->array.size;
            pPrint::expr(sz);
            assert(sz->kind == EXPR_INT);
            int size = sz->int_lit;

            return llvm::ArrayType::get(typespec(ts->base), size);
        }
        case Ast::TYPESPEC_NAME:    {
            auto itt = NamedStructs.find(ts->name);
            if(itt != NamedStructs.end()){
                return NamedStructs[ts->name]->type;
            }
            [[fallthrough]];
        }
        default: 
            assert(ts->resolved_type);
            return type(ts->resolved_type);
    }
}
llvm::StructType* LLVMCompiler::compile_struct(const char* name, SVec<AggregateItem*> items){
    if(items.len() == 0) return nullptr;

    std::vector<llvm::Type*> items_ty;
    for(AggregateItem* item: items){
        switch(item->kind){
            case Pietra::Ast::AGGREGATEITEM_FIELD:
                assert(item->field.type->resolved_type);
                items_ty.push_back(type(item->field.type->resolved_type));
                break;

            case Pietra::Ast::AGGREGATEITEM_PROC:
            case Pietra::Ast::AGGREGATEITEM_NONE:
            default: assert(0 && "unexpected");
        }
    }
    
    llvm::StructType* st = llvm::StructType::create(
        *Ctx, 
        items_ty, 
        name);       

    assert(NamedStructs.find(name) == NamedStructs.end());

    NamedStructs[name] = C_struct::Create(name, items, st);
    return st;
}
LCErr LLVMCompiler::compile_decl_aggregate(Decl* decl){
    assert(decl->kind == Ast::DECL_AGGREGATE);

    switch(decl->aggregate.aggkind){
        case Ast::AGG_STRUCT:   {        
            llvm::StructType* s_ty = compile_struct(decl->name, decl->aggregate.items);
            assert(s_ty);
            printf("declared struct: %s\n", s_ty->getName().data());
        } return ERR_NONE;
        default: assert(0 && "unexpected");
    }


    
}
LCErr LLVMCompiler::compile_decl(Decl* decl){
    assert(decl);

    switch(decl->kind){
        case DECL_PROC:
            compile_decl_proc(decl);
            return ERR_NONE;
        case DECL_AGGREGATE:
            compile_decl_aggregate(decl);
            return ERR_NONE;
        case DECL_VAR:
            compile_init_var(decl->name, decl->var.type, decl->var.init, true);
            return ERR_NONE;
        default: assert(0 && "unimplemented compile llvm for this decl");
    }

    printf("[ERR]: unexpected decl-kind.\n");    
    pPrint::decl(decl);
    exit(1);
    return ERR_NONE;
    
}

void LLVMCompiler::compile__start(){
    FunctionType* FT = FunctionType::get(Builder->getVoidTy(), false);
    Function* F = Function::Create(
        FT, 
        Function::ExternalLinkage,
        "main",
        *Mod);
    BasicBlock* entry = new_bb(F);
    Builder->SetInsertPoint(entry);

    for(std::pair<std::string, C_var*> var: globalVars){
        printf("-= %s\n", var.first.data());
        C_var* c_var = var.second;
        if(c_var->init){                        
            printf("TODO: init global variable.\n");
        }
        
    }    

    // call main procedure
    Function* lmain = Mod->getFunction("pietra_main");
    assert(lmain);    
    Builder->CreateCall(lmain);    
    Builder->CreateRetVoid();
}
LCErr LLVMCompiler::compile_ast(SVec<Decl*> ast){
    // Initialize the compiler    
    Ctx     = std::make_unique<LLVMContext>();
    Mod     = std::make_unique<Module>("PLang", *Ctx);    
    Builder = std::make_unique<IRBuilder<>>(*Ctx);



    // Compile    
    for(Decl* decl: ast){                
        if(LCErr err = compile_decl(decl)) {
            fprintf(stderr, "[COMPILER ERR]\n");
            return err;
        }
    }

    compile__start();

    // Debug
    printf("-----------------------------------------------\n");
    std::error_code err;    
    llvm::raw_fd_ostream out("out.pietra", err);
    Mod->print(errs(), nullptr);
    Mod->print(out, nullptr);
    printf("-----------------------------------------------\n");
    return ERR_NONE;
}

#endif /*LLVM_COMPILER_CPP*/