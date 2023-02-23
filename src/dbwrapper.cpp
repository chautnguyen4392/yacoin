// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#ifdef _MSC_VER
#include <stdint.h>

#include "msvc_warnings.push.h"
#endif

#ifndef BITCOIN_TXDB_H
#include "txdb.h"
#endif

#include <map>
#include <boost/version.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <leveldb/env.h>
#include <leveldb/cache.h>
#include <leveldb/filter_policy.h>
#include <memenv/memenv.h>

using std::make_pair;
using std::map;
using std::pair;
using std::runtime_error;
using std::string;
using std::vector;

using namespace boost;

leveldb::DB *txdb[DB_TYPE_MAX]; // global pointer for LevelDB object instance

static leveldb::Options GetOptions()
{
    leveldb::Options options;
    int nCacheSizeMB = GetArg("-dbcache", 25);
    options.block_cache = leveldb::NewLRUCache(nCacheSizeMB * 1048576);
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);
    return options;
}

void init_blockindex(DatabaseType dbType, leveldb::Options &options, bool fRemoveOld = false)
{
    // First time init.
	filesystem::path directory;
	switch (dbType) {
	case BLOCK_INDEX:
	    directory = GetDataDir() / "txleveldb";
	case ASSET_DATA:
	    directory = GetDataDir() / "assets";
	default:
		printf("Unknown database type = %d\n", dbType);
	}

    if (fRemoveOld)
    {
        filesystem::remove_all(directory); // remove directory
        unsigned int nFile = 1;

        while (true)
        {
            filesystem::path strBlockFile = GetDataDir() / strprintf("blk%04u.dat", nFile);

            // Break if no such file
            if (!filesystem::exists(strBlockFile))
                break;

            filesystem::remove(strBlockFile);

            nFile++;
        }
    }

    filesystem::create_directory(directory);
    printf("Opening LevelDB in %s\n", directory.string().c_str());
    leveldb::Status status = leveldb::DB::Open(options, directory.string(), &txdb[dbType]);
    if (!status.ok())
    {
        throw runtime_error(strprintf("init_blockindex(): error opening database environment %s", status.ToString().c_str()));
    }
}

// CDB subclasses are created and destroyed VERY OFTEN. That's why
// we shouldn't treat this as a free operations.
CDBWrapper::CDBWrapper(DatabaseType dbType, const char *pszMode)
{
    Yassert(pszMode);
    activeBatch = NULL;
    fReadOnly = (!strchr(pszMode, '+') && !strchr(pszMode, 'w'));
    mDbType = dbType;

    if (txdb[mDbType])
    {
        pdb = txdb[mDbType];
        return;
    }

    bool fCreate = strchr(pszMode, 'c');

    options = GetOptions();
    options.create_if_missing = fCreate;
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);

    init_blockindex(dbType, options); // Init directory
    pdb = txdb[mDbType];

    if (Exists(string("version")))
    {
        ReadVersion(nVersion);
        printf("Transaction index version is %d\n", nVersion);

        if (nVersion < DATABASE_VERSION)
        {
            printf("Required index version is %d, removing old database\n", DATABASE_VERSION);

            // Leveldb instance destruction
            delete txdb[mDbType];
            txdb[mDbType] = pdb = NULL;
            delete activeBatch;
            activeBatch = NULL;

            init_blockindex(dbType, options, true); // Remove directory and create new database
            pdb = txdb[mDbType];

            bool fTmp = fReadOnly;
            fReadOnly = false;
            WriteVersion(DATABASE_VERSION); // Save transaction index version
            fReadOnly = fTmp;
        }
    }
    else if (fCreate)
    {
        bool fTmp = fReadOnly;
        fReadOnly = false;
        WriteVersion(DATABASE_VERSION);
        fReadOnly = fTmp;
    }

    printf("Opened LevelDB successfully\n");
}

CDBIterator* CDBWrapper::NewIterator()
{
    return new CDBIterator(*this, pdb->NewIterator(leveldb::ReadOptions()));
}

void CDBWrapper::Close()
{
    delete txdb[mDbType];
    txdb[mDbType] = pdb = NULL;
    delete options.filter_policy;
    options.filter_policy = NULL;
    delete options.block_cache;
    options.block_cache = NULL;
    delete activeBatch;
    activeBatch = NULL;
}

bool CDBWrapper::TxnBegin()
{
    Yassert(!activeBatch);
    activeBatch = new leveldb::WriteBatch();
    return true;
}

bool CDBWrapper::TxnCommit()
{
    Yassert(activeBatch);
    leveldb::Status status = pdb->Write(leveldb::WriteOptions(), activeBatch);
    delete activeBatch;
    activeBatch = NULL;
    if (!status.ok())
    {
        printf("LevelDB batch commit failure: %s\n", status.ToString().c_str());
        return false;
    }
    return true;
}

class CBatchScanner : public leveldb::WriteBatch::Handler
{
public:
    std::string needle;
    bool *deleted;
    std::string *foundValue;
    bool foundEntry;

    CBatchScanner() : foundEntry(false) {}

    virtual void Put(const leveldb::Slice &key, const leveldb::Slice &value)
    {
        if (key.ToString() == needle)
        {
            foundEntry = true;
            *deleted = false;
            *foundValue = value.ToString();
        }
    }

    virtual void Delete(const leveldb::Slice &key)
    {
        if (key.ToString() == needle)
        {
            foundEntry = true;
            *deleted = true;
        }
    }
};

// When performing a read, if we have an active batch we need to check it first
// before reading from the database, as the rest of the code assumes that once
// a database transaction begins reads are consistent with it. It would be good
// to change that assumption in future and avoid the performance hit, though in
// practice it does not appear to be large.
bool CDBWrapper::ScanBatch(const CDataStream &key, string *value, bool *deleted) const
{
    Yassert(activeBatch);
    *deleted = false;
    CBatchScanner scanner;
    scanner.needle = key.str();
    scanner.deleted = deleted;
    scanner.foundValue = value;
    leveldb::Status status = activeBatch->Iterate(&scanner);
    if (!status.ok())
    {
        throw runtime_error(status.ToString());
    }
    return scanner.foundEntry;
}

CDBIterator::~CDBIterator() { delete piter; }
bool CDBIterator::Valid() const { return piter->Valid(); }
void CDBIterator::SeekToFirst() { piter->SeekToFirst(); }
void CDBIterator::Next() { piter->Next(); }

#ifdef _MSC_VER
#include "msvc_warnings.pop.h"
#endif
