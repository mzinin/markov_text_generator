#pragma once

#ifndef WORD_SPLITTER_H
#define WORD_SPLITTER_H

#include <functional>
#include <sstream>


/// @class WordSplitter
/// @brief Разбивает поток текста на слова.
class WordSplitter
{
public:
    /// @brief Тип обработчика выделенных слов.
    using Handler = std::function<void(std::string&&)>;
    
public:
    /// @brief Конструктор.
    WordSplitter();
    
    /// @brief Деструктор.
    ~WordSplitter();
    
    /// @brief Установить обработчик выделенных слов.
    /// @param[in] handler - Новый обработчик.
    void setHandler(Handler handler);
    
    /// @brief Обработать порцию текста.
    /// @param[in] text - Указатель на текст.
    /// @param[in] size - Размер текста.
    /// @throws std::exception в случае ошибки.
    void addText(const char* text, size_t size);
    
    /// @brief Обработать текст, оставшийся во внутреннем буфере.
    void flush();

private:
    /// @brief Обработчик выделенных слов.
    Handler m_Handler;
    
    /// @brief Внутренний буфер.
    std::stringstream m_Buffer;
};

#endif // WORD_SPLITTER_H
