#include "markov_text_chain.h"
#include "text_adjuster.h"
#include "text_downloader.h"
#include "word_splitter.h"

#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>


// Common values
namespace
{
    std::string testDataDir = "../test_data/";
    const std::string textDownoaderUrl = "https://dl.pushbulletusercontent.com/qLE2ofZ55IVCUsKatIam9QRO6X7CynGf/Alice_rus.txt";
    
    bool CompareFiles(const std::string& file1, const std::string& file2)
    {
        const size_t bufferSize = 1024 * 1024;
        
        std::ifstream stream1(file1, std::ifstream::in | std::ifstream::binary);
        std::ifstream stream2(file2, std::ifstream::in | std::ifstream::binary);

        if (!stream1.is_open())
        {
            std::cerr << "\n  TestTextDownloader: failed to open file '" + file1 + "' for reading" << std::endl;
            return false;
        }
        if (!stream2.is_open())
        {
            std::cerr << "\n  TestTextDownloader: failed to open file '" + file2 + "' for reading" << std::endl;
            return false;
        }

        std::vector< char > buffer1(bufferSize);
        std::vector< char > buffer2(bufferSize);

        do
        {
            stream1.read(buffer1.data(), buffer1.size());
            stream2.read(buffer2.data(), buffer2.size());

            if (memcmp(buffer1.data(), buffer2.data(), buffer1.size()) != 0)
            {
                std::cerr << "\n  TestTextDownloader: files '" + file1 + "' and '" << file2 << "' are different" << std::endl;
                return false;
            }
        } while (stream1.good() || stream2.good());

        return true;
    }
}

// TextDownloader test
namespace
{
    const std::string textDownloaderOutput = "text_downloader_output.txt";
    const std::string textDownloaderModel = "text_downloader_model.txt";
    
    void TextDownloaderHandler(std::ostream& out, const char* data, size_t size)
    {
        out << std::string(data, size);
    }
    
    bool TextDownloaderTest()
    {
        std::ofstream output(textDownloaderOutput);
        if (!output.good())
        {
            std::cerr << "  TextDownloaderTest: failed to open file '" << textDownloaderOutput << "' for reading" << std::endl;
            return false;
        }
        
        TextDownloader downloader;
        downloader.setHandler(std::bind(TextDownloaderHandler, std::ref(output), std::placeholders::_1, std::placeholders::_2));
        
        try
        {
            downloader.download(textDownoaderUrl);
        }
        catch (const std::exception& e)
        {
            std::cerr << "\n  TextDownloaderTest: failed to download url '" << textDownoaderUrl << "':\n    " << e.what() << std::endl;
            output.close();
            return false;
        }
        
        output.close();
        
        return CompareFiles(textDownloaderOutput, testDataDir + textDownloaderModel);
    }
}

// WordSplitter test
namespace
{
    const std::string wordSplitterOutput = "word_splitter_output.txt";
    const std::string wordSplitterModel = "word_splitter_model.txt";
    
    void WordSplitterHandler(std::ostream& out, std::string&& word)
    {
        out << word << std::endl;
    }
    
    bool WordSplitterTest()
    {
        std::ofstream output(wordSplitterOutput);
        if (!output.good())
        {
            std::cerr << "  WordSplitterTest: failed to open file '" << wordSplitterOutput << "' for reading" << std::endl;
            return false;
        }
        
        WordSplitter splitter;
        splitter.setHandler(std::bind(WordSplitterHandler, std::ref(output), std::placeholders::_1));
        
        TextDownloader downloader;
        downloader.setHandler(std::bind(&WordSplitter::addText, std::ref(splitter), std::placeholders::_1, std::placeholders::_2));
        
        try
        {
            downloader.download(textDownoaderUrl);
        }
        catch (const std::exception& e)
        {
            std::cerr << "\n  WordSplitterTest: failed to download url '" << textDownoaderUrl << "':\n    " << e.what() << std::endl;
            output.close();
            return false;
        }
        
        splitter.flush();
        output.close();
        
        return CompareFiles(wordSplitterOutput, testDataDir + wordSplitterModel);
    }
}

// TextAdjuster test
namespace
{
    const std::string textAdjusterOutput = "text_adjuster_output.txt";
    const std::string textAdjusterModel = "text_adjuster_model.txt";
    
    void TextAdjusterHandler(std::ostream& out, std::string&& word)
    {
        out << word << std::endl;
    }
    
    bool TextAdjusterTest()
    {
        std::ofstream output(textAdjusterOutput);
        if (!output.good())
        {
            std::cerr << "  TextAdjusterTest: failed to open file '" << textAdjusterOutput << "' for reading" << std::endl;
            return false;
        }
        
        TextAdjuster adjuster;
        adjuster.setHandler(std::bind(TextAdjusterHandler, std::ref(output), std::placeholders::_1));
        
        WordSplitter splitter;
        splitter.setHandler(std::bind(&TextAdjuster::adjust, std::ref(adjuster), std::placeholders::_1));
        
        TextDownloader downloader;
        downloader.setHandler(std::bind(&WordSplitter::addText, std::ref(splitter), std::placeholders::_1, std::placeholders::_2));
        
        try
        {
            downloader.download(textDownoaderUrl);
        }
        catch (const std::exception& e)
        {
            std::cerr << "\n  TestWordSplitter: failed to download url '" << textDownoaderUrl << "':\n    " << e.what() << std::endl;
            output.close();
            return false;
        }
        
        splitter.flush();
        output.close();
        
        return CompareFiles(textAdjusterOutput, testDataDir + textAdjusterModel);
    }
}

// MarkovTextChain test
namespace
{
    const std::string textChainOutput = "text_chain_output.txt";
    const std::string textChainModel = "text_chain_model.txt";
    const size_t chainOrder = 5;
    
    bool MarkovTextChainBuildTest()
    {
        MarkovTextChain chain(chainOrder);
        
        TextAdjuster adjuster;
        adjuster.setHandler(std::bind(&MarkovTextChain::addWord, std::ref(chain), std::placeholders::_1));
        
        WordSplitter splitter;
        splitter.setHandler(std::bind(&TextAdjuster::adjust, std::ref(adjuster), std::placeholders::_1));
        
        TextDownloader downloader;
        downloader.setHandler(std::bind(&WordSplitter::addText, std::ref(splitter), std::placeholders::_1, std::placeholders::_2));
        
        try
        {
            downloader.download(textDownoaderUrl);
        }
        catch (const std::exception& e)
        {
            std::cerr << "\n  MarkovTextChainBuildTest: failed to download url '" << textDownoaderUrl << "':\n    " << e.what() << std::endl;
            return false;
        }
        
        splitter.flush();
        
        std::ofstream output(textChainOutput);
        if (!output.good())
        {
            std::cerr << "  MarkovTextChainBuildTest: failed to open file '" << textChainOutput << "' for writing" << std::endl;
            return false;
        }
        
        chain.save(output);
        output.close();
        
        return CompareFiles(textChainOutput, testDataDir + textChainModel);
    }
    
    bool MarkovTextChainLoadTest()
    {
        std::ifstream input(testDataDir + textChainModel);
        if (!input.good())
        {
            std::cerr << "  MarkovTextChainLoadTest: failed to open file '" << testDataDir << textChainModel << "' for reading" << std::endl;
            return false;
        }
        
        MarkovTextChain chain;
        try
        {
            chain.load(input);
        }
        catch (const std::exception& e)
        {
            std::cerr << "  MarkovTextChainLoadTest: failed to load Markov Text Chain: " << e.what() << std::endl;
            input.close();
            return false;
        }
        input.close();
        
        std::ofstream output(textChainOutput);
        if (!output.good())
        {
            std::cerr << "  MarkovTextChainLoadTest: failed to open file '" << textChainOutput + "' for writing" << std::endl;
            return false;
        }
        
        chain.save(output);
        output.close();
        
        return true;
    }
}

#define RUN_TEST(test) \
    std::cout << "Running test " << #test << " ... " << std::flush; \
    std::cout << (test() ? "OK" : "FAIL") << std::endl << std::endl;

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        testDataDir = argv[1];
    }
    
    RUN_TEST(TextDownloaderTest);
    RUN_TEST(WordSplitterTest);
    RUN_TEST(TextAdjusterTest);
    RUN_TEST(MarkovTextChainBuildTest);
    RUN_TEST(MarkovTextChainLoadTest);
    
    return 0;
}
