#ifndef _PAGE_H
#define _PAGE_H

typedef void Table;

struct RowInfo
{
    char link[2048],name[255],
    type[32],size[32],time[32];
};

char* ToPageString(const char* path,const char* ts);
Table* CreatTable();
Table* InsertRow(Table *table,RowInfo *info);
char* ToTableString(Table *table);
void FreeTable(Table *table);

#endif 
