#include "chain_builder.h"
#include "text_adjuster.h"
#include "text_downloader.h"
#include "word_splitter.h"

#include <getopt.h>

#include <iostream>
#include <fstream>
#include <functional>


namespace
{
    /// @brief Порядок цепи Маркова по умолчанию.
    const size_t defaultOrder = 0;
}

ChainBuilder::ChainBuilder()
    : m_Order(defaultOrder)
    , m_Output()
    , m_Urls()
    , m_NeedHelp(false)
    , m_ProgramName()
{
}

ChainBuilder::~ChainBuilder() = default;

void ChainBuilder::init(int argc, char** argv)
{
    m_ProgramName = argv[0];
    m_ProgramName = m_ProgramName.substr(m_ProgramName.find_last_of('/') + 1);
    
    struct option longOptions[] =
    {
       {"order", required_argument, 0, 'n'},
       {"output", required_argument, 0, 'o'},
       {"help", no_argument, 0, 'h'},
       {0, 0, 0, 0 }
    };
    
    int c = 0;
    opterr = 0;
    
    // Анализ ключей и их значений.
    while ((c = getopt_long(argc, argv, "n:o:", longOptions, nullptr))!= -1)
    {
        switch (c)
        {
        case 'n':
            try
            {
                m_Order = std::stoi(optarg);
                if (m_Order <= 0)
                {
                    throw std::exception();
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "  Unsupported value for 'order' parameter" << std::endl;
                m_Order = defaultOrder;
                break;
            }
            break;
            
        case 'o':
            m_Output = optarg;
            break;
            
        case 'h':
            m_NeedHelp = true;
            break;
            
        case '?':
            if (optopt == 'n')
            {
                std::cerr << "  Options -n and --order require an argument" << std::endl;
                m_NeedHelp = true;
                break;
            }
            else if (optopt == 'o')
            {
                std::cerr << " Options -o and --output require an argument" << std::endl;
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
    
    // Все, что без ключа, считается адресом.
    for (register int i = optind; i < argc; ++i)
    {
        m_Urls.push_back(argv[i]);
    }
    
    // Проверка наличия обязательных параметров.
    if (m_Order == defaultOrder)
    {
        std::cerr << "  Chain order is not set" << std::endl;
        m_NeedHelp = true;
    }
    if (m_Urls.empty())
    {
        std::cerr << " No url is provided" << std::endl;
        m_NeedHelp = true;
    }
}

bool ChainBuilder::run() const
{
    return m_NeedHelp ? !printUsage() : buildChain();
}

bool ChainBuilder::printUsage() const
{
    std::cout << "Usage: " << m_ProgramName << " [options] [urls]" << std::endl;
    std::cout << "  -n, --order    Markov chain order, must be positive" << std::endl;
    std::cout << "  -o, --output   File to output Markov chain to, std::cout will be used if not provided" << std::endl;
    std::cout << "  -h, --help     Show this message and exit" << std::endl << std::endl;
    return true;
}

bool ChainBuilder::buildChain() const
{
    MarkovTextChain chain(m_Order);
    
    TextAdjuster adjuster;
    adjuster.setHandler(std::bind(&MarkovTextChain::addWord, std::ref(chain), std::placeholders::_1));
    
    WordSplitter splitter;
    splitter.setHandler(std::bind(&TextAdjuster::adjust, std::ref(adjuster), std::placeholders::_1));
    
    TextDownloader downloader;
    downloader.setHandler(std::bind(&WordSplitter::addText, std::ref(splitter), std::placeholders::_1, std::placeholders::_2));
    
    try
    {
        for (const auto& url : m_Urls)
        {
            std::cerr << "Processing '" << url << "' ... " << std::flush;
            downloader.download(url);
            splitter.flush();
            chain.flush();
            std::cerr << "DONE" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << std::endl << "  ChainBuilder::buildChain processing error:\n    " << e.what() << std::endl;
        return false;
    }
    
    return outputChain(chain);
}

bool ChainBuilder::outputChain(const MarkovTextChain& chain) const
{
    if (!m_Output.empty())
    {
        std::cerr << "Saving Markov chain to '" << m_Output << "' ... ";
    }
    
    std::ofstream fileOutput;
    if (!m_Output.empty())
    {
        fileOutput.open(m_Output);
        if (!fileOutput.good())
        {
            std::cerr << std::endl << "  ChainBuilder::outputChain error: failed to open file '" << m_Output << "' for writing" << std::endl;
            return false;
        }
    }
    
    try
    {
        chain.save(m_Output.empty() ? std::cout : fileOutput);
    }
    catch (const std::exception& e)
    {
        std::cerr << std::endl << "  ChainBuilder::outputChain error:\n    " << e.what() << std::endl;
        return false;
    }
    
    if (!m_Output.empty())
    {
        std::cerr << "DONE" << std::endl;
    }
    
    return true;
}
