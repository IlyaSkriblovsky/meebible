#pragma once

#include <QString>
#include <QFile>
#include <QList>
#include <QPair>


namespace appendstorage
{


template <typename EntryHeader>
struct TableEntry
{
    unsigned int address;
    unsigned int size;

    EntryHeader header;
} __attribute__((packed));


template <typename EntryHeader>
class AppendStorage
{
    public:
        AppendStorage(const QString& basename)
        {
            _fData.setFileName(basename + ".asd");
            _fData.open(QIODevice::ReadWrite);

            _fTable.setFileName(basename + ".ast");
            _fTable.open(QIODevice::ReadWrite);

            if (_fTable.size() < 4)
            {
                _fTable.resize(0);
                _fTable.resize(4);
                _entryCount = 0;
            }
            else
            {
                _fTable.read((char*)&_entryCount, 4);
                _fTable.resize(4 + sizeof(TableEntry<EntryHeader>) * _entryCount);
            }
        }

        ~AppendStorage()
        {
        }


        unsigned int entryCount() { return _entryCount; }



        void clear()
        {
            _entryCount = 0;

            _fData.seek(0);
            _fData.resize(0);
            _fData.flush();

            _fTable.seek(0);
            _fTable.resize(0);
            _fTable.resize(4);
            _fTable.flush();
        }



        void bulkSave(const QList<QPair<EntryHeader, QByteArray> >& data)
        {
            TableEntry<EntryHeader> entries[data.size()];

            _fData.seek(_fData.size());

            for (int i = 0; i < data.size(); i++)
            {
                entries[i].address = (unsigned int)_fData.pos();
                entries[i].size = data[i].second.size();
                entries[i].header = data[i].first;

                _fData.write(data[i].second);
            }

            _fData.flush();

            _fTable.seek(_fTable.size());
            _fTable.write((const char*)entries, sizeof(entries));

            _entryCount += data.size();

            _fTable.seek(0);
            _fTable.write((const char*)&_entryCount, 4);
            _fTable.flush();
        }


        void save(const EntryHeader& header, const QByteArray& data)
        {
            TableEntry<EntryHeader> entry;
            entry.header = header;
            entry.size = data.size();

            _fData.seek(_fData.size());
            entry.address = (unsigned int)_fData.pos();
            _fData.write(data);
            _fData.flush();


            _fTable.seek(_fTable.size());
            _fTable.write((const char*)&entry, sizeof(entry));

            _entryCount++;

            _fTable.seek(0);
            _fTable.write((const char*)&_entryCount, 4);
            _fTable.flush();
        }


        QByteArray load(const EntryHeader& header)
        {
            const int bufCapacity = 100;

            TableEntry<EntryHeader> buffer[bufCapacity];

            _fTable.seek(4);

            int entriesRead;
            do
            {
                int bufSize = _fTable.read((char*)&buffer, sizeof(buffer[0]) * bufCapacity);
                entriesRead = bufSize / sizeof(buffer[0]);

                for (int i = 0; i < entriesRead; i++)
                    if (buffer[i].header == header)
                    {
                        _fData.seek(buffer[i].address);
                        return _fData.read(buffer[i].size);
                    }

            } while (entriesRead == bufCapacity);

            return QByteArray();
        }



        void getAllHeaders(EntryHeader *headers)
        {
            const int bufCapacity = 100;

            TableEntry<EntryHeader> buffer[bufCapacity];

            EntryHeader *current = headers;

            _fTable.seek(4);

            int entriesRead;
            do
            {
                int bufSize = _fTable.read((char*)&buffer, sizeof(buffer[0]) * bufCapacity);
                entriesRead = bufSize / sizeof(buffer[0]);

                for (int i = 0; i < entriesRead; i++)
                    *current++ = buffer[i].header;
            } while (entriesRead == bufCapacity);
        }



    private:
        QFile _fData;
        QFile _fTable;


        unsigned int _entryCount;
};

}
