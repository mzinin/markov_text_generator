#pragma once

#ifndef TEXT_ADJUSTER_H
#define TEXT_ADJUSTER_H

#include <functional>


/// @class TextAdjuster
/// @brief Удаляет из слов пунктуацию и приводит символы к нижнему регистру..
class TextAdjuster
{
public:
    /// @brief Тип обработчика нормализованных слов.
    using Handler = std::function<void(std::string&&)>;
    
public:
    /// @brief Конструктор.
    TextAdjuster();
    
    /// @brief Деструктор.
    ~TextAdjuster();
    
    /// @brief Установить обработчик нормализованных слов.
    /// @param[in] handler - Новый обработчик.
    void setHandler(Handler handler);
    
    /// @brief Привести слово к нормальному виду.
    /// @param[in,out] word - Исходное слово.
    /// @throws std::exception в случае ошибки.
    void adjust(std::string&& word) const;
    
private:
    /// @brief Обработать однобайтовый символ UTF-8.
    /// param[in] it - Указатель на символ.
    /// param[in] first - Указатель на первый символ текущего слова.
    /// param[in] last - Указатель на последний символ текущего слова.
    /// param[in,out] hyphen - Флаг наличия дефиса в слове.
    /// param[out] result - Итоговое нормализованное слово.
    static void adjustOneByteSymbol(const char* it,
                                    const char* const first,
                                    const char* const last,
                                    bool& hyphen,
                                    std::string& result);
                                    
    /// @brief Обработать двухбайтовый символ UTF-8.
    /// param[in] it - Указатель на начало символа.
    /// param[out] result - Итоговое нормализованное слово.
    static void adjustTwoByteSymbol(const char* it, std::string& result);

private:
    /// @brief Обработчик нормализованных слов.
    Handler m_Handler;
};

#endif // TEXT_ADJUSTER_H
