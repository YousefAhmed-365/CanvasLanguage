#ifndef RES_MANAGER_HPP
#define RES_MANAGER_HPP

#include "CommonLibs.hpp"
#include <stack>

using Data = std::variant<void*, std::int32_t, float, std::string>;

enum SymbolSearchType{
    NONE,

    PRIVATE_SCOPE,
    RECURSIVE_SCOPE
};

class SymbolTable{
    private:
        // Variables
        std::shared_ptr<SymbolTable> m_parent;
        std::unordered_map<std::string, Data> m_table;
    public:
        // Variables
        // Constructor & Destructor
        SymbolTable(std::shared_ptr<SymbolTable> m_parent = nullptr);
        ~SymbolTable() = default;

        // Functions
        void push(const std::string& name, const Data &value);
        Data *find(const std::string& name, SymbolSearchType type = SymbolSearchType::RECURSIVE_SCOPE);

        std::shared_ptr<SymbolTable> getParent();
        std::unordered_map<std::string, Data> &getData();
};

class ScopeManager{
    private:
        // Variables
        std::shared_ptr<SymbolTable> m_globalScope;
        std::shared_ptr<SymbolTable> m_currentScope;

    public:
        // Variables
        std::stack<Data> globalStack;
        
        // Constructor & Destructor
        ScopeManager();
        ~ScopeManager() = default;
        
        // Functions
        void pushData(const std::string &name, const Data &value);
        Data *findData(const std::string &name, SymbolSearchType type = SymbolSearchType::RECURSIVE_SCOPE);

        void pushScope();
        void popScope();

        void debug_outScopes();
};

#endif