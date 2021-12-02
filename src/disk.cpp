#include "disk.h"

uint32_t const TABLE_SIZE = 100;
uint32_t const PAGE_SIZE = 128 * 1024;
size_t const STR_SIZE = 32;
size_t const INT_SIZE = 4;
void *table[PAGE_SIZE];
void *tablePtrs[TABLE_SIZE];
unordered_map<string, int> tableLoc;

char *myCSTR(string x)
{
    char *out = (char *)malloc(STR_SIZE);
    int s = x.size();
    for (int i = 0; i < min(s, 31); i++)
    {
        out[i] = x[i];
    }
    out[min(s, 31)] = '\0';
    // cout << out << endl;
    return out;
}

string addTable(string tableName, vector<pair<string, string>> &cols)
{
    int x;
    memcpy(&x, table, INT_SIZE);
    int s = cols.size();
    int metaSize = s * 64 + 4 + 32;
    memcpy((void *)(table + x), &metaSize, INT_SIZE);
    // int p;
    // cout << p << " ";
    // memcpy(&p, (void *)(table + x), INT_SIZE);
    // cout << p << endl;
    x += 4;
    memcpy((void *)(table + x), myCSTR(tableName), STR_SIZE);
    x += 32;
    for (int i = 0; i < s; i++)
    {
        // cout << cols[i].first << " " << cols[i].second << endl;
        memcpy((void *)(table + x), myCSTR(cols[i].first), STR_SIZE);
        memcpy((void *)(table + x + 32), myCSTR(cols[i].second), STR_SIZE);
        x += 64;
    }
    memcpy(table, &x, INT_SIZE);
    // Adding Table Loc in Memmory
    int ls = tableLoc.size(), k = 4;
    tableLoc[tableName] = ls;
    tablePtrs[ls] = malloc(PAGE_SIZE);
    memcpy(tablePtrs[ls], &k, INT_SIZE);
    // cout << ls << " " << tablePtrs[ls] << endl;
    // for (auto &x : tableLoc)
    // {
    //     cout << x.first << " " << x.second << endl;
    // }
    return "INSERTED";
}

string giveString(int loc)
{
    char *out = (char *)malloc(STR_SIZE);
    memcpy(out, (void *)(table + loc), STR_SIZE);
    string ret;
    ret += out;
    // cout << ret << endl;
    return ret;
}

vector<pair<string, string>> findTable(string name)
{
    int loc, fin, l;
    memcpy(&fin, table, INT_SIZE);
    // cout << fin << endl;
    string ref;
    vector<pair<string, string>> metadata;
    loc = 4;
    while (loc < fin)
    {
        // cout << l << " LOC " << loc << "\n";
        memcpy(&l, (void *)(table + loc), INT_SIZE);
        // cout << l << " LOC " << loc << endl;
        ref = giveString(loc + 4);
        // cout << ref << endl;
        if (ref != name)
        {
            loc += l;
            continue;
        }
        for (int x = loc + 4 + 32; x < loc + l; x += 64)
        {
            metadata.push_back({giveString(x), giveString(x + 32)});
        }
        return metadata;
    }
    return metadata;
}

string dropTable(string name)
{
    int loc, fin, l;
    memcpy(&fin, table, INT_SIZE);
    // cout << fin << endl;
    string ref;
    loc = 4;
    while (loc < fin)
    {
        // cout << l << " LOC " << loc << "\n";
        memcpy(&l, (void *)(table + loc), INT_SIZE);
        // cout << l << " LOC " << loc << endl;
        ref = giveString(loc + 4);
        if (ref != name)
        {
            loc += l;
            continue;
        }
        int p;
        memcpy(table + loc, table + loc + l, INT_SIZE);
        memcpy(&p, table + loc, INT_SIZE);
        for (int i = 0; i < p / STR_SIZE; i++)
            memcpy(table + loc + 4 + STR_SIZE * i, myCSTR(giveString(loc + l + 4 + STR_SIZE * i)), STR_SIZE);
        fin -= l;
        memcpy(table, &fin, INT_SIZE);
        int pos = tableLoc[ref];
        int mx = pos;
        for (auto &it : tableLoc)
        {
            if (tableLoc[it.first] > pos)
            {
                mx = max(tableLoc[it.first], mx);
                tableLoc[it.first]--;
            }
        }
        for (int j = pos + 1; j <= mx; j++)
            tablePtrs[j - 1] = tablePtrs[j];
        tableLoc.erase(ref);
        remove(ref.c_str());
        // cout << "AFTER DROP" << endl;
        // for (auto &x : tableLoc)
        // {
        //     cout << x.first << " " << x.second << endl;
        // }
        return "DROPPED";
    }
    return "NOT FOUND";
}

void flush(string fileName)
{
    int fd = open(fileName.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    write(fd, table, PAGE_SIZE);
    int xd;
    for (auto &it : tableLoc)
    {
        xd = open(it.first.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
        write(xd, tablePtrs[it.second], PAGE_SIZE);
    }
}

void load(string fileName)
{
    if (tableLoc.size())
        return;
    int fd = open(fileName.c_str(), O_RDWR, S_IWUSR | S_IRUSR);
    if (fd == -1)
    {
        int k = 4;
        memcpy(table, &k, INT_SIZE);
        return;
    }
    read(fd, table, PAGE_SIZE);
    int loc, fin, l, lc, xd;
    int x = 4, y;
    memcpy(&fin, table, INT_SIZE);
    string ref;
    loc = 4;
    while (loc < fin)
    {
        // cout << l << " LOC " << loc << "\n";
        memcpy(&l, (void *)(table + loc), INT_SIZE);
        // cout << l << " LOC " << loc << endl;
        ref = giveString(loc + 4);
        lc = tableLoc.size();
        if (tableLoc.find(ref) == tableLoc.end())
            tableLoc[ref] = lc;
        xd = open(ref.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
        tablePtrs[lc] = malloc(PAGE_SIZE);
        read(xd, tablePtrs[lc], PAGE_SIZE);
        loc += l;
    }
    // for (auto &x : tableLoc)
    // {
    //     cout << x.first << " " << x.second << endl;
    // }
}

string insertInto(string name, vector<string> data)
{
    if (tableLoc.find(name) == tableLoc.end())
        return "!!Table Not Found!!";
    vector<pair<string, string>> meta = findTable(name);
    if (meta.size() != data.size())
        return "!!Incorrectly formatted data!!";
    // cout << tableLoc[name] << " " << tablePtrs[tableLoc[name]] << endl;
    int size = meta.size();
    int loc, sz = 4, gg;
    memcpy(&loc, tablePtrs[tableLoc[name]], INT_SIZE);
    // cout << "LOC " << loc << endl;
    memcpy((void *)(tablePtrs[tableLoc[name]] + loc), &sz, INT_SIZE);
    for (int i = 0; i < size; i++)
    {
        // cout << i << " " << loc << " " << sz << endl;
        if (meta[i].second == "int")
        {
            gg = stoi(data[i]);
            // cout << gg << endl;
            memcpy((void *)(tablePtrs[tableLoc[name]] + loc + sz), &gg, INT_SIZE);
            sz += INT_SIZE;
        }
        else
        {
            memcpy((void *)(tablePtrs[tableLoc[name]] + loc + sz), myCSTR(data[i]), STR_SIZE);
            sz += STR_SIZE;
        }
    }
    memcpy((void *)(tablePtrs[tableLoc[name]] + loc), &sz, INT_SIZE);
    loc += sz;
    // cout << sz << " LOC " << loc << endl;
    memcpy((void *)(tablePtrs[tableLoc[name]]), &loc, INT_SIZE);
    return "INSERT SUCCESS";
}

vector<vector<string>> viewTable(string name)
{
    vector<vector<string>> out;
    if (tableLoc.find(name) == tableLoc.end())
        return out;
    vector<pair<string, string>> meta = findTable(name);
    int size = meta.size();
    char *str_temp = (char *)malloc(STR_SIZE);
    int loc, sz = 4, gg;
    // cout << tableLoc[name] << " " << tablePtrs[tableLoc[name]] << endl;
    memcpy(&loc, tablePtrs[tableLoc[name]], INT_SIZE);
    string x;
    int xx;
    vector<string> kk;
    for (int i = 0; i < size; i++)
        kk.push_back(meta[i].first);
    out.push_back(kk);

    while (sz < loc)
    {
        kk.clear();
        memcpy(&gg, table + sz, INT_SIZE);
        sz += 4;
        for (int i = 0; i < size; i++)
        {
            // cout << loc << " " << i << " " << sz << endl;
            if (meta[i].second == "int")
            {
                // cout << xx << " ";
                memcpy(&xx, tablePtrs[tableLoc[name]] + sz, INT_SIZE);
                kk.push_back(to_string(xx));
                // cout << "Here INT " << xx << endl;
                sz += INT_SIZE;
            }
            else
            {
                memcpy(str_temp, tablePtrs[tableLoc[name]] + sz, STR_SIZE);
                x += str_temp;
                kk.push_back(x);
                // cout << "Here STR" << i << " " << x << endl;
                x = "";
                sz += STR_SIZE;
            }
        }
        out.push_back(kk);
    }
    return out;
}

string takeSnap(string name)
{
    if (tableLoc.find(name) == tableLoc.end())
        return "!!Table Not Found!!";
    string snapName = name + "_snap";
    int xd = open(snapName.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    write(xd, tablePtrs[tableLoc[name]], PAGE_SIZE);
    return "SNAP TAKEN";
}

vector<vector<string>> showSnap(string name)
{
    void *tab = malloc(PAGE_SIZE);
    string snapName = name + "_snap";
    int xd = open(snapName.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    read(xd, tab, PAGE_SIZE);
    vector<vector<string>> out;
    vector<pair<string, string>> meta = findTable(name);
    int size = meta.size();
    char *str_temp = (char *)malloc(STR_SIZE);
    int loc, sz = 4, gg;
    memcpy(&loc, tab, INT_SIZE);
    string x;
    int xx;
    vector<string> kk;
    for (int i = 0; i < size; i++)
        kk.push_back(meta[i].first);
    out.push_back(kk);
    while (sz < loc)
    {
        kk.clear();
        sz += 4;
        for (int i = 0; i < size; i++)
        {
            if (meta[i].second == "int")
            {
                memcpy(&xx, tab + sz, INT_SIZE);
                kk.push_back(to_string(xx));
                sz += INT_SIZE;
            }
            else
            {
                memcpy(str_temp, tab + sz, STR_SIZE);
                x += str_temp;
                kk.push_back(x);
                x = "";
                sz += STR_SIZE;
            }
        }
        out.push_back(kk);
    }
    return out;
}