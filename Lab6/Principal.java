import java.util.Random;
//classe da estrutura de dados (recursos) compartilhado entre as threads
class S {
	//recurso compartilhado
	private int r = 0;

	//operacao de escrita sobre o recurso compartilhado
	public synchronized void inc(){
		this.r++;
	}

	//operacao de leitura sobre o recurso compartilhado
	public synchronized int get(){
		return this.r;
	}
}


//classe que estende Runnable e implementa a tarefa de cada thread do programa
class Thrd implements Runnable {
	//identificador da thread
	private int id;
	//objetos compartilhados com outras threads
	S s;
	public int[] vetor;
	public int nthreads;

	//construtor
	public Thrd(int tid, int[] vetor, int nthreads, S s) {
		this.id = tid;
		this.vetor = vetor;
		this.nthreads = nthreads;
		this.s = s;
	}

	//método main da thread
	public void run() {
		for(int i = this.id; i < this.vetor.length; i += nthreads) {
			if(vetor[i] % 2 == 0){
				this.s.inc();
			}
		}
	}
}

//classe da aplicação
class Principal {

	static void corretude(S s, int[] vetor) {
		int total = 0;
		for (int i = 0; i < vetor.length; i++) {
			if (vetor[i] % 2 == 0)
				total++;
		}

		int sequencial = total;
		int concorrente = s.get();

		if (sequencial == concorrente) {
			System.out.println("Esta correto!");
		} else {
			System.out.println("Algum valor esta errado!");
		}
	}

	public static void main (String[] args) {
		if(args.length < 2) {
			System.out.println("java Principal <numero de elementos> <numero de threads>");
			System.exit(1);
		}
		//número de elementos do vetor
		int tam = Integer.parseInt(args[0]);
		//número de threads
		int nthreads = Integer.parseInt(args[1]) > tam ? tam : Integer.parseInt(args[1]);
		//reserva espaço para um vetor de threads
		Thread[] threads = new Thread[nthreads];

		S s = new S();
		Random random = new Random();

		//cria o vetor compartilhado entre as threads
		int[] vetor = new int[tam];
		for(int i = 0; i < tam; i++) {
			vetor[i] = random.nextInt();
		}

		//cria as threads da aplicacao
		for(int i = 0; i < threads.length; i++) {
		   threads[i] = new Thread(new Thrd(i, vetor, nthreads, s));
		}

		//inicia as threads
		for(int i = 0; i < threads.length; i++) {
		   threads[i].start();
		}

		//espera pelo termino de todas as threads
		for(int i = 0; i < threads.length; i++) {
		   try {
			   threads[i].join();
			}
			catch (InterruptedException e) {
				return;
			}
		}

		corretude(s, vetor);
	}
}
