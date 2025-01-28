#ifndef BRIDGE_CPP
#define BRIDGE_CPP
#include "../include/bridge.hpp"





namespace Pietra::CBridge {
    using namespace Variables;
    using namespace Context;
    
    using namespace Constants;
    using namespace State;
    // --- Constants ---
    // ConstantExpression Constructor
    Constants::ConstantExpression* Constants::ConstantExpression::Create(const char* name, Expr* expr) {
        ConstantExpression* ce = arena_alloc<ConstantExpression>();
        ce->name = name;
        ce->expr = expr;
        return ce;
    }

    // --- Variables ---
    // Variable Constructor
    Variables::Variable::Variable(const char* name, Type* type, Expr* initializer, bool isGlobal, bool isParameter, int stackOffset)
        : name(name), type(type), initializer(initializer), isGlobal(isGlobal), isParameter(isParameter), stackOffset(stackOffset) {}

    // Display method for Variable
    void Variables::Variable::display() const {
        printf("Variable: %s, Type: %p, IsGlobal: %s\n", name, type, isGlobal ? "Yes" : "No");
    }

    // --- Procedures ---
    // Procedure Constructor
    


    Procedures::Procedure::Procedure(const char* name, Type* returnType)
        : name(name), returnType(returnType), isRecursive(false), stackAllocation(0) {}


    
    
    ScopeManager*& Procedures::Procedure::getScope(){ return this->scope; }
    // Calculate Stack Allocation
    size_t Procedures::Procedure::calculateStackAllocation() {
        // Placeholder for actual implementation
        return 0;
    }

    // Find Variable in List
    Variables::Variable* Procedures::Procedure::findVariableInList(const char* str, SVec<Variables::Variable*>* list) {
        str = cstr(str);
        for (auto* var : *list) {            
            if (cstr(var->name) == str) {                
                return var;
            }        
        }
        return nullptr;
    }

    // Find Local Variable
    Variables::Variable* Procedures::Procedure::findLocalVariable(const char* name) {
        return findVariableInList(name, this->localVariables);
    }

    // Find Parameter
    Variables::Variable* Procedures::Procedure::findParameter(const char* name) {
        return findVariableInList(name, this->parameters);
    }

    // Check Name Collision
    bool Procedures::Procedure::hasNameCollision(const char* name) {
        return findLocalVariable(name) != nullptr || findParameter(name) != nullptr;
    }

    // Allocate Local Variable
    void Procedures::Procedure::allocateLocalVariable(Variables::Variable*& var, bool isParameter) {
        this->stackAllocation += var->type->size;
        var->stackOffset = this->stackAllocation;
        if (isParameter) {            
            this->parameters->push(var);
        } else {
            this->localVariables->push(var);
        }
    }

    // Display method for Procedure
    void Procedures::Procedure::display() const {
        printf("Procedure: %s, IsRecursive: %s\n", name, isRecursive ? "Yes" : "No");
    }

    // --- State ---
    // CompilerState Constructor
    State::CompilerState::CompilerState(CompilerStateKind stateKind) : stateKind(stateKind) {}
    State::CompilerState::CompilerState() : stateKind(S_NONE) {}

    // Comparison Operators for CompilerState
    bool State::CompilerState::operator==(CompilerState& other) {
        return this->is(other.stateKind);
    }

    bool State::CompilerState::operator!=(CompilerState& other) {
        return !this->is(other.stateKind);
    }

    // Set method for CompilerState
    State::CompilerState& State::CompilerState::set(CompilerStateKind newKind) {
        stateKind = newKind;
        return *this;
    }

    // Check if state is of given kind
    bool State::CompilerState::is(CompilerStateKind kind) const {
        return stateKind == kind;
    }

    // Reset method for CompilerState
    void State::CompilerState::reset() {
        stateKind = S_NONE;
    }

    // Display method for CompilerState
    void State::CompilerState::display() const {
        printf("CompilerState: %d\n", stateKind);
    }

    static CompilerState state_none = CompilerState(S_NONE);
    static CompilerState state_getaddr = CompilerState(S_GETADDR);
    static CompilerState state_proc = CompilerState(S_PROCEDURE);
    CompilerState& State::CompilerState::None(){
        return state_none;
    }
    CompilerState& State::CompilerState::GetAddress(){
        return state_getaddr;
    }
    CompilerState& State::CompilerState::GotProcedure(){
        return state_proc;
    }
    // --- Context ---
    // AssemblyContext Constructor   
    ScopeManager* Context::AssemblyContext::getGlobalScope(){
        resolver_log({}, "###### [GOT GLOBAL SCOPE] ######\n");
        return this->scope;
    }
    
    void Context::AssemblyContext::currentProcedureRewind(){
        if(!this->tracebackProcedures.empty()){
            Procedure* last_traceback_procedure = this->tracebackProcedures.back();
            assert(last_traceback_procedure);
            this->tracebackProcedures.pop_back();
            this->setCurrentProcedure(last_traceback_procedure);

        }
    }
        
    ScopeManager* Context::AssemblyContext::getLocalScope(){
        resolver_log({}, "###### [GOT LOCAL SCOPE] ######\n");
        return this->currentProcedure->getScope();
    }

    Constants::ConstantExpression* Context::AssemblyContext::getConstantExpression(const char* name){
        name = cstr(name);
        for(Constants::ConstantExpression* ce: this->constantExpressions){
            if(ce->name == name) return ce;
        }
        return nullptr;
    }
    void Context::AssemblyContext::addConstantExpression(Constants::ConstantExpression* expr){
        if(this->getConstantExpression(expr->name)){
            printf("duplicated constexpr.\n");
        }

        this->constantExpressions.push(expr);
        ;
    }

    

    Context::AssemblyContext::AssemblyContext()
        : currentProcedure(nullptr), globalVariables({}), outputChannel(stdout), scope(new ScopeManager) {}

    // Set the type of the AssemblyContext's "self"
    void Context::AssemblyContext::setSelf(Type* ty) {
        this->self = ty;
    }

    // Get the type of the AssemblyContext's "self"
    Type* Context::AssemblyContext::getSelf() {
        return this->self;
    }

    // Clear the "self" in AssemblyContext
    void Context::AssemblyContext::clearSelf() {
        this->self = nullptr;
    }

    // Get the current procedure in the context
    Procedure* Context::AssemblyContext::getCurrentProcedure() {
        return this->currentProcedure;
    }

    // Clear the current procedure in the context
    void Context::AssemblyContext::clearCurrentProcedure() {
        this->setCurrentProcedure(nullptr);
    }

    // Set the current procedure in the context
    void Context::AssemblyContext::setCurrentProcedure(Procedure* proc) {
        this->currentProcedure = proc;
    }

    // Add a procedure to the context
    void Context::AssemblyContext::addProcedure(Procedure* proc) {    
        proc->getScope() = new ScopeManager;        
        this->compiledProcedures.push(proc);
        this->setCurrentProcedure(proc);
        this->tracebackProcedures.push_back(proc);
    }

    // Allocate a variable in the current procedure
    void Context::AssemblyContext::allocateVariableInCurrentProcedure(Variable* var, bool isParameter) {
        assert(this->currentProcedure);
        this->currentProcedure->allocateLocalVariable(var, isParameter);
    }

    // Get a global variable by name
    Variable* Context::AssemblyContext::getGlobalVariables(const char* name) {
        name = cstr(name);
        for (Variable* var : this->globalVariables) {
            if (var->name == name) {
                return var;
            }
        }
        return nullptr;
    }

    // Allocate a global variable
    void Context::AssemblyContext::allocateGlobalVariable(Variable* var) {
        this->globalVariables.push(var);
        
    }



    // Find procedure by name (currently returning nullptr)
    Procedure* Context::AssemblyContext::findProcedureByName(const char* name) {
        for(Procedures::Procedure* proc: this->compiledProcedures){
            if(proc->name == name){
                return proc;
            }
        }
        return nullptr;
    }

    // --- TypeDefinitions ---
    // TypeDefinition Constructor
    TypeDefinitions::TypeDefinition::TypeDefinition(const std::string& name, Type* ty)
        : typeName(name), type(ty) {}

    // Display method for TypeDefinition
    void TypeDefinitions::TypeDefinition::display() const {
        printf("TypeDefinition: %s, Type: %p\n", typeName.c_str(), type);
    }

    // Compare two TypeDefinitions
    bool TypeDefinitions::TypeDefinition::isEqual(const TypeDefinition& other) const {
        return typeName == other.typeName && type == other.type;
    }

    // --- TypeDefinitionManager ---
    // Add a TypeDefinition to the manager
    void TypeDefinitions::TypeDefinitionManager::addTypeDefinition(const std::string& name, Type* type) {
        if (typeDefs.find(name) == typeDefs.end()) {
            typeDefs[name] = new TypeDefinition(name, type);
        } else {
            printf("TypeDefinition with name '%s' already exists!\n", name.c_str());
        }
    }

    // Get a TypeDefinition by name
    TypeDefinitions::TypeDefinition* TypeDefinitions::TypeDefinitionManager::getTypeDefinition(const std::string& name) {
        auto it = typeDefs.find(name);
        if (it != typeDefs.end()) {
            return it->second;
        }
        
        return nullptr;
    }

    // Remove a TypeDefinition by name
    void TypeDefinitions::TypeDefinitionManager::removeTypeDefinition(const std::string& name) {
        auto it = typeDefs.find(name);
        if (it != typeDefs.end()) {
            delete it->second;
            typeDefs.erase(it);
        } else {
            printf("TypeDefinition with name '%s' not found to remove!\n", name.c_str());
        }
    }

    // Display all TypeDefinitions
    void TypeDefinitions::TypeDefinitionManager::displayAllTypeDefinitions() const {
        for (const auto& pair : typeDefs) {
            pair.second->display();
        }
    }

    // Clear all TypeDefinitions
    void TypeDefinitions::TypeDefinitionManager::clearAllTypeDefinitions() {
        for (auto& pair : typeDefs) {
            delete pair.second;
        }
        typeDefs.clear();
    }

    // Destructor to clean up memory
    TypeDefinitions::TypeDefinitionManager::~TypeDefinitionManager() {
        clearAllTypeDefinitions();
    }

    // --- Factory ---
    // Create a global string
    
    int Factory::createGlobalString(SVec<const char*>& list, const char* str) {
        list.push(str);
        return list.len() - 1;
    }

    // Create a variable
    Variable* Factory::createVariable(const char* name, Type* type, Expr* initializer, bool isGlobal, bool isParameter) {
        return new Variable(name, type, initializer, isGlobal, isParameter);
        
    }

    // Get an enum value
    int* Factory::getEnumValue(const char* str) {
        return nullptr;
    }

    // Set an enum value
    void Factory::setEnumValue(const char* str, int val) {
    }

    

    // Create a new procedure
    Procedure* Factory::createProcedure(const char* name, Type* returnType) {
        return new Procedure(name, returnType);
    }

    // Create a constant expression
    Constants::ConstantExpression* Factory::createConstantExpression(const char* name, Expr* expr) {
        return new Constants::ConstantExpression{cstr(name), expr};
    }
}



#endif /*BRIDGE_CPP*/