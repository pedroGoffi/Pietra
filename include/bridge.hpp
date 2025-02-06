#ifndef BRIDGE_HPP
#define BRIDGE_HPP
#include "arena.hpp"
#include "smallVec.hpp"
#include <stack>
#include "type.hpp"
#include "interns.hpp"
#include <map>
#include "resolve.hpp"
#include <string>
#include <llvm/IR/Value.h>


using namespace Pietra::Core;
using namespace Pietra::Ast;
using namespace Pietra::Resolver;
/*
Bridge idea is to make the AST code to lower level, making it easier to compile to LLVM or assembly x86_64
*/

namespace Pietra::CBridge {

    namespace Constants {
        struct ConstantExpression {
            const char* name;
            Expr* expr;

            static ConstantExpression* Create(const char* name, Expr* expr);
            void display() const;
        };
    }

    namespace Variables {
        struct Variable {
            const char*     name;
            Type*           type;
            Expr*           initializer;
            bool            isGlobal = false;
            bool            isParameter = false;
            int             stackOffset;
            llvm::Value*    value;

            Variable(const char* name, Type* type, Expr* initializer, bool isGlobal = false, bool isParameter = false, int stackOffset = 0);
            void display() const;
        };
    }

    namespace Procedures {
        using namespace Variables;

        struct Procedure {
            const char*         name;
            bool                isRecursive;
            size_t              stackAllocation;
            Type*               returnType;
            SVec<Variable*>*    parameters      = arena_alloc<SVec<Variable*>>();
            SVec<Variable*>*    localVariables  = arena_alloc<SVec<Variable*>>();
            ScopeManager*       scope           = nullptr;
            bool                is_used         = false;
            bool                is_extern       = false;

            ScopeManager*& getScope();
            
            Procedure(const char* name, Type* returnType);
            size_t calculateStackAllocation();
            Variable* findVariableInList(const char* str, SVec<Variable*>* list);
            Variable* findLocalVariable(const char* name);
            Variable* findParameter(const char* name);
            bool hasNameCollision(const char* name);
            void allocateLocalVariable(Variable*& var, bool isParameter);
            void display() const;
        };
    }

    namespace State {
        enum CompilerStateKind {
            S_NONE,
            S_GETADDR,
            S_PROCEDURE
        };

        struct CompilerState {
            CompilerStateKind stateKind;

            CompilerState(CompilerStateKind stateKind);
            CompilerState();
            bool operator==(CompilerState& other);
            bool operator!=(CompilerState& other);
            CompilerState& set(CompilerStateKind newKind);
            bool is(CompilerStateKind kind) const;
            void reset();
            void display() const;

        
            static CompilerState& None();
            static CompilerState& GetAddress();
            static CompilerState& GotProcedure();
            
        };
    }

    namespace Context {
        using namespace Procedures;
        using namespace Variables;

        struct AssemblyContext {
            Type*                                   self = nullptr;
            State::CompilerState                    compilerState;
            SVec<Procedure*>                        compiledProcedures;
            FILE*                                   outputChannel;                    
            Procedure*                              currentProcedure;
            SVec<Variable*>                         globalVariables;
            std::map<const char*, int>              enumMap;
            SVec<Constants::ConstantExpression*>    constantExpressions;
            std::vector<Procedure*>                 tracebackProcedures;                    

            
            ScopeManager* scope;


            
            ScopeManager* getGlobalScope();
            ScopeManager* getLocalScope();
            
            void currentProcedureRewind();
            Constants::ConstantExpression* getConstantExpression(const char* name);
            void addConstantExpression(Constants::ConstantExpression* expr);            
            AssemblyContext();
            void setSelf(Type* ty);
            Type* getSelf();
            void clearSelf();
            Procedure* getCurrentProcedure();
            void clearCurrentProcedure();
            void setCurrentProcedure(Procedure* proc);
            void addProcedure(Procedure* proc);
            void allocateVariableInCurrentProcedure(Variable* var, bool isParameter);
            Variable* getGlobalVariables(const char* name);
            void allocateGlobalVariable(Variable* var);            
            Procedure* findProcedureByName(const char* name);
        };
    }

    // TypeDefinition class
    namespace TypeDefinitions {
        class TypeDefinition {
        public:
            std::string typeName;
            Type* type;

            TypeDefinition(const std::string& name, Type* ty);
            void display() const;
            bool isEqual(const TypeDefinition& other) const;
        };
    }

    // TypeDefinitionManager class to manage all TypeDefinitions
    namespace TypeDefinitions {
        class TypeDefinitionManager {
        private:
            std::map<std::string, TypeDefinition*> typeDefs; // A map to store type definitions by name

        public:
            void addTypeDefinition(const std::string& name, Type* type);
            TypeDefinition* getTypeDefinition(const std::string& name);
            void removeTypeDefinition(const std::string& name);
            void displayAllTypeDefinitions() const;
            void clearAllTypeDefinitions();
            ~TypeDefinitionManager();
        };
    }

    namespace Factory {
        using namespace Variables;
        using namespace Procedures;

        int createGlobalString(SVec<const char*>& list, const char* str);
        Variable* createVariable(const char* name, Type* type, Expr* initializer, bool isGlobal, bool isParameter);
        int* getEnumValue(const char* str);
        void setEnumValue(const char* str, int val);        
        Procedure* createProcedure(const char* name, Type* returnType);
        Constants::ConstantExpression* createConstantExpression(const char* name, Expr* expr);
    }
}


#endif /* BRIDGE_HPP */
