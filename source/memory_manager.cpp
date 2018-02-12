#include <algorithm>
#include <cstdlib>
#include <new>


namespace
{
    /// @brief Минимальный размер выделяемой области памяти.
    constexpr size_t memoryChunkSize = 20 * 1024 * 1024;
    
    /// @brief Максимальное количество выделенных областей памяти.
    constexpr size_t poolCapacity = 1024;
    
    /// @class MemoryManager
    /// @brief Простой менеджер памяти.
    class MemoryManager
    {
    public:
        /// @brief Конструктор.
        MemoryManager()
            : m_Pool()
            , m_Chunk(-1)
            , m_ChunkSize(memoryChunkSize)
            , m_Position(memoryChunkSize)
        {
        }
        
        /// @brief Конструктор копирования.
        MemoryManager(const MemoryManager&) = delete;
        
        /// @brief Оператор присваивания.
        MemoryManager& operator=(const MemoryManager&) = delete;
        
        /// @brief Деструктор.
        ~MemoryManager()
        {
            for (register size_t i = 0; i <= m_Chunk; ++i)
            {
                free(m_Pool[i]);
            }
        }
        
        /// @brief Выделить требуемый объем памяти.
        /// @param[in] size - Размер памяти.
        /// @throws std::bad_alloc в случае ошибки.
        void* getMemory(size_t size)
        {
            if (m_Position + size > m_ChunkSize)
            {
                if (++m_Chunk >= poolCapacity)
                {
                    throw std::bad_alloc();
                }
                
                m_ChunkSize = std::max(memoryChunkSize, size);
                m_Pool[m_Chunk] = reinterpret_cast<char*>(malloc(m_ChunkSize));
                if (m_Pool[m_Chunk] == nullptr)
                {
                    throw std::bad_alloc();
                }
                
                m_Position = 0;
            }
            
            void* result = reinterpret_cast<void*>(m_Pool[m_Chunk] + m_Position);
            m_Position += size;
            return result;
        }
        
    private:
        /// @brief Массив указателей на выделенные области памяти.
        char* m_Pool[poolCapacity];
        
        /// @brief Номер текущей области памяти.
        size_t m_Chunk;
        
        /// @brief Размер текущей области памяти.
        size_t m_ChunkSize;
        
        /// @brief Смещение в текущей области памяти.
        size_t m_Position;
    };
}

/// @brief Переопределенный оператор.
void* operator new(size_t size)
{
    static MemoryManager mManager;
    return mManager.getMemory(size);
}

/// @brief Переопределенный оператор.
void operator delete(void*) throw()
{
}