#include "word_splitter.h"

#include <algorithm>
#include <iostream>


WordSplitter::WordSplitter()
    : m_Handler()
    , m_Buffer()
{
}

WordSplitter::~WordSplitter() = default;

void WordSplitter::setHandler(Handler handler)
{
    m_Handler = handler;
}

void WordSplitter::addText(const char* text, size_t size)
{
    if (!m_Handler)
    {
        throw std::logic_error("WordSplitter::addText error: no handler is set");
    }
    
    // Количество слов в новой порции текста.
    const unsigned newWords = std::count(text, text + size, ' ');
    m_Buffer.write(text, size);
    
    // Выделить все слова и передать их дальше.
    for (unsigned i = 0; i < newWords; ++i)
    {
        std::string word;
        m_Buffer >> word;
        if (!word.empty())
        {
            m_Handler(std::move(word));
        }
    }
    
    // Сбросить флаги буфера для следующего цикла записи/чтения
    m_Buffer.clear();
}

void WordSplitter::flush()
{
    while (true)
    {
        std::string word;
        m_Buffer >> word;
        if (!word.empty())
        {
            m_Handler(std::move(word));
        }
        else
        {
            break;
        }
    }
}
