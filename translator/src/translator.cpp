#include "translator.h"
#include "read_file.h"
#include <cstring>
#include <elf.h>


int CompleteElfHeader(Text* elf_buf)
{
        // typedef struct {
        //         unsigned char e_ident[16];      /* Магическое число и другая информация */
        //         uint16_t e_type;                /* Тип объектного файла */
        //         uint16_t e_machine;             /* Архитектура */
        //         uint32_t e_version;             /* Версия объектного файла */
        //         uint64_t e_entry;               /* Виртуальный адрес точки входа */
        //         uint64_t e_phoff;               /* Смещение таблицы заголовков программы в файле */
        //         uint64_t e_shoff;               /* Смещение таблицы заголовков секций в файле */
        //         uint32_t e_flags;               /* Флаги, зависящие от процессора */
        //         uint16_t e_ehsize;              /* Размер заголовка ELF в байтах */
        //         uint16_t e_phentsize;           /* Размер записи таблицы заголовков программы */
        //         uint16_t e_phnum;               /* Количество записей в таблице заголовков программы */
        //         uint16_t e_shentsize;           /* Размер записи таблицы заголовков секций */
        //         uint16_t e_shnum;               /* Количество записей в таблице заголовков секций */
        //         uint16_t e_shstrndx;            /* Индекс таблицы строк в заголовке секции */
        // } Elf64_Ehdr;

        Elf64_Ehdr elf_header = {};
        unsigned char e_ident[EI_NIDENT] = {0x7f,
                                            0x45,
                                            0x4c,
                                            0x46,
                                            0x02,
                                            0x01,
                                            0x01};
        e_ident[15] = 0x10;

        memcpy(elf_header.e_ident, e_ident, sizeof(elf_header.e_ident));
        
        elf_header.e_type       = ET_EXEC;
        elf_header.e_machine    = EM_X86_64;
        elf_header.e_version    = EI_VERSION;           // дублирует из e_ident
        elf_header.e_entry      = 0;                    // абсолютный виртуальный адрес начала выполнения программы
        elf_header.e_phoff      = sizeof(Elf64_Ehdr);   // смещение от файла к program header table
        elf_header.e_shoff      = 0;                    // смещение от начала файла к section header table
        elf_header.e_flags      = 0;
        elf_header.e_ehsize     = sizeof(Elf64_Ehdr);   // размер заголовка ELF
        elf_header.e_phentsize  = sizeof(Elf64_Phdr);
        elf_header.e_phnum      = 2;                    // количество заголовков программы .text и .data
        elf_header.e_shentsize  = 0;
        elf_header.e_shnum      = 0;
        elf_header.e_shstrndx   = 0;

        memcpy(elf_buf->str + elf_buf->position, &elf_header, sizeof(elf_header));
        
        elf_buf->position += sizeof(elf_header);

        return 0;
}



// 00 00 00 00 00 00 00 00 00 00 00 00
        



int CompleteProgramHeaderTable(Text* elf_buf, Text* bin)
{
        // typedef struct
        // {
        //         uint32_t	p_type;			/* Segment type */
        //         uint32_t	p_flags;		/* Segment flags */
        //         uint64_t	p_offset;		/* Segment file offset */
        //         uint64_t	p_vaddr;		/* Segment virtual address */
        //         uint64_t	p_paddr;		/* Segment physical address */
        //         uint64_t	p_filesz;		/* Segment size in file */
        //         uint64_t	p_memsz;		/* Segment size in memory */
        //         uint64_t	p_align;		/* Segment alignment */
        // } Elf64_Phdr;
        
        Elf64_Phdr text_prog_header  = {};

        text_prog_header.p_type      = PT_LOAD;
        text_prog_header.p_offset    = 0xC0;                    // смещение от начала файла
        text_prog_header.p_vaddr     = 0xC0800408;              // виртуальный адрес
        text_prog_header.p_paddr     = 0;                       // физический адрес
        text_prog_header.p_filesz    = 0x**;                    // количество байтов в образе файла сегмента
        text_prog_header.p_memsz     = 0x**;                    // количество байтов в памяти образа сегмента
        text_prog_header.p_flags     = PF_R | PF_X;             // PF_R = READ - 0x04, PF_W = WRITE - 0x02, PF_X = EXEC - 0x01
        text_prog_header.p_align     = 0x200000;                // выравнивание


        memcpy(elf_buf->str + elf_buf->position, &text_prog_header, sizeof(text_prog_header));
        elf_buf->position += sizeof(text_prog_header);

        // Elf64_Phdr data_prog_header  = {};

        // data_prog_header.p_type      = PT_LOAD;
        // data_prog_header.p_offset    = 0x;                    // смещение от начала файла
        // data_prog_header.p_vaddr     = 0x**800408;              // виртуальный адрес
        // data_prog_header.p_paddr     = 0;                       // физический адрес
        // data_prog_header.p_filesz    = 0x;                    // количество байтов в образе файла сегмента
        // data_prog_header.p_memsz     = 0x;                    // количество байтов в памяти образа сегмента
        // data_prog_header.p_flags     = PF_R | PF_X | PF_W;      // PF_R = READ - 0x04, PF_W = WRITE - 0x02, PF_X = EXEC - 0x01
        // data_prog_header.p_align     = 0x200000;                // выравнивание


        // memcpy(buffer->str + buffer->position, &data_prog_header, sizeof(data_prog_header));
        // buffer->position += sizeof(data_prog_header);
        
        return 0;
}

int TranslateToELF(Text* elf_buf, Tree** tree, err_allocator* err_alloc)
{

        CompleteElfHeader(elf_buf);
        CompleteProgramHeaderTable(elf_buf);
        
        Text binary = {};
        CtorBuffer(&binary, "binary", err_alloc);
        
        CompleteText(elf_buf, NULL);
        CompleteData(elf_buf);

        ReallocateBuffer(elf_buf, elf_buf->position, err_alloc);

        DtorBuffer(&binary);

        return 0;
}


int CompleteText(Text* elf_buf, Text* binary)
{
        char* buf = elf_buf->str + elf_buf->position;
        
        for (size_t align = 0; align < 64 - (elf_buf->position % 64); align++)
                *(buf++) = 0x00;
        

        elf_buf->position = (size_t) (buf - elf_buf->str);

        memcpy(elf_buf + elf_buf->position, binary->str, binary->size_buffer);

        elf_buf->position += binary->size_buffer;
        

        // *(buf++) = 0xBB;
        // *(buf++) = 0x01;
        // *(buf++) = 0x00;
        // *(buf++) = 0x00;
        // *(buf++) = 0x00;
        // *(buf++) = 0xB8;
        // *(buf++) = 0x04;
        // *(buf++) = 0x00;
        // *(buf++) = 0x00;
        // *(buf++) = 0x00;
        // *(buf++) = 0xB9;
        // *(buf++) = 0xA4;
        // *(buf++) = 0x80;
        // *(buf++) = 0x04;
        // *(buf++) = 0x08;
        // *(buf++) = 0xBA;
        // *(buf++) = 0x0D;
        // *(buf++) = 0x00;
        // *(buf++) = 0x00;
        // *(buf++) = 0x00;
        // *(buf++) = 0xCD;
        // *(buf++) = 0x80;


        // *(buf++) = 0xB8;
        // *(buf++) = 0x01;
        // *(buf++) = 0x00;
        // *(buf++) = 0x00;
        // *(buf++) = 0x00;
        // *(buf++) = 0xBB;
        // *(buf++) = 0x2A;
        // *(buf++) = 0x00;
        // *(buf++) = 0x00;
        // *(buf++) = 0x00;
        // *(buf++) = 0xCD;
        // *(buf++) = 0x80;        
        

        return 0;
}

int CompleteData(Text* buffer)
{
        char* buf = buffer->str + buffer->position;
        
        memcpy(buf, "Hello World\n", sizeof("Hello World\n"));
                
        buffer->position = (size_t) (buf - buffer->str);

        // 0x48 0x65 0x6C 0x6C 0x6F 0x20 0x57 0x6F 0x72 0x6C 0x64 0x21 0x0A

        return 0;
}



int Verificator(Text* buffer, Tree** tree, err_allocator* err_alloc)
{
        if (buffer->str[0] != 0)
                //error

        if (buffer->str[1] != 0)
                //error

        if (buffer->str[2] != 0)
                //error


        // buffer->str[0] = 0x7c;
        // buffer->str[1] = 0x45;
        // buffer->str[2] = 0x4c;
        // buffer->str[3] = 0x46;

        // buffer->str[4] = 0x02;
        // buffer->str[5] = 0x01;
        // buffer->str[6] = 0x01;

        // for (size_t i = 7; i < 15; i++)
        //         buffer->str[i] = 0x00;
        
        // buffer->str[15] = 0x10;


        // buffer->str[16] = 0x02;
        // buffer->str[17] = 0x00;

        // buffer->str[18] = 0x03;
        // buffer->str[19] = 0x00;

        // buffer->str[20] = 0x01;
        // buffer->str[21] = 0x00;
        // buffer->str[22] = 0x00;
        // buffer->str[23] = 0x00;

   
        // buffer->str[28] = 0x34;
        // buffer->str[29] = 0x00;
        // buffer->str[30] = 0x00;
        // buffer->str[31] = 0x00;
        
  

        // buffer->str[36] = 0x00;
        // buffer->str[37] = 0x00;
        // buffer->str[38] = 0x00;
        // buffer->str[39] = 0x00;

        // buffer->str[40] = 0x34;
        // buffer->str[41] = 0x00;

       

        // buffer->str[44] = 0x02;
        // buffer->str[45] = 0x00;

        // buffer->str[46] = 0x00;
        // buffer->str[47] = 0x00;

        // buffer->str[48] = 0x00;
        // buffer->str[49] = 0x00;

        // buffer->str[50] = 0x00;
        // buffer->str[51] = 0x00;


        // buffer->str[52] = 0x01;
        // buffer->str[53] = 0x00;
        // buffer->str[54] = 0x00;
        // buffer->str[55] = 0x00;

        // // !
        // buffer->str[56] = 0x80;
        // buffer->str[57] = 0x00;
        // buffer->str[58] = 0x00;
        // buffer->str[59] = 0x00;

        // // !
        // buffer->str[60] = 0x80;
        // buffer->str[61] = 0x80;
        // buffer->str[62] = 0x04;
        // buffer->str[63] = 0x08;

        // buffer->str[64] = 0x00;
        // buffer->str[65] = 0x00;
        // buffer->str[66] = 0x00;
        // buffer->str[67] = 0x00;

        // // !
        // buffer->str[68] = 0x24;
        // buffer->str[69] = 0x00;
        // buffer->str[70] = 0x00;
        // buffer->str[71] = 0x00;

        // // !
        // buffer->str[72] = 0x24;
        // buffer->str[73] = 0x00;
        // buffer->str[74] = 0x00;
        // buffer->str[75] = 0x00;

        // // !
        // buffer->str[76] = 0x05;
        // buffer->str[77] = 0x00;
        // buffer->str[78] = 0x00;
        // buffer->str[79] = 0x00;

        // buffer->str[80] = 0x00;
        // buffer->str[81] = 0x10;
        // buffer->str[82] = 0x00;
        // buffer->str[83] = 0x00;


        // buffer->str[84] = 0x01; 
        // buffer->str[85] = 0x00;
        // buffer->str[86] = 0x00;
        // buffer->str[87] = 0x00;
 
        // buffer->str[96] = 0x00;
        // buffer->str[97] = 0x00;
        // buffer->str[98] = 0x00;
        // buffer->str[99] = 0x00;

        // buffer->str[108] = 0x07;
        // buffer->str[109] = 0x00;
        // buffer->str[110] = 0x00;
        // buffer->str[111] = 0x00;
        
        // buffer->str[112] = 0x00;
        // buffer->str[113] = 0x10;
        // buffer->str[114] = 0x00;
        // buffer->str[115] = 0x00;

        return 0;
}
