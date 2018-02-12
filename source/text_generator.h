#pragma once

#ifndef TEXT_GENERATOR_H
#define TEXT_GENERATOR_H

#include "markov_text_chain.h"

#include <list>
#include <string>


/// @class TextGenerator
/// @brief Анализирует аргументы командной строки и создает текст на основе цепи Маркова.
class TextGenerator
{
public:
    /// @brief Конструктор.
    TextGenerator();
    
    /// @brief Деструктор.
    ~TextGenerator();
    
    /// @brief Проанализировать аргументы командной строки.
    /// @param[in] argc - Число аргументов.
    /// @param[in] argv - Список аргументов.
    void init(int argc, char** argv);
    
    /// @brief Выполнить заданное командной строкой действие.
    /// @return true если действие выполнено успешно, false в противном случае.
    bool run();
    
private:
    /// @brief Показать справку.
    /// @return true если действие выполнено успешно, false в противном случае.
    bool printUsage() const;
    
    /// @brief Создать текст на основе цепи Маркова.
    /// @return true если действие выполнено успешно, false в противном случае.
    bool generateText();
    
    /// @brief Загрузить цепь Маркова.
    /// @param[in] chain - Цепь Маркова.
    /// @return true если действие выполнено успешно, false в противном случае.
    bool loadChain(MarkovTextChain& chain) const;
    
    /// @brief Проверить, достаточно ли начальных слов.
    /// @param[in] chainOrder - порядок цепи Маркова.
    /// @return true если начальных слов достаточно, false в противном случае.
    bool checkInitialWords(size_t chainOrder);

private:
    /// @brief Число слов, которое надо создать.
    int m_NumberOfNewWords;
    
    /// @brief Имя файла для ввода цепи Маркова.
    std::string m_Input;
    
    /// @brief Список начальных слов.
    MarkovTextChain::Words m_InitialWords;
    
    /// @brief Флаг необходимости показа справки.
    bool m_NeedHelp;
    
    /// @brief Имя исполняемого файла.
    std::string m_ProgramName;
};

#endif // TEXT_GENERATOR_H
