#include "text_adjuster.h"

#include <array>
#include <cstdint>
#include <unordered_map>


namespace
{
    /// @brief Число бит в байте.
    constexpr size_t bitsPerByte = 8;
    
    /// @brief Строка, состоящая из одного дефиса.
    const std::string hyphenOnlyString = "-";

    /// @brief Таблица отображения верхнего регистра в нижний кириллических символов UTF-8.
    const std::unordered_map<uint16_t, uint16_t> cyrillicUtf8Map =
    {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        { 0x90D0, 0xB0D0 }, // А
        { 0x91D0, 0xB1D0 }, // Б
        { 0x92D0, 0xB2D0 }, // В
        { 0x93D0, 0xB3D0 }, // Г
        { 0x94D0, 0xB4D0 }, // Д
        { 0x95D0, 0xB5D0 }, // Е
        { 0x81D0, 0x91D1 }, // Ё
        { 0x96D0, 0xB6D0 }, // Ж
        { 0x97D0, 0xB7D0 }, // З
        { 0x98D0, 0xB8D0 }, // И
        { 0x99D0, 0xB9D0 }, // Й
        { 0x9AD0, 0xBAD0 }, // К
        { 0x9BD0, 0xBBD0 }, // Л
        { 0x9CD0, 0xBCD0 }, // М
        { 0x9DD0, 0xBDD0 }, // Н
        { 0x9ED0, 0xBED0 }, // О
        { 0x9FD0, 0xBFD0 }, // П
        { 0xA0D0, 0x80D1 }, // Р
        { 0xA1D0, 0x81D1 }, // С
        { 0xA2D0, 0x82D1 }, // Т
        { 0xA3D0, 0x83D1 }, // У
        { 0xA4D0, 0x84D1 }, // Ф
        { 0xA5D0, 0x85D1 }, // Х
        { 0xA6D0, 0x86D1 }, // Ц
        { 0xA7D0, 0x87D1 }, // Ч
        { 0xA8D0, 0x88D1 }, // Ш
        { 0xA9D0, 0x89D1 }, // Щ
        { 0xAAD0, 0x8AD1 }, // Ъ
        { 0xABD0, 0x8BD1 }, // Ы
        { 0xACD0, 0x8CD1 }, // Ь
        { 0xADD0, 0x8DD1 }, // Э
        { 0xAED0, 0x8ED1 }, // Ю
        { 0xAFD0, 0x8FD1 }, // Я
        { 0xB0D0, 0xB0D0 }, // а
        { 0xB1D0, 0xB1D0 }, // б
        { 0xB2D0, 0xB2D0 }, // в
        { 0xB3D0, 0xB3D0 }, // г
        { 0xB4D0, 0xB4D0 }, // д
        { 0xB5D0, 0xB5D0 }, // е
        { 0x91D1, 0x91D1 }, // ё
        { 0xB6D0, 0xB6D0 }, // ж
        { 0xB7D0, 0xB7D0 }, // з
        { 0xB8D0, 0xB8D0 }, // и
        { 0xB9D0, 0xB9D0 }, // й
        { 0xBAD0, 0xBAD0 }, // к
        { 0xBBD0, 0xBBD0 }, // л
        { 0xBCD0, 0xBCD0 }, // м
        { 0xBDD0, 0xBDD0 }, // н
        { 0xBED0, 0xBED0 }, // о
        { 0xBFD0, 0xBFD0 }, // п
        { 0x80D1, 0x80D1 }, // р
        { 0x81D1, 0x81D1 }, // с
        { 0x82D1, 0x82D1 }, // т
        { 0x83D1, 0x83D1 }, // у
        { 0x84D1, 0x84D1 }, // ф
        { 0x85D1, 0x85D1 }, // х
        { 0x86D1, 0x86D1 }, // ц
        { 0x87D1, 0x87D1 }, // ч
        { 0x88D1, 0x88D1 }, // ш
        { 0x89D1, 0x89D1 }, // щ
        { 0x8AD1, 0x8AD1 }, // ъ
        { 0x8BD1, 0x8BD1 }, // ы
        { 0x8CD1, 0x8CD1 }, // ь
        { 0x8DD1, 0x8DD1 }, // э
        { 0x8ED1, 0x8ED1 }, // ю
        { 0x8FD1, 0x8FD1 }  // я
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        { 0xD090, 0xD0B0 }, // А
        { 0xD091, 0xD0B1 }, // Б
        { 0xD092, 0xD0B2 }, // В
        { 0xD093, 0xD0B3 }, // Г
        { 0xD094, 0xD0B4 }, // Д
        { 0xD095, 0xD0B5 }, // Е
        { 0xD081, 0xD191 }, // Ё
        { 0xD096, 0xD0B6 }, // Ж
        { 0xD097, 0xD0B7 }, // З
        { 0xD098, 0xD0B8 }, // И
        { 0xD099, 0xD0B9 }, // Й
        { 0xD09A, 0xD0BA }, // К
        { 0xD09B, 0xD0BB }, // Л
        { 0xD09C, 0xD0BC }, // М
        { 0xD09D, 0xD0BD }, // Н
        { 0xD09E, 0xD0BE }, // О
        { 0xD09F, 0xD0BF }, // П
        { 0xD0A0, 0xD180 }, // Р
        { 0xD0A1, 0xD181 }, // С
        { 0xD0A2, 0xD182 }, // Т
        { 0xD0A3, 0xD183 }, // У
        { 0xD0A4, 0xD184 }, // Ф
        { 0xD0A5, 0xD185 }, // Х
        { 0xD0A6, 0xD186 }, // Ц
        { 0xD0A7, 0xD187 }, // Ч
        { 0xD0A8, 0xD188 }, // Ш
        { 0xD0A9, 0xD189 }, // Щ
        { 0xD0AA, 0xD18A }, // Ъ
        { 0xD0AB, 0xD18B }, // Ы
        { 0xD0AC, 0xD18C }, // Ь
        { 0xD0AD, 0xD18D }, // Э
        { 0xD0AE, 0xD18E }, // Ю
        { 0xD0AF, 0xD18F }, // Я
        { 0xD0B0, 0xD0B0 }, // а
        { 0xD0B1, 0xD0B1 }, // б
        { 0xD0B2, 0xD0B2 }, // в
        { 0xD0B3, 0xD0B3 }, // г
        { 0xD0B4, 0xD0B4 }, // д
        { 0xD0B5, 0xD0B5 }, // е
        { 0xD191, 0xD191 }, // ё
        { 0xD0B6, 0xD0B6 }, // ж
        { 0xD0B7, 0xD0B7 }, // з
        { 0xD0B8, 0xD0B8 }, // и
        { 0xD0B9, 0xD0B9 }, // й
        { 0xD0BA, 0xD0BA }, // к
        { 0xD0BB, 0xD0BB }, // л
        { 0xD0BC, 0xD0BC }, // м
        { 0xD0BD, 0xD0BD }, // н
        { 0xD0BE, 0xD0BE }, // о
        { 0xD0BF, 0xD0BF }, // п
        { 0xD180, 0xD180 }, // р
        { 0xD181, 0xD181 }, // с
        { 0xD182, 0xD182 }, // т
        { 0xD183, 0xD183 }, // у
        { 0xD184, 0xD184 }, // ф
        { 0xD185, 0xD185 }, // х
        { 0xD186, 0xD186 }, // ц
        { 0xD187, 0xD187 }, // ч
        { 0xD188, 0xD188 }, // ш
        { 0xD189, 0xD189 }, // щ
        { 0xD18A, 0xD18A }, // ъ
        { 0xD18B, 0xD18B }, // ы
        { 0xD18C, 0xD18C }, // ь
        { 0xD18D, 0xD18D }, // э
        { 0xD18E, 0xD18E }, // ю
        { 0xD18F, 0xD18F }  // я
#endif // __BYTE_ORDER__
    };

    /// @brief Таблица соответствия первого байта символа UTF-8 и длины этого символа.
    const std::array<uint8_t, 256> symbolSizeLut =
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 8
    };
}

TextAdjuster::TextAdjuster()
    : m_Handler()
{
}

TextAdjuster::~TextAdjuster() = default;

void TextAdjuster::setHandler(Handler handler)
{
    m_Handler = handler;
}

void TextAdjuster::adjust(const std::string& word) const
{
    if (!m_Handler)
    {
        throw std::logic_error("TextAdjuster::adjust error: no handler is set");
    }
    
    std::string result;
    result.reserve(word.size());
    
    const char* const first = word.c_str();
    const char* const last = first + word.size() - 1;
    const char* it = first;
    bool hyphen = false;

    while (it <= last)
    {
        const uint8_t symbolSize = symbolSizeLut[static_cast<uint8_t>(*it)];
        if (last + 1 - it < symbolSize)
        {
            throw std::logic_error("TextAdjuster::adjust error: not UTF-8 text");
        }
        
        switch (symbolSize)
        {
        case 1:
            adjustOneByteSymbol(it, first, last, hyphen, result);
            ++it;
            break;
         
        case 2:
            adjustTwoByteSymbol(it, result);
            it += symbolSize;
            break;
            
        default:
            it += symbolSize;
        }
    }
    
    result.shrink_to_fit();
    if (!result.empty() && result != hyphenOnlyString)
    {
        m_Handler(std::move(result));
    }
}

void TextAdjuster::adjustOneByteSymbol(const char* it,
                                       const char* const first,
                                       const char* const last,
                                       bool& hyphen,
                                       std::string& result)
{
    // Остаются только буквы, цифры и дефис.
    if ('A' <= *it && *it <= 'Z')
    {
        result += *it - 'A' + 'a';
    }
    else if (('a' <= *it && *it <= 'z') ||
             ('0' <= *it && *it <= '9'))
    {
        result += *it;
    }
    // В слове может быть только один дефис, причем не в самом начале и не в самом конце.
    else if (*it == '-' && !hyphen && it != first && it != last)
    {
        hyphen = true;
        result += *it;
    }
}

void TextAdjuster::adjustTwoByteSymbol(const char* it, std::string& result)
{
    // Если в кириллической таблице есть соотвествие символу,
    // добавить это соответствие к итоговому слову. Если нет - ничего не делать.
    try
    {
        const uint16_t* twoByteChar = reinterpret_cast<const uint16_t*>(it);
        const uint16_t replacement = cyrillicUtf8Map.at(*twoByteChar);
        result += static_cast<char>(replacement);
        result += static_cast<char>(replacement >> bitsPerByte);
    }
    catch (const std::out_of_range&)
    {
    }
}
