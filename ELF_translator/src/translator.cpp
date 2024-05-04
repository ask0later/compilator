#include "translator.h"
#include <cstddef>
#include <cstdio>
#include <cstring>


int TranslateToELF(Text* elf_buf, Text* code, Text* data, err_allocator* err_alloc)
{
        if (code->size_buffer % ALIGNMENT != 0)
        {
                INSERT_ERROR_NODE(err_alloc, "text segment is not aligned");
                err_alloc->need_call = true;
                return 1;
        }

        if (data->size_buffer % ALIGNMENT != 0)
        {
                INSERT_ERROR_NODE(err_alloc, "text segment is not aligned");
                err_alloc->need_call = true;                
                return 1;
        }

        CompleteElfHeader(elf_buf);
        CompleteProgramHeaderTable(elf_buf, code, data);
        
        CompleteText(elf_buf, code);
        CompleteData(elf_buf, data);

        ReallocateBuffer(elf_buf, elf_buf->position, err_alloc);
        if (err_alloc->need_call)
        {
                INSERT_ERROR_NODE(err_alloc, "invalid executing ReallocateBuffer");
                return 1;
        }


        VerificatorELF(elf_buf, err_alloc);
        if (err_alloc->need_call)
        {
                INSERT_ERROR_NODE(err_alloc, "invalid executing VerificatorELF");
                return 1;
        }

        return 0;
}



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

        elf_header.e_ident[EI_MAG0]        = ELFMAG0;
        elf_header.e_ident[EI_MAG1]        = ELFMAG1;              
        elf_header.e_ident[EI_MAG2]        = ELFMAG2;
        elf_header.e_ident[EI_MAG3]        = ELFMAG3;
        elf_header.e_ident[EI_CLASS]       = ELFCLASS64;
        elf_header.e_ident[EI_DATA]        = ELFDATA2LSB;
        elf_header.e_ident[EI_VERSION]     = EV_CURRENT;
        elf_header.e_ident[EI_OSABI]       = ELFOSABI_NONE;  
        elf_header.e_ident[EI_ABIVERSION]  = 0x00;  
        elf_header.e_ident[EI_PAD]         = ET_NONE;

        
        elf_header.e_type       = ET_EXEC;
        elf_header.e_machine    = EM_X86_64;
        elf_header.e_version    = EI_VERSION;                           // дублирует из e_ident
        elf_header.e_entry      = ABSOLUTE_FILE_ADDRESS + ALIGNMENT;    // абсолютный виртуальный адрес начала выполнения программы
        elf_header.e_phoff      = sizeof(Elf64_Ehdr);                   // смещение от файла к program header table
        elf_header.e_shoff      = 0;                                    // смещение от начала файла к section header table
        elf_header.e_flags      = 0;
        elf_header.e_ehsize     = sizeof(Elf64_Ehdr);                   // размер заголовка ELF
        elf_header.e_phentsize  = sizeof(Elf64_Phdr);                   // размер заголовка программ
        elf_header.e_phnum      = 2;                                    // количество заголовков программы .text и .data
        elf_header.e_shentsize  = 0;
        elf_header.e_shnum      = 0;
        elf_header.e_shstrndx   = 0;

        memcpy(elf_buf->str + elf_buf->position, &elf_header, sizeof(Elf64_Ehdr));
        
        elf_buf->position += sizeof(Elf64_Ehdr);

        return 0;
}

int CompleteProgramHeaderTable(Text* elf_buf, Text* text, Text* data)
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

        size_t text_size = text->size_buffer;
        
        text_prog_header.p_type      = PT_LOAD;
        text_prog_header.p_offset    = TEXT_OFFSET;                             // смещение от начала файла
        text_prog_header.p_vaddr     = ABSOLUTE_FILE_ADDRESS + ALIGNMENT;       // виртуальный адрес
        text_prog_header.p_paddr     = 0;                                       // физический адрес
        text_prog_header.p_filesz    = text_size;                               // количество байтов в образе файла сегмента
        text_prog_header.p_memsz     = text_size;                               // количество байтов в памяти образа сегмента
        text_prog_header.p_flags     = PF_R | PF_X;                             // PF_R = READ - 0x04, PF_W = WRITE - 0x02, PF_X = EXEC - 0x01
        text_prog_header.p_align     = ALIGNMENT;                               // выравнивание


        memcpy(elf_buf->str + elf_buf->position, &text_prog_header, sizeof(Elf64_Phdr));
        elf_buf->position += sizeof(Elf64_Phdr);

        Elf64_Phdr data_prog_header  = {};

        uint64_t data_offset = TEXT_OFFSET + text_size;
        uint64_t data_size   = data->size_buffer;

        data_prog_header.p_type      = PT_LOAD;
        data_prog_header.p_offset    = data_offset;                             // смещение от начала файла
        data_prog_header.p_vaddr     = ABSOLUTE_FILE_ADDRESS + data_offset;     // виртуальный адрес
        data_prog_header.p_paddr     = 0;                                       // физический адрес
        data_prog_header.p_filesz    = data_size;                               // количество байтов в образе файла сегмента
        data_prog_header.p_memsz     = data_size;                               // количество байтов в памяти образа сегмента
        data_prog_header.p_flags     = PF_R | PF_X | PF_W;                      // PF_R = READ - 0x04, PF_W = WRITE - 0x02, PF_X = EXEC - 0x01
        data_prog_header.p_align     = ALIGNMENT;                               // выравнивание


        memcpy(elf_buf->str + elf_buf->position, &data_prog_header, sizeof(Elf64_Phdr));
        elf_buf->position += sizeof(Elf64_Phdr);

        elf_buf->position += (ALIGNMENT - elf_buf->position % ALIGNMENT);
        
        return 0;
}


int CompleteText(Text* elf_buf, Text* code)
{
        memcpy(elf_buf->str + elf_buf->position, code->str, code->size_buffer);

        elf_buf->position += code->size_buffer;

        return 0;
}

int CompleteData(Text* elf_buf, Text* data)
{
        memcpy(elf_buf->str + elf_buf->position, data->str, data->size_buffer);
        elf_buf->position += data->size_buffer;

        return 0;
}


int VerificatorELF(Text* elf_buf, err_allocator* err_alloc)
{

        return 0;
}
