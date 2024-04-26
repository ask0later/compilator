#include "translator.h"
#include "read_file.h"
#include <cstring>
#include <elf.h>


int TranslateToELF(Text* elf_buf, err_allocator* err_alloc)
{
        Text start_bin = {};
        Text converted_bin = {};

        CtorBuffer(&start_bin, "binary", err_alloc);
        CtorEmptyBuffer(&converted_bin, start_bin.size_buffer * 2, err_alloc);
        ConvertBinary(&start_bin, &converted_bin);
        DtorBuffer(&start_bin);


        CompleteElfHeader(elf_buf);
        CompleteProgramHeaderTable(elf_buf);
        
        
        
        CompleteText(elf_buf, &converted_bin);
        // CompleteData(elf_buf);

        ReallocateBuffer(elf_buf, elf_buf->position, err_alloc);

        return 0;
}

int ConvertBinary(Text* start_bin, Text* converted_bin)
{
        while (start_bin->position != start_bin->size_buffer)
        {
                ParseInstuction(start_bin, converted_bin);
        }

        return 0;
}

// int DecToHex(char* number)
// {
//         size_t i = 0;
//         char hex_number[20] = {};

//         while (number[i] != '\0')
//         {
//                 hex_number


//                 i++;
//         }

// }

// int ParseInstuction(Text* start_bin, Text* converted_bin)
// {
//         switch(start_bin->str[start_bin->position] & 0b00011111)
//         {       
//                 const unsigned char NUM_BIT      =  1 << 5;
//                 const unsigned char REG_BIT      =  1 << 6;
//                 const unsigned char RAM_BIT      =  1 << 7;

//                 int argument = 0;

//                 start_bin->position++;
//                 case PUSH:
//                         if (start_bin->str[start_bin->position - 1] & 1 << 5)
//                         {
//                                 converted_bin->str[converted_bin->position] = 0x68;
//                                 converted_bin->position++;

//                                 memcpy(converted_bin->str + converted_bin->position, start_bin->str + start_bin->position, sizeof(int));
//                                 converted_bin->position += sizeof(int);
//                         }
//                         else if (start_bin->str[start_bin->position - 1] & 1 << 6)
//                         {
//                                 converted_bin->str[converted_bin->position] = 0x50;
//                                 converted_bin->str[converted_bin->position] += (start_bin->str[start_bin->position] - 1);
                                
//                                 converted_bin->position++;
//                         }

//                         if (start_bin->str[start_bin->position - 1] & 1 << 7)
//                         {
//                                 value1 = spu->RAM[argument];
//                         }
//                         else
//                         {
//                                 value1 = argument;
//                         }

//                         if (start_bin->str[start_bin->position - 1] & NUM_BIT)
//                                 cmd->position += sizeof(int);
//                         else
//                                 cmd->position += sizeof(char);

//                         StackPush(value1, &(spu->stk));

//                         break;
//                 case POP:
//                         value1 = StackPop(&(spu->stk));
                        
//                         if (start_bin->str[start_bin->position - 1] & NUM_BIT)
//                         {
//                                 memcpy(&argument, (int*)(cmd->buffer + cmd->position), sizeof(int));
//                                 cmd->position += sizeof(int);
//                                 spu->RAM[argument] = value1;
//                         }
//                         else if (start_bin->str[start_bin->position - 1] & RAM_BIT)
//                         {
//                                 argument = spu->reg[(size_t) cmd->buffer[cmd->position] - 1];
//                                 cmd->position += sizeof(char);
                                
//                                 spu->RAM[argument] = value1;
//                         }
//                         else
//                         {
//                                 spu->reg[(size_t) cmd->buffer[cmd->position] - 1] = value1;
//                                 cmd->position += sizeof(char);
//                         }

//                         break;
//                 case ADD:

//                         break;
//                 case SUB:

//                         break;
//                 case MUL:

//                         break;
//                 case DIV:

//                         break; 
//                 case SQRT:

//                         break;
//                 case SIN:

//                         break;
//                 case COS:

//                         break;
//                 case IN:

//                         break;
//                 case JA:

//                         break;
//                 case JAE:

//                         break;
//                 case JB:

//                         break;
//                 case JBE:

//                         break;
//                 case JE:

//                         break;
//                 case JNE:

//                         break;
//                 case JMP:

//                         break;
//                 case CALL:

//                         break;
//                 case RETURN:

//                         break; 
//                 case OUT:

//                         break;
//                 case HTL:

//                         break;

//                 default:
//                         /*error*/
//                         printf("extra instruction");
//                         break;

//         }
// }



// PUSH
// {
//             cmd->position++;
//             if (cmd->buffer[cmd->position - 1] & NUM_BIT)
//             {
//                 memcpy(&argument, (int*)(cmd->buffer + cmd->position), sizeof(int));
//             }
//             else if (cmd->buffer[cmd->position - 1] & REG_BIT)
//             {
//                 argument = spu->reg[(size_t) cmd->buffer[cmd->position] - 1];    
//             }
//             if (cmd->buffer[cmd->position - 1] & RAM_BIT)
//             {
//                 value1 = spu->RAM[argument];
//             }
//             else
//             {
//                 value1 = argument;
//             }
//             if (cmd->buffer[cmd->position - 1] & NUM_BIT)
//                 cmd->position += sizeof(int);
//             else
//                 cmd->position += sizeof(char);

//             StackPush(value1, &(spu->stk));
// }

// POP
// {
//             cmd->position++;
//             value1 = StackPop(&(spu->stk));
        
//             if (cmd->buffer[cmd->position - 1] & NUM_BIT)
//             {
//                 memcpy(&argument, (int*)(cmd->buffer + cmd->position), sizeof(int));
//                 cmd->position += sizeof(int);
//                 spu->RAM[argument] = value1;
//             }
//             else if (cmd->buffer[cmd->position - 1] & RAM_BIT)
//             {
//                 argument = spu->reg[(size_t) cmd->buffer[cmd->position] - 1];
//                 cmd->position += sizeof(char);
                
//                 spu->RAM[argument] = value1;
//             }
//             else
//             {
//                 spu->reg[(size_t) cmd->buffer[cmd->position] - 1] = value1;
//                 cmd->position += sizeof(char);
//             }
// }


// DEF_CMD("add", ADD, NO_ARGUMENTS,
// {         
//                 cmd->position++;
//                 value1 = StackPop(&(spu->stk));                                                  
//                 value2 = StackPop(&(spu->stk));                                                  
//                 StackPush(value2 + value1, &(spu->stk));  
// }
// )

// DEF_CMD("sub", SUB, NO_ARGUMENTS,
// {
//             cmd->position++;
//             ARIFMETIC_OPERATION(-)
// }
// )

// DEF_CMD("mul", MUL, NO_ARGUMENTS,
// {
//             cmd->position++;
//             ARIFMETIC_OPERATION(*)
// }
// )


// DEF_CMD("div", DIV, NO_ARGUMENTS,
// {
//             cmd->position++;
//             ARIFMETIC_OPERATION(/)
// }
// )


// DEF_CMD("sqrt", SQRT, NO_ARGUMENTS,
// {
//             cmd->position++;
//             value1 = 100 * StackPop(&(spu->stk));
//             value1 =(int) sqrt((double) value1) / 10;
//             StackPush(value1, &(spu->stk));
// }
// )

// DEF_CMD("sin", SIN, NO_ARGUMENTS,
// {
//             cmd->position++;
//             value1 = StackPop(&(spu->stk));
//             value1 = (int) sin((double) value1);
//             StackPush(value1, &(spu->stk));
// }
// )


// DEF_CMD("cos", COS, NO_ARGUMENTS, 
// {
//             cmd->position++;
//             value1 = StackPop(&(spu->stk));
//             value1 = (int) cos((double) value1);
//             StackPush(value1, &(spu->stk));
// }
// )

// DEF_CMD("in", IN, NO_ARGUMENTS, 
// {
//             cmd->position++;
//             scanf("%d", &argument);
//             StackPush(argument, &(spu->stk));
// }
// )

// DEF_CMD("ja",  JA,  LABEL_ARGUMENTS, 
// {
//                 value1 = StackPop(&(spu->stk));                                                  \
//                 value2 = StackPop(&(spu->stk));                                                  \
//                 if (value2 > value1)                                                        \
//                 {                                                                                \
//                         memcpy(&argument, (int*)(cmd->buffer + cmd->position + 1), sizeof(int));     \
//                         value3 = (int) cmd->position;                                                \
//                         cmd->position = (size_t)(value3 - argument);                                 \
//                 }                                                                                \
//                 else                                                                             \
//                         cmd->position += sizeof(int) + sizeof(char);                                 \
// })       


// DEF_CMD("jmp", JMP, LABEL_ARGUMENTS,
// {
//             memcpy(&argument, (int*)(cmd->buffer + cmd->position + 1), sizeof(int));
//             value3 = (int) cmd->position;
//             cmd->position = (size_t)(value3 - argument);
// }
// )

// DEF_CMD("call", CALL, LABEL_ARGUMENTS,
// {
//             memcpy(&argument, (int*)(cmd->buffer + cmd->position + 1), sizeof(int));
//             value3 = (int)  cmd->position;
//             value1 = (int) (cmd->position + sizeof(char) + sizeof(int));
            
//             StackPush(value1, &(spu->adress));
//             cmd->position = (size_t)(value3 - argument);
// }
// )

// DEF_CMD("ret", RET, NO_ARGUMENTS,
// {
//             value1 = StackPop(&(spu->adress));
//             cmd->position = (size_t) value1;
// }
// )

// DEF_CMD("out", OUT, NO_ARGUMENTS,
// {
//             cmd->position++;
//             value1 = StackPop(&(spu->stk));
//             printf("|%2d|\n", value1);
// }




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



int Verificator(Text* elf_buf, err_allocator* err_alloc)
{
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
