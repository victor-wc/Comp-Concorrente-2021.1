
// Monitor que implementa a logica do padrao leitores/escritores
class LE {
    private int leit, escr;
    private int compartilhada = 0;

    // Construtor
    LE() {
        this.leit = 0; //leitores lendo (0 ou mais)
        this.escr = 0; //escritor escrevendo (0 ou 1)
    }

    	// Getter para a variavel compartilhada
	public synchronized int getCompartilhada() {
		return this.compartilhada;
	}

	// Setter para a variavel compartilhada
	public synchronized void setCompartilhada(int n) {
		this.compartilhada = n;
	}

    // Entrada para leitores
    public synchronized void EntraLeitor (int id) {
        try {
            //while (this.escr > 0) {
            if (this.escr > 0) {
                System.out.println ("le.leitorBloqueado("+id+")");
                wait();  //bloqueia pela condicao logica da aplicacao
            }
            this.leit++;  //registra que ha mais um leitor lendo
            System.out.println ("le.leitorLendo("+id+")");
        } catch (InterruptedException e) { }
    }

    // Saida para leitores
    public synchronized void SaiLeitor (int id) {
        this.leit--; //registra que um leitor saiu
        if (this.leit == 0)
            this.notify(); //libera escritor (caso exista escritor bloqueado)
        System.out.println ("le.leitorSaindo("+id+")");
    }

    // Entrada para escritores
    public synchronized void EntraEscritor (int id) {
        try {
            //while ((this.leit > 0) || (this.escr > 0)) {
            if ((this.leit > 0) || (this.escr > 0)) {
                System.out.println ("le.escritorBloqueado("+id+")");
                wait();  //bloqueia pela condicao logica da aplicacao
            }
            this.escr++; //registra que ha um escritor escrevendo
            System.out.println ("le.escritorEscrevendo("+id+")");
        } catch (InterruptedException e) { }
    }

    // Saida para escritores
    public synchronized void SaiEscritor (int id) {
        this.escr--; //registra que o escritor saiu
        notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
        System.out.println ("le.escritorSaindo("+id+")");
    }
}



//Aplicacao de exemplo--------------------------------------------------------
// Leitor
class Leitor extends Thread {
    int id; //identificador da thread
    int delay; //atraso bobo
    LE monitor;//objeto monitor para coordenar a lógica de execução das threads

    // Construtor
    Leitor (int id, int delayTime, LE m) {
        this.id = id;
        this.delay = delayTime;
        this.monitor = m;
    }

    public void primalidade(int number) {
		if (number == 0) {
			System.out.println("variavel compartilhada: " + number + " O número não é primo");
			return;
		}
		for (int i = 2; i < number; i++) {
			if (number % i == 0) {
				System.out.println("variavel compartilhada: " + number + " O número não é primo");
				return;
			}
		}
		System.out.println("variavel compartilhada: " + number + " Primalidade confirmada!");
	}

    // Método executado pela thread
    public void run () {
        try {
            for (int i=0; i<Principal.MAX_LOOPS; i++) {
                this.monitor.EntraLeitor(this.id);
                primalidade(this.monitor.getCompartilhada());
                System.out.println("le.escritorEscrevendo("+id+") - "+id);
                this.monitor.SaiLeitor(this.id);
                sleep(this.delay);
            }
        } catch (InterruptedException e) { return; }
    }
}

//--------------------------------------------------------
// Escritor
class Escritor extends Thread {
    int id; //identificador da thread
    int delay; //atraso bobo...
    LE monitor; //objeto monitor para coordenar a lógica de execução das threads

    // Construtor
    Escritor (int id, int delayTime, LE m) {
        this.id = id;
        this.delay = delayTime;
        this.monitor = m;
    }

    // Método executado pela thread
    public void run () {
        try {
                for (int i=0; i<Principal.MAX_LOOPS; i++) {
                    this.monitor.EntraEscritor(this.id);
                    this.monitor.setCompartilhada(this.id);
                    System.out.println("le.leitorLendo("+id+")");
                    this.monitor.SaiEscritor(this.id);
                    sleep(this.delay); //atraso bobo...
                
                } //...loop bobo para simbolizar o tempo de escrita
        } catch (InterruptedException e) { return; }
    }
}

class LeitorEscritor extends Thread {
    int id; //identificador da thread
    int delay; //atraso bobo...
    LE monitor;

    LeitorEscritor (int id, int delayTime, LE m) {
        this.id = id;
        this.delay = delayTime;
        this.monitor = m;

    }

    public void run() {
        try{
            for(int i = 0; i < Principal.MAX_LOOPS; i++) {
                this.monitor.EntraLeitor(this.id);
                int n = this.monitor.getCompartilhada();
                if (n % 2 == 0) {
                    System.out.println("le.leitorLendo("+id+")" + " A memória compartilhada " + n + " é par");
                } else {
                    System.out.println("le.leitorLendo("+id+")"+ " A variável compartilhada " + n + " é ímpar");
                }
                System.out.println("le.leitorLendo("+id+")");
                this.monitor.SaiLeitor(this.id);
                this.monitor.EntraEscritor(this.id);
                this.monitor.setCompartilhada(n * 2);
                System.out.println("le.escritorEscrevendo("+id+") - "+id);
                this.monitor.SaiEscritor(this.id);
                sleep(this.delay);
            }
        }catch (InterruptedException e){

        }
    }
}

class Principal extends Thread {
    public static final int MAX_LOOPS = 10; //interacoes das threads
	private static final int L = 5; // total de threads leitoras
	private static final int E = 5; // total de threads escritoras
	private static final int LE = 5; // total de threads leitoras e escritoras

    public static void main (String[] args) {
        int i;
        LE monitor = new LE();            // Monitor (objeto compartilhado entre leitores e escritores)
        Leitor[] l = new Leitor[L];       // Threads leitores
        Escritor[] e = new Escritor[E];   // Threads escritores
        LeitorEscritor[] le = new LeitorEscritor[LE]; // Threads leitores/escritores

        //inicia o log de saida
        System.out.println ("import verificaLE");
        System.out.println ("le = verificaLE.LE()");

        for (i=0; i<L; i++) {
            l[i] = new Leitor(i+1, (i+1)*500,monitor);
            l[i].start();
        }
        for (i=0; i<E; i++) {
            e[i] = new Escritor(i+1, (i+1)*500, monitor);
            e[i].start();
        }

        for (i=0; i<E; i++) {
            le[i] = new LeitorEscritor(i+1, (i+1)*500, monitor);
            le[i].start();
        }
    }
}