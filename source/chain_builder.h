#pragma once

#ifndef CHAIN_BUILDER_H
#define CHAIN_BUILDER_H

#include "markov_text_chain.h"

#include <list>
#include <string>


/// @class ChainBuilder
/// @brief Анализирует аргументы командной строки и строит текстовую цепь Маркова.
class ChainBuilder
{
public:
    /// @brief Конструктор.
    ChainBuilder();
    
    /// @brief Деструктор.
    ~ChainBuilder();
    
    /// @brief Проанализировать аргументы командной строки.
    /// @param[in] argc - Число аргументов.
    /// @param[in] argv - Список аргументов.
    void init(int argc, char** argv);
    
    /// @brief Выполнить заданное командной строкой действие.
    /// @return true если действие выполнено успешно, false в противном случае.
    bool run() const;
    
private:
    /// @brief Показать справку.
    /// @return true если действие выполнено успешно, false в противном случае.
    bool printUsage() const;
    
    /// @brief Построить текстовую цепь Маркова.
    /// @return true если действие выполнено успешно, false в противном случае.
    bool buildChain() const;
    
    /// @brief Сохранить цепь Маркова.
    /// @param[in] chain - Цепь Маркова.
    /// @return true если действие выполнено успешно, false в противном случае.
    bool outputChain(const MarkovTextChain& chain) const;

private:
    /// @brief Порядок цепи Маркова.
    int m_Order;
    
    /// @brief Файл вывода цепи Маркова.
    std::string m_Output;
    
    /// @brief Список адресов для построения цепи Маркова.
    std::list<std::string> m_Urls;
    
    /// @brief Флаг необходимости показа справки.
    bool m_NeedHelp;
    
    /// @brief Имя исполняемого файла.
    std::string m_ProgramName;
};

#endif // CHAIN_BUILDER_H
