#pragma once

#ifndef MARKOV_TEXT_CHAIN_H
#define MARKOV_TEXT_CHAIN_H

#include <istream>
#include <list>
#include <memory>
#include <ostream>
#include <string>


/// @class MarkovTextChain
/// @brief Текстовая цепь Маркова.
class MarkovTextChain
{
public:
    /// @brief Тип слова.
    using Word = std::string;
    
    /// @brief Тип последовательности слов.
    using Words = std::list<Word>;
    
public:
    /// @brief Конструктор.
    /// @param[in] chainOrder - Порядок цепи Маркова.
    MarkovTextChain(size_t chainOrder = 0);
    
    /// @brief Деструктор.
    ~MarkovTextChain();
    
    /// @brief Задать порядок цепи.
    /// @param[in] chainOrder - Порядок цепи Маркова.
    void setOrder(size_t chainOrder);
    
    /// @brief Получить порядок цепи.
    /// @return Порядок цепи Маркова.
    size_t order() const;
    
    /// @brief Заполнить цепь из потока.
    /// @param[in] input - Поток ввода.
    /// @throws std::exception в случае ошибки.
    void load(std::istream& input);
    
    /// @brief Сохранить цепь в поток.
    /// @param[in] output - Поток вывода.
    /// @throws std::exception в случае ошибки.
    void save(std::ostream& output) const;
    
    /// @brief Добавить слово к цепи.
    /// @param[in] word - Новое слово.
    /// @throws std::exception в случае ошибки.
    void addWord(Word&& word);
    
    /// @brief Сгенерировать слово, соответствующее заданной последовательности слов.
    /// @param[in] words - Последовательности слов.
    /// @return Слово.
    /// @throws std::exception в случае ошибки.
    Word generateWord(const Words& words) const;
    
    /// @brief Подготовить цепь к обработке нового потока слов.
    void flush();
    
private:
    /// @brief Разобрать строку, описывающую состояние цепи Маркова.
    /// @param[in] string - Входная строка.
    /// @throws std::exception в случае ошибки.
    void parseChainString(const std::string& string);
    
    /// @brief Сбросить состояние цепи.
    void reset();

private:
    /// @brief Порядок цепи Маркова.
    size_t m_Order;
    
    /// @brief Последняя рассмотренная последовательность слов.
    Words m_CurrentWords;
    
    /// @brief Тип внутренней цепи.
    struct InnerChain;
    
    /// @brief Указатель на внутреннюю цепь.
    std::unique_ptr<InnerChain> m_Chain;
    
    /// @brief Заголовок для сериализации цепи.
    static const std::string m_ChainHeader;
    
    /// @brief Концевик для сериализации цепи.
    static const std::string m_ChainTrailer;
    
    /// @brief Разделитель для сериализации цепи.
    static const std::string m_Delimiter;
};

#endif // MARKOV_TEXT_CHAIN_H
