#pragma once
#include "elf.h"
#include <cwchar>

// Returns the address of the page containing address 'x'.
#define PAGE_START(x)  ((x)& PAGE_MASK)

// Returns the offset of address 'x' in its page.
#define PAGE_OFFSET(x) ((x) & ~PAGE_MASK)

// Returns the address of the next page after address 'x', unless 'x' is
// itself at the start of a page.

#define PAGE_END(x)    PAGE_START((x)+(PAGE_SIZE - 1))

// Android uses RELA for aarch64 and x86_64. mips64 still uses REL.
#if defined(__aarch64__) || defined(__x86_64__)
#define USE_RELA 1
#endif


#define FLAG_LINKED     0x00000001
#define FLAG_EXE        0x00000004 // The main executable
#define FLAG_LINKER     0x00000010 // The linker itself
#define FLAG_NEW_SOINFO 0x40000000 // new soinfo format

#define SOINFO_NAME_LEN 128
typedef void (*linker_function_t)();

struct soinfo;
//struct SoInfoListAllocator {
//public:
//	LinkedList
//
//};
struct soinfo {
	char name[SOINFO_NAME_LEN];
	const ElfW(Phdr) *phdr;
	size_t phnum;
	ElfW(Addr) entry;
	ElfW(Addr) base;
	size_t size;
	ElfW(Dyn)* dynamic;

	soinfo* next;
	unsigned flags;//是否被Linked的标志
	const char* strtab;//字符串表
	ElfW(Sym)* symtab;//符号表
	size_t nbucket;//保存函数符号表的索引
	size_t nchain;//指向函数符号
	unsigned * bucket;//指向函数符号表索引表地址
	unsigned * chain;//指向函数符号表地址

    linker_function_t* preinit_array;
    size_t preinit_array_count;
    linker_function_t* init_array;
    size_t init_array_count;
    linker_function_t* fini_array;
    size_t fini_array_count;

    linker_function_t init_func;
    linker_function_t fini_func;
    ElfW(Rel)* plt_rel;
    size_t plt_rel_count;

    ElfW(Rel)* rel;
    size_t rel_count;


#if defined(__mips__) || ! defined(__LP64__)
	//
	ElfW(Addr) ** plt_got;//给 mpis和Mips64预留的空间，
#endif

#if defined(__arm__)
  // ARM EABI section used for stack unwinding.
      unsigned* ARM_exidx;
     size_t ARM_exidx_count;
#elif defined(__mips__)
     unsigned mips_symtabno;
     unsigned mips_local_gotno;
     unsigned mips_gotsym;
#endif

#if !defined(__LP64__)
    bool has_text_relocations;
#endif
    bool has_DT_SYMBOLIC;

 size_t ref_count;
 bool constructors_called;

 // When you read a virtual address from the ELF file, add this
  // value to get the corresponding address in the process' address space.
    ElfW(Addr) load_bias; //so文件加载到内存中的基地址，需要通过计算得到

    void call_constructors();
    void CallConstructor(const char* name,linker_function_t function);
    void CallArray(const char* array_name,linker_function_t* functions,size_t count ,bool reverse);

};

