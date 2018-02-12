#pragma once

#ifndef TEXT_DOWNLOADER_H
#define TEXT_DOWNLOADER_H

#include <cstdio>
#include <functional>
#include <memory>


/// @class TextDownloader
/// @brief Загружает текст с указанного адреса.
class TextDownloader
{
public:
    /// @brief Тип обработчика полученных данных.
    using Handler = std::function<void(const char*, size_t)>;
    
public:
    /// @brief Конструктор.
    TextDownloader();
    
    /// @brief Деструктор.
    ~TextDownloader();
    
    /// @brief Установить обработчик полученных данных.
    /// @param[in] handler - Новый обработчик.
    void setHandler(Handler handler);
    
    /// @brief Загрузить текст с указанного адреса.
    /// @param[in] url - Удаленный адрес.
    /// @throws std::exception в случае ошибки.
    void download(const std::string& url) const;
    
private:
    /// @brief Тип для безопасного хранения указателя на FILE.
    using PipePtr = std::unique_ptr<FILE, int(*)(FILE*)>;
    
    /// @brief Открыть канал внешней утилиты для загрузки данных.
    /// @param[in] url - Удаленный адрес.
    /// @return Указатель на открытый канал.
    /// @throws std::exception в случае ошибки.
    PipePtr openPipe(const std::string& url) const;

private:
    /// @brief Обработчик полученных данных.
    Handler m_Handler;
};

#endif // TEXT_DOWNLOADER_H
