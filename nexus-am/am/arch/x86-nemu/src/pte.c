#include <x86.h>
#include <string.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) {
    // 获取页目录基地址（物理地址）
    PDE *pdir = (PDE *)p->ptr;
    
    // 从虚拟地址提取索引
    uint32_t vaddr = (uint32_t)va;
    uint32_t pd_index = (vaddr >> 22) & 0x3FF;   // 页目录索引（高10位）
    uint32_t pt_index = (vaddr >> 12) & 0x3FF;    // 页表索引（中10位）
    
    
    PTE *pt = NULL;
    
    if (pdir[pd_index] & PTE_P) {
        // 页表已存在：获取页表基地址
        pt = (PTE *)(pdir[pd_index] & 0xFFFFF000);
    } else {
        // 页表不存在：分配新页表
        pt = (PTE *)palloc_f();  // 获取一页物理内存
        
				pdir[pd_index] = ((uint32_t)pt & 0xFFFFF000) | PTE_P;
        }
         
    
    // 写入页表项
    pt[pt_index] = ((uint32_t)pa & 0xFFFFF000) | PTE_P;
}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack,
                void *entry, char *const argv[], char *const envp[]) {
                
  uint32_t *stack_top = (uint32_t *)ustack.end;
  
  stack_top -= 2;
  stack_top[0] = 0; 
  stack_top[1] = 0;  

  // 然后分配 trap frame
  _RegSet *tf = (_RegSet *)((uintptr_t)stack_top - sizeof(_RegSet));
  memset(tf, 0, sizeof(_RegSet)); 
  tf->eip = (uintptr_t)entry;     
  tf->esp = (uintptr_t)stack_top;
  tf->cs = 8;  // 保证 differential testing 正确
  tf->eflags = 0x02; // IF=1，表示开启中断（常规初始化）

  return tf;
}
