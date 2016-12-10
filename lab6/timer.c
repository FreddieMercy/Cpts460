/******************** timer.c file *************************************/
#define LATCH_COUNT     0x00	   /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	   /* ccaammmb, a = access, m = mode, b = BCD */

#define TIMER_FREQ   1193182L	   /* clock frequency for timer in PC and AT */
#define TIMER_COUNT  TIMER_FREQ/60 /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0


char *ctable = "0123456789ABCDEF";
u16 BASE = 10;

u16 tick;

int hour, min, sec;
hour = 0;
min = 0;
sec = 0;

int enable_irq(u16 irq_nr)
{
  lock();
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));

}

int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */

  printf("timer init\n");
  tick = 0; 
  out_byte(TIMER_MODE, SQUARE_WAVE);	// set timer to run continuously
  out_byte(TIMER0, TIMER_COUNT);	// timer count low byte
  out_byte(TIMER0, TIMER_COUNT >> 8);	// timer count high byte 
  enable_irq(TIMER_IRQ); 
}

/*===================================================================*
 *		    timer interrupt handler       		     *
 *===================================================================*/
int thandler()
{
  tick++; 
  tick %= 60;
  if (tick == 0){                      // at each second

        //itimer first
        tqeDec();
        tqeDequeue();

        sec++;

        if(sec >= 60)
        {
            sec = 0;
            min++;
        
            if(min >= 60)
            {
                min = 0;
                hour++;

                if(hour >= 24)
                {
                    hour = 0;
                }
            }
        }

        printTime();


        //if in Umode, dec, and switch if timeup
        if(running->inkmode <= 1)
        {
            time--;
            if(time <= 0)
            {
                out_byte(0x20, 0x20); 
                //tswitch();

                if(running->pid <= 3 && running->pid >= 1)
                {

                  scheduler();
                  goUmode();
                }
                else
                {
                  tswitch();
                }
            }
        }

            
  }
  out_byte(0x20, 0x20);                // tell 8259 PIC EOI
}


int printTime()
{
  
  row = 23;
  column = 66;

  printu(hour);
  putcs(':');
  printu(min);
  putcs(':');
  printu(sec);


}

int rpu(u16 x)
{
  char c;
  if (x) {
    c = ctable[x % BASE];
    rpu(x / BASE);
    putcs(c);
  }
}

int printu(u16 x)
{
  BASE = 10;
  if (x==0)
    putcs('0');
  else
    rpu(x);

}

//############################## itimer ########################################################

int itimer_init()
{
  //u2.c ??

  PROC *p;
  int i;

  printf("init TQE....");

  for (i=0; i<NPROC; i++){   // initialize all procs

        p = &proc[i];
        tqe[i].time = 0;
        tqe[i].proc = p;
        tqe[i].action = -1;
        tqe[i].next=0;
  }
}


int itimer(int t)
{
  TQE *tmp = &tqe[running->pid];

  if(t == 0)
  {
    lock()
    //cancellation, no need to check dup in queue
    tmp->action = -1;
    tmp->time = 0;
    tqeEnqueue(tmp);
    unlock();
  }
  else
  {
    if(tmp->action<0)
    {
      lock();
      kcinth_For_itimer(tmp);
      tmp->time = t;
      tqeEnqueue(tmp);
      unlock();
    }
    else
    {
      printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nHello!!  Proc %d had been assigned itimer already, FAILED!!!\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", running->pid);
    }
  }

  sleep(running);
}


//Missing: time, action

int tqeEnqueue(TQE *t)
{
  int timeReminder = 0;
  TQE *tmp = tq;
  TQE *pre = tq;
  int index = 0;
  int found = 0;

  while(tmp != 0)
  {
    if(t->time > tmp->time)
    {
      t->time -= tmp->time;
      tmp = tmp->next;

      if(index > 0)
      {
        pre = pre->next;
      }
    }
    else
    {
      found = 1;

      //insert t if valid

      if(t->time != 0)
      {
        t->next = tmp;

        if(pre != 0)
        {
          //the middle
          pre->next = t;
          
        }
        else
        {
          //the first
          tq = t;
          pre = tq;
        }

        //update the time next
        tmp->time -= t->time;
      }
      while(tmp != 0)
      {
        //time == 0, aka, cancel
        if(tmp == t)
        {
          tqeRemove(tmp, pre);

          //since found == 1, and we cancelled the demanded, and time - 0 = time still, thus we just terminate
          return;
        }

        tmp = tmp->next;
        
        if(index > 0)
        {
          pre = pre->next;
        }
      }

      break;
    }

    index++;
  }

  if(!found)
  {
      if(pre != 0)
      {
        //the last
        pre->next = t;
        
      }
      else
      {
        //the initial
        tq = t;
      }

  }
}

//remove from queue, don't exec action!!!!

int tqeRemove(TQE *t, TQE *pre)
{
  TEQ *target = t;
  if(t != 0)
  {
    if(pre != 0)
    {
      pre->next = t->next;
    }
    else
    {
      t = t->next;
    }

    //init t (aka, *target)
    target->time = 0;
    target->action = -1;
    target->next=0;
  }
  
  //if t == 0, we don't have to do anything.
}

int tqeDec()
{
  TQE *tmp = tq;
  while(tmp != 0)
  {
    tmp->time--;
  }
}


//if tq->time == 0, then pop it, otherwise don't!
//if pop, exec action
int tqeDequeue()
{
    TEQ *target = tq;
    PROC *rec = running;
    PROC *tarPro;


    while(tq != 0 && tq->time < 0)
    {
      tq = tq->next;
      target->time = 0;

      //exec action!!

      if(target->action < 0)
      {
        printf("\n**************************************\ntarget's action is null, ERROR!!!\n**************************************\n");
      }
      else if(target->action == 0) //wakeup
      {
        running = target->proc;
        wakeup(running);
        running = rec;
      }
      else if(target->action == 1) //notify (WTF??)
      {
        tarPro = target->proc;
        printf("\n**************************************\nHello!!  Proc %d reached the time intervel\n**************************************\n", tarPro->pid);
      }
      else
      {
        running = target->proc;
        target->action();
        running = rec;
      }

      //################

      target->action = -1;
      target->next=0;

      target = tq;

    }
  
}
