#ifndef PIETRA_SCOPE_HPP
#define PIETRA_SCOPE_HPP
#include "resolve.hpp"
#include <vector>

namespace Pietra {

/**
 * @class Scope
 * @brief Manages a collection of symbols within a specific scope.
 */
class Scope {
public:
    std::vector<Resolver::Sym*> m_symbols; ///< List of symbols in the current scope.
    int m_lvl; ///< Current scope level.
    int scope_level(); ///< Returns the current scope level.
    /**
     * @brief Adds a symbol to the current scope.
     * @param sym Pointer to the symbol to be added.
     */
    void add_sym(Resolver::Sym* sym);

    /**
     * @brief Finds a symbol by its name in the current scope.
     * @param name Name of the symbol to find.
     * @return Pointer to the found symbol, or nullptr if not found.
     */
    Resolver::Sym* find_sym(const char* name);
};

/**
 * @class ScopeManager
 * @brief Manages multiple scopes and provides functionality to enter new scopes and manage symbols.
 */
class ScopeManager {
public:
    std::vector<Scope*> m_scopes; ///< List of all scopes managed by the ScopeManager.
    int m_lvl = 0; ///< Current scope level.
    int get_scope_level(); ///< Returns the current scope level.

    /**
     * @brief Enters a new scope.
     */
    void enter_scope();
    void leave_scope();

    /**
     * @brief Adds a symbol to the current scope.
     * @param sym Pointer to the symbol to be added.
     * @return Index of the added symbol in the current scope.
     */
    int add_sym(Resolver::Sym* sym);

    /**
     * @brief Finds a symbol by its name across all scopes.
     * @param name Name of the symbol to find.
     * @return Pointer to the found symbol, or nullptr if not found.
     */
    Resolver::Sym* find_sym(const char* name);
};

}
#endif /*PIETRA_SCOPE_HPP*/