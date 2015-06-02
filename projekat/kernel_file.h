class KernelFile
{
public:
    char write(BytesCnt, char* buffer);
    BytesCnt read(BytesCnt, char* buffer);
    char seek(BytesCnt);

    BytesCnt filePos();
    char eof();
    BytesCnt getFileSize();
    char truncate();
    ~KernelFile();
private:

    Entry m_entry;
    unsigned long m_caret;

    // lista pokazivaca na FIFO niti
    // trenutni modalitet


    friend class FS;
    friend class KernelFS;
    KernelFile();
};