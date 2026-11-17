#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Allocate/map a shared 4KB memory page into the calling process's address space
int
sys_shm_get(void)
{
  return shm_get();
}

// Initialize kernel semaphore sem_id to value
int
sys_sem_init(void)
{
  int sem_id, value;

  if(argint(0, &sem_id) < 0 || argint(1, &value) < 0)
    return -1;
  return sem_init(sem_id, value);
}

// Wait (decrement / block) on kernel semaphore sem_id
int
sys_sem_wait(void)
{
  int sem_id;

  if(argint(0, &sem_id) < 0)
    return -1;
  return sem_wait(sem_id);
}

// Post (increment / wake) on kernel semaphore sem_id
int
sys_sem_post(void)
{
  int sem_id;

  if(argint(0, &sem_id) < 0)
    return -1;
  return sem_post(sem_id);
}
