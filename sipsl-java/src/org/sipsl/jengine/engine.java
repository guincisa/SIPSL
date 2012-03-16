package org.sipsl.jengine;

import java.util.LinkedList;
import java.util.Queue;


public class engine {
	
	private static class QQ{
		
	    Queue<String> queue=new LinkedList<String>();	
	
	    public synchronized void put(String message){
	    	queue.add(message);
	    	notify();
	    }
	    public synchronized String get(){
			try {
				wait();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	    	return queue.poll();
	    }

	}
	
	static QQ MainQ = new QQ();
	
    
    private static class Worker
    implements Runnable {
	    public void run() {
	    	while(true){
	    		String message = MainQ.get();
		    	parse(message);
	    	}
	    }
    };

    
	public static void parse(String message){
		System.out.println(">>> Parsing " + message);
		try {
			Thread.sleep(5000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println("<<< Parsed " + message);
	}
	
	public void p_w(String message){
		MainQ.put(message);
	}
	
	Thread t[] = new Thread[10];

	
	engine() {

        for(int i = 0 ; i <10 ;i ++ ){
        	t[i] = new Thread(new Worker());
			t[i].start();
        }
		Thread.yield();
		System.out.println("You shoudl not see this!");
		
	}
	
}
