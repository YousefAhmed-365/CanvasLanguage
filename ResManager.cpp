#include "headers/ResManager.hpp"
/* SymbolTable Class */
// Variables
// Constructor & Destructor
SymbolTable::SymbolTable(std::shared_ptr<SymbolTable> m_parent){
    this->m_parent = m_parent;
}

// Functions
void SymbolTable::push(const std::string& name, const Data& value){
    m_table[name] = value;
}

Data SymbolTable::find(const std::string& name, SymbolSearchType type){
    if(m_table.find(name) != m_table.end()){
        return m_table[name];
    }else if(m_parent != nullptr && type == SymbolSearchType::RECURSIVE_SCOPE){
        return m_parent->find(name);
    }else{
        throw std::runtime_error("~Error~ Can't find symbol \'" + name + "\'.");
    }
}

std::shared_ptr<SymbolTable> SymbolTable::getParent(){
    return m_parent;
}
std::unordered_map<std::string, Data> &SymbolTable::getData(){
    return m_table;
}

/* ScopeManager Class */
// Variables
// Constructor & Destructor
ScopeManager::ScopeManager(){
    m_globalScope = std::make_shared<SymbolTable>(nullptr);
    m_currentScope = m_globalScope;
}

// Functions
void ScopeManager::pushScope(){
    m_currentScope = std::make_shared<SymbolTable>(m_currentScope);
}
void ScopeManager::popScope(){
    if(m_currentScope->getParent() != nullptr){
        m_currentScope = m_currentScope->getParent();
    }
}

void ScopeManager::pushData(const std::string &name, const Data &value){
    m_currentScope->push(name, value);
}
Data ScopeManager::findData(const std::string &name, SymbolSearchType type){
    return m_currentScope->find(name, type);
}

void ScopeManager::debug_outTree() {
    std::stack<std::shared_ptr<SymbolTable>> scopeStack;
    std::shared_ptr<SymbolTable> current = m_currentScope;

    while (current != nullptr) {
        scopeStack.push(current);
        current = current->getParent();
    }

    std::cout << "Scope Tree:\n";
    int depth = 0;
    while (!scopeStack.empty()) {
        current = scopeStack.top();
        scopeStack.pop();

        for (int i = 0; i < depth; ++i) {
            std::cout << "    ";
        }

        std::cout << "⤷ ● Scope\n";

        // Print variables in the scope
        for (const auto& symbol : current->getData()) {
            // Print indentation for variables
            for (int i = 0; i < depth + 1; ++i) {
                std::cout << "    ";
            }

            std::cout << "⤷ " << symbol.first << " : ";
            std::visit([](const auto& value) {
                std::cout << value;
            }, symbol.second);

            std::cout << std::endl;
        }
        ++depth;
    }
}