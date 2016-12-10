int P(struct semaphore *s)
{

	int sr = int_off();

    s->value--;
   
    if (s->value < 0){
      running->status=BLOCK;
      enqueue(&s->queue, running);
      tswitch();
    }
    
    int_on(sr);
  // write YOUR C code for P()
}

int V(struct semaphore *s)
{
  // write YOUR C code for V()

    PROC *p;

    int sr = int_off();

    s->value++;

    if (s->value <= 0){
        p = dequeue(&s->queue);
        p->status = READY;
        enqueue(&readyQueue, p);
    }
    
    int_on(sr);

}
