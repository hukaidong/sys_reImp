#include <signal.h>
#include <sys/mman.h>

#include "my_malloc.h"
#include "malloc/type.h"
#include "malloc/global.h"
#include "malloc/thread_entries.h"
#include "malloc/page.h"
#include "malloc/pcb.h"
#include "malloc/page_file_map.h"
#include "pthread/type.h"
#include "malloc/segment.h"

// TODO: bind to scheduler
void _page_setup() {
  __ds_init();
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = page_segfault_handler;
  sigaction(SIGSEGV, &sa, NULL);

}
void _page_protect(ssize_t pidx) {
  mprotect(
      page_index_2_base(pidx),
      PAGE_SIZE, P_N);
}

void _page_unprotect(ssize_t pidx) {
  mprotect(
      page_index_2_base(pidx),
      PAGE_SIZE, P_RW);
}

void *new_page(size_t size_req, ssize_t thread_id) {
  int req_page_num = (size_req + sizeof(segment_header)) / PAGE_SIZE + 1;
  ssize_t pidx;
  tNode* thread_e;
  search_thread(thread_id, &thread_e);
  if(thread_e->num_page_claimed > PAGE_LIM_PER_THREAD)
    return NULL;
  else{
    if((pidx = pcb_next_free_page(thread_id, req_page_num))<0) {
      pidx = pcb_next_swapable_page(thread_id, req_page_num);
      if (pidx >= 0) {
        for (int i=0; i<req_page_num; i++) {
          insert_swap_page(pidx+i);
        }
      } else {
        return NULL;
      }
    }
    thread_e->num_page_claimed += req_page_num;
  }
  for (int i=0; i<req_page_num; i++) {
    page_assign(pidx+i, thread_id);
  }
  int  maxfree = seg_init(page_index_2_base(pidx), req_page_num, size_req);
  pcb[pidx].max_avail = maxfree;
  return page_index_2_base(pidx) + sizeof(segment_header);
}

void release_page(ssize_t pidx, ssize_t thread_id) {
  if(pcb[pidx].thread_id!= thread_id){
    insert_swap_page(pidx);
    page_swap_in_virtual(pidx, thread_id);
  }
  pcb[pidx].thread_id = -1;
  pcb[pidx].max_avail = -1;
}

void page_assign(ssize_t pidx, ssize_t thread_id) {
  pcb[pidx].thread_id = thread_id;
  pcb[pidx].max_avail = 0;
  _page_unprotect(pidx);
}



void page_segfault_handler (int sig, siginfo_t *si, void *_) {
  UNUSED(sig);
  UNUSED(_);
  void *addr = si->si_addr;
  // thread_id = get_thread_id();
  // int page_id = (addr & PAGE_MSK) >> PAGE_OFFSET;
  // page_swap_out(pageid);
  // page_svap_in(pageid, thread_id);
}
