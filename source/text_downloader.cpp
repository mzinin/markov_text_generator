#include "text_downloader.h"

#include <errno.h>

#include <cstring>
#include <exception>
#include <vector>


namespace
{
    /// @brief Внешняя утилита для загрузки данных.
    const std::string externalUtil = "curl";
    
    /// @brief Суффикс консольных команд для скрытия служебной информации.
    const std::string silentSuffix = " 2> /dev/null";
    
    /// @brief Размер буфера для чтения из канала.
    constexpr size_t bufferSize = 4096;
}

TextDownloader::TextDownloader()
    : m_Handler()
{
}

TextDownloader::~TextDownloader() = default;

void TextDownloader::setHandler(Handler handler)
{
    m_Handler = handler;
}

void TextDownloader::download(const std::string& url) const
{
    if (!m_Handler)
    {
        throw std::logic_error("TextDownloader::download error: no handler is set");
    }
    
    auto pipe = openPipe(url);
    std::vector<char> buffer(bufferSize, '\0');
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        m_Handler(buffer.data(), strlen(buffer.data()));
    }
    
    if (ferror(pipe.get()))
    {
        throw std::runtime_error("TextDownloader::download error: pipe is broken");
    }
    
    FILE* rawPipe = pipe.release();
    const int closeStatus = pclose(rawPipe);
    if (WEXITSTATUS(closeStatus) != 0)
    {
        throw std::runtime_error("TextDownloader::download error: failed to run utility '" + externalUtil + "'");
    }
}

TextDownloader::PipePtr TextDownloader::openPipe(const std::string& url) const
{
    const std::string command = externalUtil + " \"" + url + "\"" + silentSuffix;
    
    errno = 0;
    FILE* rawPipe = popen(command.c_str(), "r");
    if (rawPipe == nullptr)
    {
        throw std::runtime_error(std::string("TextDownloader::openPipe error: ") + strerror(errno));
    }
    
    return PipePtr(rawPipe, pclose);
}
