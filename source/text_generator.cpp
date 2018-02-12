#include "text_generator.h"

#include <getopt.h>

#include <iostream>
#include <fstream>


namespace
{
    /// @brief Число требуемых слов по умолчанию.
    const size_t defaultNumber = 0;
    
    /// @brief Число слов в строке при выводе.
    const int wordsPerLine = 10;
}

TextGenerator::TextGenerator()
    : m_NumberOfNewWords(defaultNumber)
    , m_Input()
    , m_InitialWords()
    , m_NeedHelp(false)
    , m_ProgramName()
{
}

TextGenerator::~TextGenerator() = default;

void TextGenerator::init(int argc, char** argv)
{
    m_ProgramName = argv[0];
    m_ProgramName = m_ProgramName.substr(m_ProgramName.find_last_of('/') + 1);
    
    struct option longOptions[] =
    {
       {"words", required_argument, 0, 'w'},
       {"input", required_argument, 0, 'i'},
       {"help", no_argument, 0, 'h'},
       {0, 0, 0, 0 }
    };
    
    int c = 0;
    opterr = 0;
    
    // Анализ ключей и их значений.
    while ((c = getopt_long(argc, argv, "w:i:", longOptions, nullptr))!= -1)
    {
        switch (c)
        {
        case 'w':
            try
            {
                m_NumberOfNewWords = std::stoi(optarg);
                if (m_NumberOfNewWords <= 0)
                {
                    throw std::exception();
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "  Unsupported value for 'words' parameter" << std::endl;
                m_NumberOfNewWords = defaultNumber;
                break;
            }
            break;
            
        case 'i':
            m_Input = optarg;
            break;
            
        case 'h':
            m_NeedHelp = true;
            break;
            
        case '?':
            if (optopt == 'w')
            {
                std::cerr << "  Options -w and --words require an argument" << std::endl;
                m_NeedHelp = true;
                break;
            }
            else if (optopt == 'i')
            {
                std::cerr << " Options -i and --input require an argument" << std::endl;
                m_NeedHelp = true;
                break;
            }
            else
            {
                std::cerr << " Unknown option " << argv[optind-1] << std::endl;
                m_NeedHelp = true;
                break;
            }
            break;
        }
    }
    
    // Все, что без ключа, считается начальным словом.
    for (register int i = optind; i < argc; ++i)
    {
        m_InitialWords.push_back(argv[i]);
    }
    
    // Проверка наличия обязательных параметров.
    if (m_NumberOfNewWords == defaultNumber)
    {
        std::cerr << "  Number of new words to generate is not set" << std::endl;
        m_NeedHelp = true;
    }
    if (m_InitialWords.empty())
    {
        std::cerr << " No initial words are provided" << std::endl;
        m_NeedHelp = true;
    }
}

bool TextGenerator::run()
{
    return m_NeedHelp ? !printUsage() : generateText();
}

bool TextGenerator::printUsage() const
{
    std::cout << "Usage: " << m_ProgramName << " [options] [initial words]" << std::endl;
    std::cout << "  -w, --words    Number of new words to generate, must be positive" << std::endl;
    std::cout << "  -i, --input    File to load Markov chain from, std::cin will be used if not provided" << std::endl;
    std::cout << "  -h, --help     Show this message and exit" << std::endl << std::endl;
    return true;
}

bool TextGenerator::generateText()
{
    // Загрузить цепь Маркова.
    MarkovTextChain chain;
    if (!loadChain(chain))
    {
        return false;
    }
    
    // Проверить, достаточно ли начальных слов.
    if (!checkInitialWords(chain.order()))
    {
        return false;
    }
    
    // Попытаться сгенерировать требуемое число слов.
    try
    {
        for (register int i = 1; i <= m_NumberOfNewWords; ++i)
        {
            MarkovTextChain::Word newWord = chain.generateWord(m_InitialWords);
            std::cout << newWord << (i % wordsPerLine == 0 ? '\n' : ' ');
            
            m_InitialWords.pop_front();
            m_InitialWords.push_back(std::move(newWord));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << std::endl << "  TextGenerator::generateText cannot generate next word:\n    " << e.what() << std::endl;
        return false;
    }
    
    std::cout << std::endl;
    return true;
}

bool TextGenerator::loadChain(MarkovTextChain& chain) const
{
    if (!m_Input.empty())
    {
        std::cerr << "Loading Markov chain from '" << m_Input << "' ... ";
    }
    
    std::ifstream fileInput;
    if (!m_Input.empty())
    {
        fileInput.open(m_Input);
        if (!fileInput.good())
        {
            std::cerr << std::endl << "  TextGenerator::loadChain error: failed to open file '" << m_Input << "' for reading" << std::endl;
            return false;
        }
    }
    
    try
    {
        chain.load(m_Input.empty() ? std::cin : fileInput);
    }
    catch (const std::exception& e)
    {
        std::cerr << std::endl << "  TextGenerator::loadChain error:\n    " << e.what() << std::endl;
        return false;
    }
    
    if (!m_Input.empty())
    {
        std::cerr << "DONE" << std::endl << std::endl;
    }
    
    return true;
}

bool TextGenerator::checkInitialWords(size_t chainOrder)
{
    if (m_InitialWords.size() < chainOrder)
    {
        std::cerr << "  Provided chain order is " << chainOrder << ", number of initial words is too small" << std::endl;
        return false;
    }

    int counter = 1;
    for (const auto& initialWord : m_InitialWords)
    {
        std::cout << initialWord << (counter++ % wordsPerLine == 0 ? '\n' : ' ');
    }
    std::cout <<  std::endl;
    
    while (m_InitialWords.size() > chainOrder)
    {
        m_InitialWords.pop_front();
    }
    
    return true;
}
