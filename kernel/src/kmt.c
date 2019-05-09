#include <common.h>


static void kmt_init(){

    //TO BE DONE
    return;
}

static int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){

    //TO BE DONE


    return NULL;
}

static void kmt_teardown(task_t *task){

    //TO BE DONE

    return;
}

static void kmt_spin_init(spinlock_t *lk, const char *name){


    //TO BE DONE
    return ;
}


static void kmt_spin_lock(spinlock_t *lk){

    //TO BE DONE
    return ;
}


static void kmt_spin_unlock(spinlock_t *lk){




    //TO BE DONE
    return ;    
}
static void kmt_sem_init(sem_t *sem, const char *name, int value){


    return ;
}

static void kmt_sem_wait(sem_t *sem){


    return;
}

void kmt_sem_signal(sem_t *sem){


    return;
}
