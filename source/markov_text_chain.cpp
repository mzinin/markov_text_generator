#include "markov_text_chain.h"

#include <cstdlib>
#include <ctime>
#include <sstream>
#include <unordered_map>
#include <vector>


namespace
{
    /// @class WordsKeeper
    /// @brief Хранит слова состояния цепи Маркова, обеспечивает их случайную выдачу.
    class WordsKeeper
    {
    public:
        /// @brief Конструктор.
        WordsKeeper()
            : m_TotalWords()
            , m_Words()
        {
        }
        
        /// @brief Перемещающий оператор присваивания.
        /// @brief another - Другой экземпляр WordsKeeper.
        WordsKeeper& operator=(WordsKeeper&& another)
        {
            m_TotalWords = another.m_TotalWords;
            m_Words = std::move(another.m_Words);
            another.m_TotalWords = 0;
            return *this;
        }
        
        /// @brief Деструктор.
        ~WordsKeeper() = default;
        
        /// @brief Добавить слово для хранения.
        /// @param[in] word - Новое слово.
        void addWord(const MarkovTextChain::Word& word)
        {
            m_Words.push_back(word);
            ++m_TotalWords;
        }
        
        /// @brief Добавить слово для хранения.
        /// @param[in] word - Новое слово.
        void addWord(MarkovTextChain::Word&& word)
        {
            m_Words.emplace_back(std::move(word));
            ++m_TotalWords;
        }
        
        /// @brief Случайно выдать одно из хранимых слов.
        /// @return Слово.
        const MarkovTextChain::Word& getWord() const
        {
            auto it = m_Words.begin();
            for (register int i = rand() % m_TotalWords; i > 0; --i)
            {
                ++it;
            }
            return *it;
        }
        
        /// @brief Сериализовать в строку.
        /// @return Строка.
        std::string toString() const
        {
            std::string result = std::to_string(m_TotalWords) + ' ';
            for (const auto& word : m_Words)
            {
                result += word + ' ';
            }
            
            return result;
        }
    
        /// @brief Проверка на пустоту.
        /// @return true если список хранимых слов пустой, false в противном случае.
        bool empty() const
        {
            return m_Words.empty();
        }
    
    private:
        /// @brief Количество хранимых слов.
        size_t m_TotalWords;
        
        /// @brief Список хранимых слов.
        std::list<MarkovTextChain::Word> m_Words;
    };

    /// @class WordsHash
    /// @brief Структура для вычисления хэша последовательности слов.
    struct WordsHash
    {
        /// @brief Вычислить хэш последовательности слов.
        /// @param[in] words - Последовательность слов.
        /// @return Хэш.
        size_t operator()(const MarkovTextChain::Words& words) const noexcept
        {
            size_t hash = 0;
            
            for (const auto& word : words)
            {
                const size_t size = word.size();
                for (register size_t i = 0; i < size; ++i)
                {
                    hash ^= (hash << 5) + (hash >> 2) + word[i];
                }
                hash ^= (hash << 5) + (hash >> 2) + ' ';
            }
            
            return hash;
        }
    };
}


/// @class InnerChain
/// @brief Внутреняя цепь текстовой цепи Маркова.
struct MarkovTextChain::InnerChain
{
    /// @brief Конструктор.
    InnerChain()
        : m_Map()
    {
    }
    
    /// @brief Таблица состояний цепи.
    std::unordered_map<MarkovTextChain::Words, WordsKeeper, WordsHash> m_Map;
};


const std::string MarkovTextChain::m_ChainHeader = "MARKOV_TEXT_CHAIN_BEGIN";
const std::string MarkovTextChain::m_ChainTrailer = "MARKOV_TEXT_CHAIN_END";
const std::string MarkovTextChain::m_Delimiter = "->";

    
MarkovTextChain::MarkovTextChain(size_t chainOrder)
    : m_Order(chainOrder)
    , m_CurrentWords()
    , m_Chain(new InnerChain)
{
    srand(time(nullptr));
}

MarkovTextChain::~MarkovTextChain() = default;

void MarkovTextChain::setOrder(size_t chainOrder)
{
    m_Order = chainOrder;
}

size_t MarkovTextChain::order() const
{
    return m_Order;
}

void MarkovTextChain::load(std::istream& input)
{
    try
    {
        // Найти заголовок в потоке
        std::string buffer;
        while (buffer != m_ChainHeader && input.good())
        {
            input >> buffer;
        }
        
        if (!input.good())
        {
            throw std::runtime_error("MarkovTextChain::load error: input stream is not good");
        }
        
        // Считать порядок цепи.
        input >> m_Order;
        if (m_Order == 0)
        {
            throw std::runtime_error("MarkovTextChain::load error: inadmissible chain order");
        }
        
        // Считать размер таблицы состояний.
        size_t buckets;
        input >> buckets;
        m_Chain->m_Map.reserve(buckets);
        
        // После оператора ввода getline вернет ненужную пустую строку.
        std::getline(input, buffer);
        
        parseChainStates(input);
    }
    catch (const std::exception&)
    {
        reset();
        throw;
    }
}

void MarkovTextChain::save(std::ostream& output) const
{
    if (m_Order == 0)
    {
        throw std::logic_error("MarkovTextChain::save error: inadmissible chain order");
    }
    
    output << m_ChainHeader << std::endl;
    output << m_Order << std::endl;
    output << m_Chain->m_Map.bucket_count() << std::endl;
    
    for (const auto& pair : m_Chain->m_Map)
    {
        for (const auto& word : pair.first)
        {
            output << word << ' ';
        }
        output << m_Delimiter << ' ';
        output << pair.second.toString() << std::endl;
    }
    
    output << m_ChainTrailer << std::endl;
}

void MarkovTextChain::addWord(Word&& word)
{
    if (m_Order == 0)
    {
        throw std::logic_error("MarkovTextChain::addWord error: inadmissible chain order");
    }
    
    if (m_CurrentWords.size() < m_Order)
    {
        m_CurrentWords.push_back(std::move(word));
        return;
    }
    
    m_Chain->m_Map[m_CurrentWords].addWord(word);
    m_CurrentWords.pop_front();
    m_CurrentWords.push_back(std::move(word));
}

MarkovTextChain::Word MarkovTextChain::generateWord(const Words& words) const
{
    if (m_Order == 0)
    {
        throw std::logic_error("MarkovTextChain::generateWord error: inadmissible chain order");
    }
    
    try
    {
        return m_Chain->m_Map.at(words).getWord();
    }
    catch (const std::out_of_range&)
    {
        throw std::logic_error("MarkovTextChain::generateWord error: cannot generate word");
    }
}

void MarkovTextChain::flush()
{
    m_CurrentWords.clear();
}

void MarkovTextChain::parseChainStates(std::istream& input)
{
    std::string tmp;
    Words key;
    int totalWords = 0;
    
    while (!input.eof())
    {
        // До разделителя считать слова ключа таблицы состояний.
        while (!input.eof())
        {
            input >> tmp;
            if (tmp == m_Delimiter)
            {
                break;
            }
            if (tmp == m_ChainTrailer)
            {
                return;
            }
            if (!tmp.empty())
            {
                key.push_back(std::move(tmp));
            }
        }
        
        if (key.size() != m_Order)
        {
            throw std::logic_error("MarkovTextChain::parseChainString error: chain string has wrong order");
        }
        
        WordsKeeper& value = m_Chain->m_Map[std::move(key)];
        
        input >> totalWords;
        // После разделителя считать слова значения таблицы состояний.
        while (!input.eof() && --totalWords >= 0)
        {
            input >> tmp;
            if (!tmp.empty())
            {
                value.addWord(std::move(tmp));
            }
        }
        
        if (value.empty())
        {
            throw std::logic_error("MarkovTextChain::parseChainString error: chain string has no value");
        }
    }
    
    throw std::runtime_error("MarkovTextChain::load error: input stream is not good");
}

void MarkovTextChain::reset()
{
    m_Order = 0;
    m_CurrentWords.clear();
    m_Chain->m_Map.clear();
}
