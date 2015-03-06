#include "stdio.h"
#include <windows.h>
#include "fs.h"
#include "kernel_file.h"

char KernelFile::write(BytesCnt cnt, char* buffer)
{
    // TODO
    return 0;
}

BytesCnt KernelFile::read(BytesCnt cnt, char* buffer)
{
    // TODO
	// uzmi poziciju caret-a i odredi dokle treba da cita
	// za svaki klaster iz kog treba dohvatiti podatke, procitaj deo koji treba iz tog klastera
	BytesCnt toRead = m_entry.size - m_caret < cnt ? m_entry.size - m_caret : cnt;
	BytesCnt leftToRead = toRead;
	char* data = new char[toRead];

	while (leftToRead>0)
	{
		// TODO: some black magic shit
	}

	memcpy(buffer, data, toRead);//shit no more

    return toRead;
}

char KernelFile::seek(BytesCnt cnt)
{
    if (m_entry.size < cnt)
        return 0;

    m_caret = cnt;
    return 1;
}

BytesCnt KernelFile::filePos()
{
    return m_caret;
}

char KernelFile::eof()
{
    return m_caret == m_entry.size;
}

BytesCnt KernelFile::getFileSize()
{
    return m_entry.size;
}

char KernelFile::truncate()
{
    // TODO
    return 0;
}

KernelFile::~KernelFile()
{
    // TODO
}

// private

KernelFile::KernelFile()
{
    // TODO
}